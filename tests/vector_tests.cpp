#include "core/vector.h"

#include "tests.h"

#include <vector>

#ifdef _DEBUG
static const int N = 100000;
#else   // _DEBUG
static const int N = 10000000;
#endif  // _DEBUG

template<typename Ty, typename Alloc, typename InputIt>
bool check_vector(const util::vector<Ty, Alloc>& v, size_t sz, InputIt src) {
    if (v.size() != sz) { return false; }
    if (v.end() - v.begin() != sz) { return false; }
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (!(*it == *src++)) { return false; }
    }
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        if (!(*it == *(--src))) { return false; }
    }
    return true;
}

#define CHECK(...) \
    if (!check_vector(__VA_ARGS__)) { throw std::logic_error(report_error(__FILE__, __LINE__, "vector mismatched")); }

#define CHECK_EMPTY(...) \
    if (((__VA_ARGS__).size() != 0) || ((__VA_ARGS__).begin() != (__VA_ARGS__).end())) { \
        throw std::logic_error(report_error(__FILE__, __LINE__, "vector is not empty")); \
    }

// --------------------------------------------

static void test_0() {  // empty vector
    VERIFY(util::is_random_access_iterator<std::vector<T>::iterator>::value);
    VERIFY(util::is_random_access_iterator<util::vector<T>::iterator>::value);

    util::pool_allocator<void> al;

    util::vector<T> v;
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    util::vector<T, util::pool_allocator<T>> v1(al);
    CHECK_EMPTY(v1);
    VERIFY(v1.capacity() == 0);
    VERIFY(v1.get_allocator() == al);

    v.insert(v.end(), 10, 1);
}

static void test_1() {  // vector with size initialized by default
    util::pool_allocator<void> al;

    util::vector<T, util::pool_allocator<T>> v0(0, al);
    CHECK_EMPTY(v0);
    VERIFY(v0.capacity() == 0);
    VERIFY(v0.get_allocator() == al);

    T tst[5];
    util::vector<T, util::pool_allocator<T>> v(5, al);
    CHECK(v, 5, tst);
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
}

static void test_2() {  // vector with size initialized with given value
    util::pool_allocator<void> al;

    util::vector<T, util::pool_allocator<T>> v0(0, 10, al);
    CHECK_EMPTY(v0);
    VERIFY(v0.capacity() == 0);
    VERIFY(v0.get_allocator() == al);

    T tst[] = {10, 10, 10, 10, 10};
    util::vector<T, util::pool_allocator<T>> v(5, 10, al);
    CHECK(v, 5, tst);
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
}

static void test_3() {  // vector initialized with iterator range
    util::pool_allocator<void> al;

    T tst0[] = {1};
    util::vector<T, util::pool_allocator<T>> v0(tst0, tst0, al);
    CHECK_EMPTY(v0);
    VERIFY(v0.capacity() == 0);
    VERIFY(v0.get_allocator() == al);

    T tst[] = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v(tst, tst + 5, al);
    CHECK(v, 5, tst);
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
}

static void test_4() {  // vector with initializer
    util::pool_allocator<void> al;

    std::initializer_list<T> tst0;
    util::vector<T, util::pool_allocator<T>> v0(tst0, al);
    CHECK_EMPTY(v0);
    VERIFY(v0.capacity() == 0);
    VERIFY(v0.get_allocator() == al);

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v(tst, al);
    CHECK(v, tst.size(), tst.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
}

static void test_5() {  // initializer assignment
    util::vector<T, util::pool_allocator<T>> v;

    std::initializer_list<T> tst0;
    v = tst0;  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    v = tst1;  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());

    v.push_back(6);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7};
    auto cap0 = v.capacity();
    v = tst2;  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    v = tst3;  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto cap1 = v.capacity();
    v = tst4;  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    v = tst5;  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);

    std::initializer_list<T> tst6;
    v = tst6;  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
}

// --------------------------------------------

