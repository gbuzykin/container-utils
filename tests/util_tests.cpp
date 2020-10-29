#include "core/set.h"
#include "core/util_algorithm.h"
#include "core/util_span.h"

#include "tests.h"

#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

// --------------------------------------------

static void test_0() {
    std::pair<std::string, int> p1{"Hello", 10};
    std::tuple<std::string, int, double> p2{"Hello", 10, 2.5};
    std::string p3{"Hello"};

    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<0>()(p1))>, std::string>::value);
    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<1>()(p1))>, int>::value);
    VERIFY(util::get_n<0>()(p1) == "Hello");

    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<0>()(p2))>, std::string>::value);
    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<1>()(p2))>, int>::value);
    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<2>()(p2))>, double>::value);
    VERIFY(util::get_n<0>()(p2) == "Hello");

    VERIFY(std::is_same<std::decay_t<decltype(util::get_n<0>()(p3))>, std::string>::value);
    VERIFY(util::get_n<0>()(p3) == "Hello");

    std::vector<std::tuple<std::string, int, double>> v1, v2;
    for (const auto& item : v1) { util::binary_insert_new(v2, item); }
}

std::vector<std::string> g_vec;

void g_func(const std::string& s) { g_vec.emplace_back(s); }

static void test_1() {
    auto it3 = util::function_caller(&g_func);
    auto it4 = util::function_caller(&g_func);
    it3 = it4;

    g_vec.clear();
    *it3 = "aaa";
    VERIFY(g_vec[0] == "aaa");

    std::string s2;
    auto l2 = [=](const std::string& s) { g_vec.emplace_back(s2 + s); };

    auto it5 = util::function_caller(l2);
    auto it6 = util::function_caller(l2);
    it5 = it6;

    g_vec.clear();
    *it5 = "bbb";
    VERIFY(g_vec[0] == "bbb");

    auto l = [](const std::string& s) { g_vec.emplace_back(s); };

    auto it7 = util::function_caller(l);
    auto it8 = util::function_caller(l);
    it7 = it8;

    g_vec.clear();
    *it7 = "ccc";
    VERIFY(g_vec[0] == "ccc");

    struct F {
        void operator()(const std::string& s) const { g_vec.emplace_back(s); }
    } f;

    auto it9 = util::function_caller(f);
    auto it10 = util::function_caller(f);
    it9 = it10;

    g_vec.clear();
    *it9 = "ddd";
    VERIFY(g_vec[0] == "ddd");
}

static void test_2() {
    std::vector<std::string> strs{"aaa", "bbb", "ccc"};
    g_vec.clear();
    std::copy(strs.begin(), strs.end(), util::function_caller([](const std::string& s) { g_vec.emplace_back(s); }));
    VERIFY(g_vec[0] == "aaa");
    VERIFY(g_vec[1] == "bbb");
    VERIFY(g_vec[2] == "ccc");
}

template<typename Ty1, typename Ty2>
inline bool operator==(const std::pair<const Ty1, Ty2>& p1, const std::pair<Ty1, Ty2>& p2) {
    return (p1.first == p2.first) && (p1.second == p2.second);
}

