#include "core/pool_allocator.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define USE_UTIL
#define USE_STD

#define VERIFY(...) \
    if (!(__VA_ARGS__)) { throw std::logic_error(report_error(__FILE__, __LINE__, #__VA_ARGS__)); }

inline std::string report_error(const char* file, int line, const char* msg) {
    std::stringstream ss;
    ss << file << "(" << line << "): " << msg << std::flush;
    return ss.str();
}

struct T {
    std::string text;
    static std::int64_t cnt;
    static std::int64_t inst_cnt;
    static std::int64_t comp_cnt;

    T() { ++inst_cnt; }
    T(int a) : text(std::to_string(a)) {
        ++inst_cnt;
        ++cnt;
    }
    T(const T& t) : text(t.text) {
        ++inst_cnt;
        if (!text.empty()) { ++cnt; }
    }
    T(T&& t) : text(std::move(t.text)) {
        ++inst_cnt;
        t.text = "";
    }
    ~T() {
        if (!text.empty()) { --cnt; }
        --inst_cnt;
    }

    T& operator=(const T& t) {
        if (!text.empty()) { --cnt; }
        text = t.text;
        if (!text.empty()) { ++cnt; }
        return *this;
    }
    T& operator=(T&& t) {
        if (!text.empty()) { --cnt; }
        text = std::move(t.text);
        t.text = "";
        return *this;
    }

    operator std::string() const { return "'" + text + "'"; }
    explicit operator int() const {
        int v = 0;
        for (char ch : text) { v = 10 * v + ch - '0'; }
        return v;
    }

    friend bool operator==(const T& t1, const T& t2) { return static_cast<int>(t1) == static_cast<int>(t2); }
    friend bool operator<(const T& t1, const T& t2) {
        ++comp_cnt;
        return static_cast<int>(t1) < static_cast<int>(t2);
    }
    friend bool operator==(const T& t, int a) { return static_cast<int>(t) == a; }
    friend bool operator<(const T& t, int a) {
        ++comp_cnt;
        return static_cast<int>(t) < a;
    }
    friend bool operator==(int a, const T& t) { return a == static_cast<int>(t); }
    friend bool operator<(int a, const T& t) {
        ++comp_cnt;
        return a < static_cast<int>(t);
    }
};

template<typename Ty>
class unfriendly_pool_allocator : public util::pool_allocator<Ty> {
 private:
    using super = util::pool_allocator<Ty>;

 public:
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::false_type;

    unfriendly_pool_allocator() = default;
    explicit unfriendly_pool_allocator(uint32_t partition_size) : super(partition_size) {}
    template<typename Ty2>
    unfriendly_pool_allocator(const unfriendly_pool_allocator<Ty2>& other) : super(other) {}
    template<typename Ty2>
    unfriendly_pool_allocator& operator=(const unfriendly_pool_allocator<Ty2>& other) {
        super::operator=(other);
        return *this;
    }

    template<typename Ty2>
    unfriendly_pool_allocator(unfriendly_pool_allocator<Ty2>&& other) : super(std::move(other)) {}
    template<typename Ty2>
    unfriendly_pool_allocator& operator=(unfriendly_pool_allocator<Ty2>&& other) {
        super::operator=(std::move(other));
        return *this;
    }

    template<typename Ty2>
    bool operator==(const unfriendly_pool_allocator<Ty2>& other) const {
        return static_cast<const super&>(*this) == static_cast<const super&>(other);
    }
    template<typename Ty2>
    bool operator!=(const unfriendly_pool_allocator<Ty2>& other) const {
        return !(*this == other);
    }
};