static void test_6() {  // copy constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v0(al1), v(tst, al1);

    util::vector<T, util::pool_allocator<T>> v01(v0);
    CHECK_EMPTY(v01);
    VERIFY(v01.capacity() == 0);
    VERIFY(v01.get_allocator() == al1);

    util::vector<T, util::pool_allocator<T>> v02(v0, al2);
    CHECK_EMPTY(v02);
    VERIFY(v02.capacity() == 0);
    VERIFY(v02.get_allocator() == al2);

    util::vector<T, util::pool_allocator<T>> v1(v);
    CHECK(v1, tst.size(), tst.begin());
    VERIFY(v1.capacity() >= v1.size());
    VERIFY(v1.get_allocator() == al1);

    util::vector<T, util::pool_allocator<T>> v2(v, al2);
    CHECK(v2, tst.size(), tst.begin());
    VERIFY(v2.capacity() >= v2.size());
    VERIFY(v2.get_allocator() == al2);
}

static void test_7() {  // move constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v0(al1), v(tst, al1);

    util::vector<T, util::pool_allocator<T>> v01(std::move(v0));
    CHECK_EMPTY(v01);
    VERIFY(v01.capacity() == 0);
    VERIFY(v01.get_allocator() == al1);

    util::vector<T, util::pool_allocator<T>> v02(std::move(v01), al2);
    CHECK_EMPTY(v02);
    VERIFY(v02.capacity() == 0);
    VERIFY(v02.get_allocator() == al2);

    util::vector<T, util::pool_allocator<T>> v03(std::move(v02), al2);
    CHECK_EMPTY(v03);
    VERIFY(v03.capacity() == 0);
    VERIFY(v03.get_allocator() == al2);

    util::vector<T, util::pool_allocator<T>> v1(std::move(v));
    CHECK(v1, tst.size(), tst.begin());
    VERIFY(v1.capacity() >= v1.size());
    VERIFY(v1.get_allocator() == al1);
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    util::vector<T, util::pool_allocator<T>> v2(std::move(v1), al2);
    CHECK(v2, tst.size(), tst.begin());
    VERIFY(v2.capacity() >= v2.size());
    VERIFY(v2.get_allocator() == al2);
    VERIFY(v1.size() == v2.size());  // different allocators -> per-element movement

    util::vector<T, util::pool_allocator<T>> v3(std::move(v2), al2);
    CHECK(v3, tst.size(), tst.begin());
    VERIFY(v3.capacity() >= v3.size());
    VERIFY(v3.get_allocator() == al2);
    CHECK_EMPTY(v2);
    VERIFY(v2.capacity() == 0);
}

// --------------------------------------------

static void test_8() {  // copy assignment, same allocator
    util::pool_allocator<void> al;
    util::vector<T, util::pool_allocator<T>> v(al);

    util::vector<T, util::pool_allocator<T>> v0(al);
    v = v0;  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v1(tst1, al);
    v = v1;  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);

    v.push_back(6);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7};
    util::vector<T, util::pool_allocator<T>> v2(tst2, al);
    auto cap0 = v.capacity();
    v = v2;  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);
    VERIFY(v.get_allocator() == al);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, util::pool_allocator<T>> v3(tst3, al);
    v = v3;  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::vector<T, util::pool_allocator<T>> v4(tst4, al);
    auto cap1 = v.capacity();
    v = v4;  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::vector<T, util::pool_allocator<T>> v5(tst5, al);
    v = v5;  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al);

    util::vector<T, util::pool_allocator<T>> v6(al);
    v = v6;  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al);
}

static void test_9() {  // copy assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::vector<T, util::pool_allocator<T>> v(al1);

    util::vector<T, util::pool_allocator<T>> v0(al2);
    v = v0;  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al1);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v1(tst1, al2);
    v = v1;  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);

    v.push_back(6);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7};
    util::vector<T, util::pool_allocator<T>> v2(tst2, al2);
    auto cap0 = v.capacity();
    v = v2;  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);
    VERIFY(v.get_allocator() == al1);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, util::pool_allocator<T>> v3(tst3, al2);
    v = v3;  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::vector<T, util::pool_allocator<T>> v4(tst4, al2);
    auto cap1 = v.capacity();
    v = v4;  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::vector<T, util::pool_allocator<T>> v5(tst5, al2);
    v = v5;  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);

    util::vector<T, util::pool_allocator<T>> v6(al2);
    v = v6;  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);
}

static void test_10() {  // copy assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::vector<T, unfriendly_pool_allocator<T>> v(al1);

    util::vector<T, unfriendly_pool_allocator<T>> v0(al2);
    v = v0;  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, unfriendly_pool_allocator<T>> v1(tst1, al1);
    v = v1;  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, unfriendly_pool_allocator<T>> v2(tst2, al2);
    v = v2;  // from non-empty
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al2);

    util::vector<T, unfriendly_pool_allocator<T>> v3(al1);
    v = v3;  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al1);
}