static void test_3() {
    std::vector<std::string> v{"a", "b", "c", "d", "e"};
    VERIFY(util::find(v, "d") == std::make_pair(std::find(v.begin(), v.end(), "d"), true));
    VERIFY(util::find(v, "f") == std::make_pair(v.end(), false));
    VERIFY(util::contains(v, "d"));
    VERIFY(!util::contains(v, "f"));
    VERIFY(util::find_if(v, [](const std::string& s) { return s >= "d"; }) ==
           std::make_pair(std::find(v.begin(), v.end(), "d"), true));
    VERIFY(util::find_if(v, [](const std::string& s) { return s >= "f"; }) == std::make_pair(v.end(), false));
    VERIFY(util::any_of(v, [](const std::string& s) { return s >= "d"; }));
    VERIFY(!util::any_of(v, [](const std::string& s) { return s >= "f"; }));
    VERIFY(util::all_of(v, [](const std::string& s) { return s >= "a"; }));
    VERIFY(!util::all_of(v, [](const std::string& s) { return s >= "b"; }));
    VERIFY(util::none_of(v, [](const std::string& s) { return s >= "f"; }));
    VERIFY(!util::none_of(v, [](const std::string& s) { return s >= "d"; }));

    std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}};
    using value_type = std::decay_t<decltype(m)>::value_type;
    VERIFY(util::find(m, "d") == std::make_pair(m.find("d"), true));
    VERIFY(util::find(m, value_type{"d", 4}) == std::make_pair(m.find("d"), true));
    VERIFY(util::find(m, value_type{"d", 5}) == std::make_pair(m.end(), false));
    VERIFY(util::find(m, "f") == std::make_pair(m.end(), false));
    VERIFY(util::contains(m, "d"));
    VERIFY(util::contains(m, value_type{"d", 4}));
    VERIFY(!util::contains(m, value_type{"d", 5}));
    VERIFY(!util::contains(m, "f"));

    std::vector<std::string> v1{"a", "b", "c", "d"};
    std::vector<std::string> v2{"d", "c", "b", "a"};
    VERIFY(util::equal(util::reverse_range(v1), v2.begin()) == true);

    std::vector<std::string> v3{"a", "b", "b", "c", "d", "b"};
    VERIFY(util::count(v3, "b") == 3);
    VERIFY(util::count_if(v3, [](decltype(*v1.cbegin()) el) { return el == "b"; }) == 3);
}

static void test_4() {
    std::vector<std::string> v{"a", "b", "b", "b", "c", "c", "c", "c", "d", "e"};
    VERIFY(*util::erase_one(v, "d") == "e");
    VERIFY(util::erase_one(v, "f") == v.end());
    VERIFY((util::erase(v, "b") == 3) && (v.size() == 6));
    VERIFY((util::erase_if(v, [](const std::string& s) { return s == "c"; }) == 4) && (v.size() == 2));

    std::list<std::string> l{"a", "b", "b", "b", "c", "c", "c", "c", "d", "e"};
    VERIFY(*util::erase_one(l, "d") == "e");
    VERIFY(util::erase_one(l, "f") == l.end());
    VERIFY((util::erase(l, "b") == 3) && (l.size() == 6));
    VERIFY((util::erase_if(l, [](const std::string& s) { return s == "c"; }) == 4) && (l.size() == 2));

    std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}};
    using value_type = std::decay_t<decltype(m)>::value_type;
    VERIFY(*util::erase_one(m, "d") == value_type{"e", 5});
    VERIFY(util::erase_one(m, "f") == m.end());
    VERIFY((util::erase(m, value_type{"b", 2}) == 1) && (m.size() == 3));
    VERIFY((util::erase(m, value_type{"c", 4}) == 0) && (m.size() == 3));
    VERIFY((util::erase_if(m, [](const value_type& i) { return i.first == "c"; }) == 1) && (m.size() == 2));

    std::multiset<std::string> ms{"a", "b", "b", "b", "c", "c", "c", "c", "d", "e"};
    VERIFY((util::erase(ms, util::make_range(ms.equal_range("b"))) == 3) && (ms.size() == 7));

    util::set<std::string, util::less<>> s;
    // util::erase(s, "a");
    util::erase(s, s);

    std::vector<std::string> v1;
    util::erase(v1, v1);
    util::erase(v1, "aa" /*, "aa"*/);
}

static void test_5() {
    std::vector<std::string> v{"a", "b", "b", "b", "c", "c", "c", "c", "d", "e"};
    util::unique(v);
    VERIFY(v == decltype(v){"a", "b", "c", "d", "e"});

    std::list<std::string> l{"a", "b", "b", "b", "c", "c", "c", "c", "d", "e"};
    util::unique(l);
    VERIFY(l == decltype(l){"a", "b", "c", "d", "e"});
}

static void test_6() {
    std::vector<std::string> v{"a", "b", "c", "d", "e"};
    util::emplace_at(v, 3, "f");
    VERIFY(v == decltype(v){"a", "b", "c", "f", "d", "e"});
    util::erase_at(v, 2);
    VERIFY(v == decltype(v){"a", "b", "f", "d", "e"});
}