// --------------------------------------------

static void test_11() {  // move assignment, same allocator
    util::pool_allocator<void> al;
    util::vector<T, util::pool_allocator<T>> v(al);

    util::vector<T, util::pool_allocator<T>> v0(al);
    v = std::move(v0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v1(tst1, al);
    v = std::move(v1);  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
    CHECK_EMPTY(v1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, util::pool_allocator<T>> v2(tst2, al);
    v = std::move(v2);  // from non-empty
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al);
    CHECK_EMPTY(v2);

    util::vector<T, util::pool_allocator<T>> v3(al);
    v = std::move(v3);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al);
}

static void test_12() {  // move assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::vector<T, util::pool_allocator<T>> v(al1);

    util::vector<T, util::pool_allocator<T>> v0(al2);
    v = std::move(v0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, util::pool_allocator<T>> v1(tst1, al1);
    v = std::move(v1);  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);
    CHECK_EMPTY(v1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, util::pool_allocator<T>> v2(tst2, al2);
    v = std::move(v2);  // from non-empty
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al2);
    CHECK_EMPTY(v2);

    util::vector<T, util::pool_allocator<T>> v3(al1);
    v = std::move(v3);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al1);
}

static void test_13() {  // move assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::vector<T, unfriendly_pool_allocator<T>> v(al1);

    util::vector<T, unfriendly_pool_allocator<T>> v0(al2);
    v = std::move(v0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);
    VERIFY(v.get_allocator() == al1);
    VERIFY(v0.size() == v.size());

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::vector<T, unfriendly_pool_allocator<T>> v1(tst1, al2);
    v = std::move(v1);  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);
    VERIFY(v1.size() == v.size());

    v.push_back(6);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7};
    util::vector<T, unfriendly_pool_allocator<T>> v2(tst2, al2);
    auto cap0 = v.capacity();
    v = std::move(v2);  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);
    VERIFY(v.get_allocator() == al1);
    VERIFY(v2.size() == v.size());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::vector<T, unfriendly_pool_allocator<T>> v3(tst3, al2);
    v = std::move(v3);  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());
    VERIFY(v.get_allocator() == al1);
    VERIFY(v3.size() == v.size());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::vector<T, unfriendly_pool_allocator<T>> v4(tst4, al2);
    auto cap1 = v.capacity();
    v = std::move(v4);  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);
    VERIFY(v4.size() == v.size());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::vector<T, unfriendly_pool_allocator<T>> v5(tst5, al2);
    v = std::move(v5);  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);
    VERIFY(v5.size() == v.size());

    util::vector<T, unfriendly_pool_allocator<T>> v6(al2);
    v = std::move(v6);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
    VERIFY(v.get_allocator() == al1);
    VERIFY(v6.size() == v.size());
}

// --------------------------------------------

static void test_14() {  // assignment from given size and value
    util::vector<T, util::pool_allocator<T>> v;

    v.assign(0, 10);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    std::initializer_list<T> tst1 = {10, 10, 10, 10, 10};
    v.assign(5, 10);  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());

    v.push_back(10);

    std::initializer_list<T> tst2 = {10, 10, 10, 10, 10, 10, 10};
    auto cap0 = v.capacity();
    v.assign(7, 10);  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);

    std::initializer_list<T> tst3 = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    v.assign(16, 10);  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());

    std::initializer_list<T> tst4 = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    auto cap1 = v.capacity();
    v.assign(10, 10);  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);

    std::initializer_list<T> tst5 = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
    v.assign(10, 20);  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);

    v.assign(0, 10);
    CHECK_EMPTY(v);  // to empty
    VERIFY(v.capacity() == cap1);
}

static void test_15() {  // assignment from iterator range
    util::vector<T, util::pool_allocator<T>> v;

    T tst0[] = {1};
    v.assign(tst0, tst0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    T tst1[] = {1, 2, 3, 4, 5};
    v.assign(tst1, tst1 + 5);  // from empty
    CHECK(v, 5, tst1);
    VERIFY(v.capacity() >= v.size());

    v.push_back(6);

    T tst2[] = {1, 2, 3, 4, 5, 6, 7};
    auto cap0 = v.capacity();
    v.assign(tst2, tst2 + 7);  // no reallocation
    CHECK(v, 7, tst2);
    VERIFY(v.capacity() == cap0);

    T tst3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    v.assign(tst3, tst3 + 16);  // reallocation
    CHECK(v, 16, tst3);
    VERIFY(v.capacity() >= v.size());

    T tst4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto cap1 = v.capacity();
    v.assign(tst4, tst4 + 10);  // to less size
    CHECK(v, 10, tst4);
    VERIFY(v.capacity() == cap1);

    T tst5[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    v.assign(tst5, tst5 + 10);  // to the same size
    CHECK(v, 10, tst5);
    VERIFY(v.capacity() == cap1);

    T tst6[] = {1};
    v.assign(tst6, tst6);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
}

static void test_16() {  // assignment from iterator range of different type
    util::vector<T, util::pool_allocator<T>> v;

    int tst0[] = {1};
    v.assign(tst0, tst0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    int tst1[] = {1, 2, 3, 4, 5};
    v.assign(tst1, tst1 + 5);  // from empty
    CHECK(v, 5, tst1);
    VERIFY(v.capacity() >= v.size());

    v.push_back(6);

    int tst2[] = {1, 2, 3, 4, 5, 6, 7};
    auto cap0 = v.capacity();
    v.assign(tst2, tst2 + 7);  // no reallocation
    CHECK(v, 7, tst2);
    VERIFY(v.capacity() == cap0);

    int tst3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    v.assign(tst3, tst3 + 16);  // reallocation
    CHECK(v, 16, tst3);
    VERIFY(v.capacity() >= v.size());

    int tst4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto cap1 = v.capacity();
    v.assign(tst4, tst4 + 10);  // to less size
    CHECK(v, 10, tst4);
    VERIFY(v.capacity() == cap1);

    int tst5[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    v.assign(tst5, tst5 + 10);  // to the same size
    CHECK(v, 10, tst5);
    VERIFY(v.capacity() == cap1);

    int tst6[] = {1};
    v.assign(tst6, tst6);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
}

static void test_17() {  // assignment from initializer
    util::vector<T, util::pool_allocator<T>> v;

    std::initializer_list<T> tst0;
    v.assign(tst0);  // from empty to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == 0);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    v.assign(tst1);  // from empty
    CHECK(v, tst1.size(), tst1.begin());
    VERIFY(v.capacity() >= v.size());

    v.push_back(6);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7};
    auto cap0 = v.capacity();
    v.assign(tst2);  // no reallocation
    CHECK(v, tst2.size(), tst2.begin());
    VERIFY(v.capacity() == cap0);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    v.assign(tst3);  // reallocation
    CHECK(v, tst3.size(), tst3.begin());
    VERIFY(v.capacity() >= v.size());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto cap1 = v.capacity();
    v.assign(tst4);  // to less size
    CHECK(v, tst4.size(), tst4.begin());
    VERIFY(v.capacity() == cap1);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    v.assign(tst5);  // to the same size
    CHECK(v, tst5.size(), tst5.begin());
    VERIFY(v.capacity() == cap1);

    std::initializer_list<T> tst6;
    v.assign(tst6);  // to empty
    CHECK_EMPTY(v);
    VERIFY(v.capacity() == cap1);
}

// --------------------------------------------

static void test_18() {  // swap
    util::pool_allocator<void> al1, al2;
    util::vector<T, util::pool_allocator<T>> v1(al1), v2(al2);

    v1.swap(v2);
    CHECK_EMPTY(v1);
    VERIFY(v1.capacity() == 0);
    CHECK_EMPTY(v2);
    VERIFY(v2.capacity() == 0);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    v1.assign(tst1);
    v1.swap(v2);
    CHECK_EMPTY(v1);
    VERIFY(v1.capacity() == 0);
    CHECK(v2, tst1.size(), tst1.begin());
    VERIFY(v2.capacity() >= v2.size());

    v1.swap(v2);
    CHECK(v1, tst1.size(), tst1.begin());
    VERIFY(v1.capacity() >= v1.size());
    CHECK_EMPTY(v2);
    VERIFY(v2.capacity() == 0);

    std::initializer_list<T> tst2 = {10, 11, 12, 13, 14, 15, 16, 17};
    v2.assign(tst2);
    v1.swap(v2);
    CHECK(v1, tst2.size(), tst2.begin());
    VERIFY(v1.capacity() >= v1.size());
    CHECK(v2, tst1.size(), tst1.begin());
    VERIFY(v2.capacity() >= v2.size());
}

// --------------------------------------------

template<typename Ty>
static void vector_test(int iter_count, bool log = false) {
    util::vector<Ty> v;
    std::vector<Ty> v_ref;

    srand(0);

    int iter = 0, perc0 = 0;
    std::cout << "  0.0%" << std::flush;
    for (; iter < iter_count; ++iter) {
        int perc = (1000 * static_cast<int64_t>(iter)) / iter_count;
        if (perc > perc0) {
            std::cout << "\b\b\b\b\b\b" << std::setw(3) << (perc / 10) << "." << std::setw(0) << (perc % 10) << "%"
                      << std::flush;
            perc0 = perc;
        }

        int act = rand() % 65;
        if ((act >= 0) && (act < 10)) {
            if (log) { std::cout << "insert one" << std::endl; }

            size_t n = rand() % (v.size() + 1);
            char val = rand() % 100;
            auto t_res = v.emplace(v.begin() + n, val);
            auto ref_res = v_ref.emplace(v_ref.begin() + n, val);
            VERIFY(std::distance(v.begin(), t_res) == std::distance(v_ref.begin(), ref_res));
        } else if ((act >= 10) && (act < 20)) {
            if (log) { std::cout << "insert" << std::endl; }

            size_t n = rand() % (v.size() + 1);
            size_t count = 1 + rand() % 5;

            Ty val[10];
            for (size_t i = 0; i < count; ++i) { val[i] = rand() % 100; }

            auto t_res = v.insert(v.begin() + n, (Ty*)val, (Ty*)val + count);
            auto ref_res = v_ref.insert(v_ref.begin() + n, (Ty*)val, (Ty*)val + count);
            VERIFY(std::distance(v.begin(), t_res) == std::distance(v_ref.begin(), ref_res));
        } else if ((act >= 20) && (act < 30)) {
            if (!v.empty()) {
                if (log) { std::cout << "erase one" << std::endl; }

                size_t n = rand() % v.size();
                auto t_res = v.erase(v.begin() + n);
                auto ref_res = v_ref.erase(v_ref.begin() + n);
                VERIFY(std::distance(v.begin(), t_res) == std::distance(v_ref.begin(), ref_res));
            }
        } else if ((act >= 30) && (act < 40)) {
            if (!v.empty()) {
                if (log) { std::cout << "erase" << std::endl; }

                size_t n = rand() % (v.size() + 1);
                size_t count = rand() % (v.size() + 1 - n);
                auto t_res = v.erase(v.begin() + n, v.begin() + n + count);
                auto ref_res = v_ref.erase(v_ref.begin() + n, v_ref.begin() + n + count);
                VERIFY(std::distance(v.begin(), t_res) == std::distance(v_ref.begin(), ref_res));
            }
        } else if ((act >= 40) && (act < 50)) {
            if (log) { std::cout << "emplace back" << std::endl; }

            char val = rand() % 100;
            v.emplace_back(val);
            v_ref.emplace_back(val);
        } else if ((act >= 50) && (act < 60)) {
            if (!v.empty()) {
                if (log) { std::cout << "pop back" << std::endl; }

                v.pop_back();
                v_ref.pop_back();
            }
        } else if (act == 60) {
            if (log) { std::cout << "shrink to fit" << std::endl; }

            v.shrink_to_fit();
            v_ref.shrink_to_fit();
            VERIFY(v.size() == v.capacity());
        } else if (act == 61) {
            if (log) { std::cout << "clear" << std::endl; }

            v.clear();
            v_ref.clear();
        } else if (act == 62) {
            if (log) { std::cout << "tidy" << std::endl; }

            v.clear();
            v.shrink_to_fit();
            v_ref.clear();
            v_ref.shrink_to_fit();
            VERIFY(v.size() == 0);
            VERIFY(v.capacity() == 0);
        } else if (act == 63) {
            size_t sz = rand() % 100;

            if (log) {
                if (v.size() < sz) {
                    std::cout << "resize default grow" << std::endl;
                } else {
                    std::cout << "resize default trim" << std::endl;
                }
            }

            v.resize(sz);
            v_ref.resize(sz);
        } else if (act == 64) {
            size_t sz = rand() % 100;

            if (log) {
                if (v.size() < sz) {
                    std::cout << "resize grow" << std::endl;
                } else {
                    std::cout << "resize trim" << std::endl;
                }
            }

            char val = rand() % 100;
            v.resize(sz, val);
            v_ref.resize(sz, val);
        }

        CHECK(v, v_ref.size(), v_ref.begin());

        if (!v.empty()) {
            VERIFY((v[0] == v_ref[0]) && (v[v.size() - 1] == v_ref[v_ref.size() - 1]));
            VERIFY((v.front() == v_ref.front()) && (v.back() == v_ref.back()));
        }
    }

    std::cout << "\b\b\b\b\b\b" << std::flush;
}

static void test_100() {
#if defined(USE_UTIL) && defined(USE_STD)
    vector_test<T>(10 * N);
#endif
}

// --------------------------------------------

template<typename VecType>
static void performance(int iter_count) {
    VecType v;

    srand(0);

    auto start = std::clock();
    using Ty = typename VecType::value_type;
    const Ty val0 = 10;

    for (int iter = 0; iter < iter_count; ++iter) {
        int act = rand() % 64;
        if ((act >= 0) && (act < 10)) {
            v.emplace(v.begin() + rand() % (v.size() + 1), val0);
        } else if ((act >= 10) && (act < 20)) {
            size_t n = rand() % (v.size() + 1);
            size_t count = 1 + rand() % 5;
            Ty val[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            v.insert(v.begin() + n, (Ty*)val, (Ty*)val + count);
        } else if ((act >= 20) && (act < 30)) {
            if (!v.empty()) { v.erase(v.begin() + rand() % v.size()); }
        } else if ((act >= 30) && (act < 40)) {
            if (!v.empty()) {
                size_t n = rand() % v.size();
                size_t count = 1 + rand() % (v.size() - n);
                v.erase(v.begin() + n, v.begin() + n + count);
            }
        } else if ((act >= 40) && (act < 50)) {
            v.emplace_back(val0);
        } else if ((act >= 50) && (act < 60)) {
            if (!v.empty()) { v.pop_back(); }
        } else if (act == 60) {
            v.shrink_to_fit();
        } else if (act == 61) {
            v.clear();
        } else if (act == 62) {
            v.resize(rand() % 100);
        } else if (act == 63) {
            v.resize(rand() % 100, val0);
        }
    }

    std::cout << (std::clock() - start) << std::endl;
}

static void test_102() {
#if defined(USE_UTIL)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- util::vector<T> performance..." << std::flush;
    performance<util::vector<T>>(N);
    std::cout << "---------- util::vector<int> performance..." << std::flush;
    performance<util::vector<int>>(N);
    std::cout << "---------- util::vector<char> performance..." << std::flush;
    performance<util::vector<char>>(N);
#endif

#if defined(USE_STD)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- std::vector<T> performance..." << std::flush;
    performance<std::vector<T>>(N);
    std::cout << "---------- std::vector<int> performance..." << std::flush;
    performance<std::vector<int>>(N);
    std::cout << "---------- std::vector<char> performance..." << std::flush;
    performance<std::vector<char>>(N);
#endif
}

// --------------------------------------------

static void test_103() {
    std::cout << std::endl;
    std::cout << "sizeof(util::vector<T>::iterator) = " << sizeof(util::vector<T>::iterator) << std::endl;
    std::cout << "sizeof(util::vector<T>) = " << sizeof(util::vector<T>) << std::endl;
    std::cout << std::endl;
    std::cout << "sizeof(std::vector<T>::iterator) = " << sizeof(std::vector<T>::iterator) << std::endl;
    std::cout << "sizeof(std::vector<T>) = " << sizeof(std::vector<T>) << std::endl;
}

// --------------------------------------------

std::pair<std::pair<size_t, void (*)()>*, size_t> get_vector_tests() {
    static std::pair<size_t, void (*)()> _tests[] = {
        {0, test_0},   {1, test_1},     {2, test_2},     {3, test_3},     {4, test_4},   {5, test_5},
        {6, test_6},   {7, test_7},     {8, test_8},     {9, test_9},     {10, test_10}, {11, test_11},
        {12, test_12}, {13, test_13},   {14, test_14},   {15, test_15},   {16, test_16}, {17, test_17},
        {18, test_18}, {100, test_100}, {102, test_102}, {103, test_103},
    };

    return std::make_pair(_tests, sizeof(_tests) / sizeof(_tests[0]));
}