static void test_7() {
    std::vector<std::pair<std::string, int>> v{{"a", 0}, {"b", 1}, {"b", 2}, {"b", 3}, {"c", 4},
                                               {"c", 5}, {"c", 6}, {"c", 7}, {"d", 8}, {"e", 9}};

    VERIFY(util::lower_bound(v, "b") == util::find_if(v, util::is_equal_to("b")).first);
    VERIFY(util::upper_bound(v, "b") == util::find_if(v, util::is_equal_to("c")).first);
    VERIFY(util::equal_range(v, "b") == util::make_range(util::find_if(v, util::is_equal_to("b")).first,
                                                         util::find_if(v, util::is_equal_to("c")).first));

    VERIFY(util::lower_bound(v, "cc") == util::find_if(v, util::is_equal_to("d")).first);
    VERIFY(util::upper_bound(v, "cc") == util::find_if(v, util::is_equal_to("d")).first);
    VERIFY(util::equal_range(v, "cc") == util::make_range(util::find_if(v, util::is_equal_to("d")).first,
                                                          util::find_if(v, util::is_equal_to("d")).first));

    VERIFY(util::binary_find(v, "c") == util::find_if(v, util::is_equal_to("c")));
    VERIFY(util::binary_find(v, "cc") == std::make_pair(util::find_if(v, util::is_equal_to("d")).first, false));

    VERIFY(util::binary_contains(v, "c"));
    VERIFY(!util::binary_contains(v, "cc"));
}

static void test_8() {
    std::vector<std::pair<std::string, int>> v{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}};
    using value_type = std::decay_t<decltype(v)>::value_type;

    auto result = util::binary_insert_unique(v, std::make_pair("cc", 30));
    VERIFY(result.second && (v.size() == 6) && (*result.first == value_type{"cc", 30}) &&
           (*std::next(result.first) == value_type{"d", 4}));

    result = util::binary_insert_unique(v, std::make_pair("c", 35));
    VERIFY(!result.second && (v.size() == 6) && (*result.first == value_type{"c", 3}) &&
           (*std::next(result.first) == value_type{"cc", 30}));

    util::binary_access_unique(v, "bb").second = 40;
    result = util::binary_find(v, "bb");
    VERIFY(result.second && (v.size() == 7) && (*result.first == value_type{"bb", 40}) &&
           (*std::next(result.first) == value_type{"c", 3}));

    util::binary_access_unique(v, "bb").second = 41;
    result = util::binary_find(v, "bb");
    VERIFY(result.second && (v.size() == 7) && (*result.first == value_type{"bb", 41}) &&
           (*std::next(result.first) == value_type{"c", 3}));

    auto it = util::binary_insert_new(v, std::make_pair("cc", 35));
    VERIFY((v.size() == 8) && (*it == value_type{"cc", 35}) && (*std::next(it) == value_type{"cc", 30}));

    util::binary_access_new(v, "bb").second = 45;
    result = util::binary_find(v, "bb");
    VERIFY(result.second && (v.size() == 9) && (*result.first == value_type{"bb", 45}) &&
           (*std::next(result.first) == value_type{"bb", 41}));

    VERIFY(*util::binary_erase_one(v, "cc") == value_type{"cc", 30});
    VERIFY(v.size() == 8);

    VERIFY(util::erase(v, util::equal_range(v, "bb")) == 2);
    VERIFY(v.size() == 6);

    VERIFY(*util::binary_erase_one(v, "bb") == value_type{"c", 3});
    VERIFY(v.size() == 6);
}

static void test_9() {
    int* p = nullptr;
    const int* cp = nullptr;
    util::span<int> s(p, 10);
    util::span<const int> s2(p, 10);
    util::span<const int> s3(cp, 10);
    // util::span<int> s4(cp, 10);

    std::vector<int> v;
    util::span<int> s5(v);
    util::span<const int> s6(v);
    util::span<const int> s7(std::as_const(v));
    // util::span<int> s8(std::as_const(v));

    util::span<const int> s9(s5);
    // util::span<int> s10(s6);
}

// --------------------------------------------

std::pair<std::pair<size_t, void (*)()>*, size_t> get_util_tests() {
    static std::pair<size_t, void (*)()> _tests[] = {
        {0, test_0}, {1, test_1}, {2, test_2}, {3, test_3}, {4, test_4},
        {5, test_5}, {6, test_6}, {7, test_7}, {8, test_8}, {9, test_9},
    };

    return std::make_pair(_tests, sizeof(_tests) / sizeof(_tests[0]));
}
