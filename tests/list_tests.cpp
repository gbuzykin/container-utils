#include "core/list.h"

#include "tests.h"

#include <list>

#ifdef _DEBUG
static const int N = 100000;
#else   // _DEBUG
static const int N = 10000000;
#endif  // _DEBUG

template<typename Ty, typename Alloc, typename InputIt>
bool check_list(const util::list<Ty, Alloc>& l, size_t sz, InputIt src) {
    if (l.size() != sz) { return false; }
    if (std::distance(l.begin(), l.end()) != sz) { return false; }
    for (auto it = l.begin(); it != l.end(); ++it) {
        if (!(*it == *src++)) { return false; }
    }
    for (auto it = l.rbegin(); it != l.rend(); ++it) {
        if (!(*it == *(--src))) { return false; }
    }
    return true;
}

#define CHECK(...) \
    if (!check_list(__VA_ARGS__)) { throw std::logic_error(report_error(__FILE__, __LINE__, "list mismatched")); }

#define CHECK_EMPTY(...) \
    if (((__VA_ARGS__).size() != 0) || ((__VA_ARGS__).begin() != (__VA_ARGS__).end())) { \
        throw std::logic_error(report_error(__FILE__, __LINE__, "list is not empty")); \
    }

// --------------------------------------------

static void test_0() {  // empty list
    VERIFY(!util::is_random_access_iterator<std::list<T>::iterator>::value);
    VERIFY(!util::is_random_access_iterator<util::list<T, util::pool_allocator<T>>::iterator>::value);

    util::pool_allocator<void> al;

    util::list<T, util::pool_allocator<T>> l;
    CHECK_EMPTY(l);

    util::list<T, util::pool_allocator<T>> l1(al);
    CHECK_EMPTY(l1);
    VERIFY(l1.get_allocator() == al);
}

static void test_1() {  // list with size initialized by default
    util::pool_allocator<void> al;

    util::list<T, util::pool_allocator<T>> l0(0, al);
    CHECK_EMPTY(l0);
    VERIFY(l0.get_allocator() == al);

    T tst[5];
    util::list<T, util::pool_allocator<T>> l(5, al);
    CHECK(l, 5, tst);
    VERIFY(l.get_allocator() == al);
}

static void test_2() {  // list with size initialized with given value
    util::pool_allocator<void> al;

    util::list<T, util::pool_allocator<T>> l0(0, 10, al);
    CHECK_EMPTY(l0);
    VERIFY(l0.get_allocator() == al);

    T tst[] = {10, 10, 10, 10, 10};
    util::list<T, util::pool_allocator<T>> l(5, 10, al);
    CHECK(l, 5, tst);
    VERIFY(l.get_allocator() == al);
}

static void test_3() {  // list initialized with iterator range
    util::pool_allocator<void> al;

    T tst0[] = {1};
    util::list<T, util::pool_allocator<T>> l0(tst0, tst0, al);
    CHECK_EMPTY(l0);
    VERIFY(l0.get_allocator() == al);

    T tst[] = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l(tst, tst + 5, al);
    CHECK(l, 5, tst);
    VERIFY(l.get_allocator() == al);
}

static void test_4() {  // list with initializer
    util::pool_allocator<void> al;

    std::initializer_list<T> tst0;
    util::list<T, util::pool_allocator<T>> l0(tst0, al);
    CHECK_EMPTY(l0);
    VERIFY(l0.get_allocator() == al);

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l(tst, al);
    CHECK(l, tst.size(), tst.begin());
    VERIFY(l.get_allocator() == al);
}

static void test_5() {  // initializer assignment
    util::list<T, util::pool_allocator<T>> l;

    std::initializer_list<T> tst0;
    l = tst0;  // from empty to empty
    CHECK_EMPTY(l);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    l = tst1;  // from empty
    CHECK(l, tst1.size(), tst1.begin());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    l = tst3;  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l = tst4;  // to less size
    CHECK(l, tst4.size(), tst4.begin());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    l = tst5;  // to the same size
    CHECK(l, tst5.size(), tst5.begin());

    std::initializer_list<T> tst6;
    l = tst6;  // to empty
    CHECK_EMPTY(l);
}

// --------------------------------------------

static void test_6() {  // copy constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l0(al1), l(tst, al1);

    util::list<T, util::pool_allocator<T>> l01(l0);
    CHECK_EMPTY(l01);
    VERIFY(l01.get_allocator() == al1);

    util::list<T, util::pool_allocator<T>> l02(l0, al2);
    CHECK_EMPTY(l02);
    VERIFY(l02.get_allocator() == al2);

    util::list<T, util::pool_allocator<T>> l1(l);
    CHECK(l1, tst.size(), tst.begin());
    VERIFY(l1.get_allocator() == al1);

    util::list<T, util::pool_allocator<T>> l2(l, al2);
    CHECK(l2, tst.size(), tst.begin());
    VERIFY(l2.get_allocator() == al2);
}

static void test_7() {  // move constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l0(al1), l(tst, al1);

    util::list<T, util::pool_allocator<T>> l01(std::move(l0));
    CHECK_EMPTY(l01);
    VERIFY(l01.get_allocator() == al1);

    util::list<T, util::pool_allocator<T>> l02(std::move(l01), al2);
    CHECK_EMPTY(l02);
    VERIFY(l02.get_allocator() == al2);

    util::list<T, util::pool_allocator<T>> l03(std::move(l02), al2);
    CHECK_EMPTY(l03);
    VERIFY(l03.get_allocator() == al2);

    util::list<T, util::pool_allocator<T>> l1(std::move(l));
    CHECK(l1, tst.size(), tst.begin());
    VERIFY(l1.get_allocator() == al1);
    CHECK_EMPTY(l);

    util::list<T, util::pool_allocator<T>> l2(std::move(l1), al2);
    CHECK(l2, tst.size(), tst.begin());
    VERIFY(l2.get_allocator() == al2);
    VERIFY(l1.size() == l2.size());  // different allocators -> per-element movement

    util::list<T, util::pool_allocator<T>> l3(std::move(l2), al2);
    CHECK(l3, tst.size(), tst.begin());
    VERIFY(l3.get_allocator() == al2);
    CHECK_EMPTY(l2);
}

// --------------------------------------------

static void test_8() {  // copy assignment, same allocator
    util::pool_allocator<void> al;
    util::list<T, util::pool_allocator<T>> l(al);

    util::list<T, util::pool_allocator<T>> l0(al);
    l = l0;  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l1(tst1, al);
    l = l1;  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, util::pool_allocator<T>> l3(tst3, al);
    l = l3;  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());
    VERIFY(l.get_allocator() == al);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::list<T, util::pool_allocator<T>> l4(tst4, al);
    l = l4;  // to less size
    CHECK(l, tst4.size(), tst4.begin());
    VERIFY(l.get_allocator() == al);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::list<T, util::pool_allocator<T>> l5(tst5, al);
    l = l5;  // to the same size
    CHECK(l, tst5.size(), tst5.begin());
    VERIFY(l.get_allocator() == al);

    util::list<T, util::pool_allocator<T>> l6(al);
    l = l6;  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al);
}

static void test_9() {  // copy assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::list<T, util::pool_allocator<T>> l(al1);

    util::list<T, util::pool_allocator<T>> l0(al2);
    l = l0;  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l1(tst1, al2);
    l = l1;  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al1);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, util::pool_allocator<T>> l3(tst3, al2);
    l = l3;  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());
    VERIFY(l.get_allocator() == al1);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::list<T, util::pool_allocator<T>> l4(tst4, al2);
    l = l4;  // to less size
    CHECK(l, tst4.size(), tst4.begin());
    VERIFY(l.get_allocator() == al1);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::list<T, util::pool_allocator<T>> l5(tst5, al2);
    l = l5;  // to the same size
    CHECK(l, tst5.size(), tst5.begin());
    VERIFY(l.get_allocator() == al1);

    util::list<T, util::pool_allocator<T>> l6(al2);
    l = l6;  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);
}

static void test_10() {  // copy assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::list<T, unfriendly_pool_allocator<T>> l(al1);

    util::list<T, unfriendly_pool_allocator<T>> l0(al2);
    l = l0;  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, unfriendly_pool_allocator<T>> l1(tst1, al1);
    l = l1;  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, unfriendly_pool_allocator<T>> l2(tst2, al2);
    l = l2;  // from non-empty
    CHECK(l, tst2.size(), tst2.begin());
    VERIFY(l.get_allocator() == al2);

    util::list<T, unfriendly_pool_allocator<T>> l3(al1);
    l = l3;  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);
}

// --------------------------------------------

static void test_11() {  // move assignment, same allocator
    util::pool_allocator<void> al;
    util::list<T, util::pool_allocator<T>> l(al);

    util::list<T, util::pool_allocator<T>> l0(al);
    l = std::move(l0);  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l1(tst1, al);
    l = std::move(l1);  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al);
    CHECK_EMPTY(l1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, util::pool_allocator<T>> l2(tst2, al);
    l = std::move(l2);  // from non-empty
    CHECK(l, tst2.size(), tst2.begin());
    VERIFY(l.get_allocator() == al);
    CHECK_EMPTY(l2);

    util::list<T, util::pool_allocator<T>> l3(al);
    l = std::move(l3);  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al);
}

static void test_12() {  // move assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::list<T, util::pool_allocator<T>> l(al1);

    util::list<T, util::pool_allocator<T>> l0(al2);
    l = std::move(l0);  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, util::pool_allocator<T>> l1(tst1, al1);
    l = std::move(l1);  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al1);
    CHECK_EMPTY(l1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, util::pool_allocator<T>> l2(tst2, al2);
    l = std::move(l2);  // from non-empty
    CHECK(l, tst2.size(), tst2.begin());
    VERIFY(l.get_allocator() == al2);
    CHECK_EMPTY(l2);

    util::list<T, util::pool_allocator<T>> l3(al1);
    l = std::move(l3);  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);
}

static void test_13() {  // move assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::list<T, unfriendly_pool_allocator<T>> l(al1);

    util::list<T, unfriendly_pool_allocator<T>> l0(al2);
    l = std::move(l0);  // from empty to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);
    VERIFY(l0.size() == l.size());

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::list<T, unfriendly_pool_allocator<T>> l1(tst1, al2);
    l = std::move(l1);  // from empty
    CHECK(l, tst1.size(), tst1.begin());
    VERIFY(l.get_allocator() == al1);
    VERIFY(l1.size() == l.size());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::list<T, unfriendly_pool_allocator<T>> l3(tst3, al2);
    l = std::move(l3);  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());
    VERIFY(l.get_allocator() == al1);
    VERIFY(l3.size() == l.size());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::list<T, unfriendly_pool_allocator<T>> l4(tst4, al2);
    l = std::move(l4);  // to less size
    CHECK(l, tst4.size(), tst4.begin());
    VERIFY(l.get_allocator() == al1);
    VERIFY(l4.size() == l.size());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::list<T, unfriendly_pool_allocator<T>> l5(tst5, al2);
    l = std::move(l5);  // to the same size
    CHECK(l, tst5.size(), tst5.begin());
    VERIFY(l.get_allocator() == al1);
    VERIFY(l5.size() == l.size());

    util::list<T, unfriendly_pool_allocator<T>> l6(al2);
    l = std::move(l6);  // to empty
    CHECK_EMPTY(l);
    VERIFY(l.get_allocator() == al1);
    VERIFY(l6.size() == l.size());
}

// --------------------------------------------

static void test_14() {  // assignment from given size and value
    util::list<T, util::pool_allocator<T>> l;

    l.assign(0, 10);  // from empty to empty
    CHECK_EMPTY(l);

    std::initializer_list<T> tst1 = {10, 10, 10, 10, 10};
    l.assign(5, 10);  // from empty
    CHECK(l, tst1.size(), tst1.begin());

    std::initializer_list<T> tst3 = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    l.assign(16, 10);  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());

    std::initializer_list<T> tst4 = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    l.assign(10, 10);  // to less size
    CHECK(l, tst4.size(), tst4.begin());

    std::initializer_list<T> tst5 = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
    l.assign(10, 20);  // to the same size
    CHECK(l, tst5.size(), tst5.begin());

    l.assign(0, 10);
    CHECK_EMPTY(l);  // to empty
}

static void test_15() {  // assignment from iterator range
    util::list<T, util::pool_allocator<T>> l;

    T tst0[] = {1};
    l.assign(tst0, tst0);  // from empty to empty
    CHECK_EMPTY(l);

    T tst1[] = {1, 2, 3, 4, 5};
    l.assign(tst1, tst1 + 5);  // from empty
    CHECK(l, 5, tst1);

    T tst3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    l.assign(tst3, tst3 + 16);  // from non-empty
    CHECK(l, 16, tst3);

    T tst4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l.assign(tst4, tst4 + 10);  // to less size
    CHECK(l, 10, tst4);

    T tst5[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    l.assign(tst5, tst5 + 10);  // to the same size
    CHECK(l, 10, tst5);

    T tst6[] = {1};
    l.assign(tst6, tst6);  // to empty
    CHECK_EMPTY(l);
}

static void test_16() {  // assignment from iterator range of different type
    util::list<T, util::pool_allocator<T>> l;

    int tst0[] = {1};
    l.assign(tst0, tst0);  // from empty to empty
    CHECK_EMPTY(l);

    int tst1[] = {1, 2, 3, 4, 5};
    l.assign(tst1, tst1 + 5);  // from empty
    CHECK(l, 5, tst1);

    int tst3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    l.assign(tst3, tst3 + 16);  // from non-empty
    CHECK(l, 16, tst3);

    int tst4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l.assign(tst4, tst4 + 10);  // to less size
    CHECK(l, 10, tst4);

    int tst5[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    l.assign(tst5, tst5 + 10);  // to the same size
    CHECK(l, 10, tst5);

    int tst6[] = {1};
    l.assign(tst6, tst6);  // to empty
    CHECK_EMPTY(l);
}

static void test_17() {  // assignment from initializer
    util::list<T, util::pool_allocator<T>> l;

    std::initializer_list<T> tst0;
    l.assign(tst0);  // from empty to empty
    CHECK_EMPTY(l);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    l.assign(tst1);  // from empty
    CHECK(l, tst1.size(), tst1.begin());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    l.assign(tst3);  // from non-empty
    CHECK(l, tst3.size(), tst3.begin());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l.assign(tst4);  // to less size
    CHECK(l, tst4.size(), tst4.begin());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    l.assign(tst5);  // to the same size
    CHECK(l, tst5.size(), tst5.begin());

    std::initializer_list<T> tst6;
    l.assign(tst6);  // to empty
    CHECK_EMPTY(l);
}

// --------------------------------------------

static void test_18() {  // swap
    util::pool_allocator<void> al1, al2;
    util::list<T, util::pool_allocator<T>> l1(al1), l2(al2);

    l1.swap(l2);
    CHECK_EMPTY(l1);
    CHECK_EMPTY(l2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    l1 = tst1;
    l1.swap(l2);
    CHECK_EMPTY(l1);
    CHECK(l2, tst1.size(), tst1.begin());

    l1.swap(l2);
    CHECK(l1, tst1.size(), tst1.begin());
    CHECK_EMPTY(l2);

    std::initializer_list<T> tst2 = {10, 11, 12, 13, 14, 15, 16, 17};
    l2 = tst2;
    l1.swap(l2);
    CHECK(l1, tst2.size(), tst2.begin());
    CHECK(l2, tst1.size(), tst1.begin());
}

// --------------------------------------------

static void test_19() {
    util::pool_allocator<void> al;

    util::list<T, util::pool_allocator<T>> list1{{1, 2, 3, 4, 5}, al};
    util::list<T, util::pool_allocator<T>> list2{{10, 20, 30, 40, 50}, al};

    auto it = list1.begin();
    std::advance(it, 2);

    std::initializer_list<T> tst1 = {1, 2, 10, 20, 30, 40, 50, 3, 4, 5};
    list1.splice(it, list2);
    CHECK(list1, tst1.size(), tst1.begin());
    VERIFY(list2.size() == 0);
    VERIFY(list2.begin() == list2.end());

    std::initializer_list<T> tst2 = {1, 2, 10, 20, 30, 40, 50};
    std::initializer_list<T> tst3 = {3, 4, 5};
    list2.splice(list2.begin(), list1, it, list1.end());
    CHECK(list1, tst2.size(), tst2.begin());
    CHECK(list2, tst3.size(), tst3.begin());

    std::initializer_list<T> tst4 = {1, 2, 5, 10, 20, 30, 40, 50};
    std::initializer_list<T> tst5 = {3, 4};
    list1.splice(std::next(list1.begin(), 2), list2, std::next(list2.begin(), 2));
    CHECK(list1, tst4.size(), tst4.begin());
    CHECK(list2, tst5.size(), tst5.begin());

    std::initializer_list<T> tst6 = {1, 2, 20, 5, 10, 30, 40, 50};
    list1.splice(std::next(list1.begin(), 2), list1, std::next(list1.begin(), 4));
    CHECK(list1, tst6.size(), tst6.begin());

    std::initializer_list<T> tst7 = {1, 2, 5, 10, 30, 40, 50, 20};
    list1.splice(std::next(list1.begin(), 2), list1, std::next(list1.begin(), 3), list1.end());
    CHECK(list1, tst7.size(), tst7.begin());
}

static void test_20() {
    util::pool_allocator<void> al;

    util::list<T, util::pool_allocator<T>> list1{{5, 9, 0, 1, 3}, al};
    std::initializer_list<T> tst1 = {0, 1, 3, 5, 9};
    list1.sort();
    CHECK(list1, tst1.size(), tst1.begin());

    util::list<T, util::pool_allocator<T>> list2{{8, 7, 2, 6, 4}, al};
    std::initializer_list<T> tst2 = {2, 4, 6, 7, 8};
    list2.sort();
    CHECK(list2, tst2.size(), tst2.begin());

    std::initializer_list<T> tst3 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    list1.merge(list2);
    CHECK(list1, tst3.size(), tst3.begin());
    VERIFY(list2.size() == 0);
    VERIFY(list2.begin() == list2.end());

    util::list<T, util::pool_allocator<T>> list3{{0, 1, 2, 3, 4, 5, 6, 7, 3, 2}, al};
    list3.remove(2);
    std::initializer_list<T> tst4 = {0, 1, 3, 4, 5, 6, 7, 3};
    CHECK(list3, tst4.size(), tst4.begin());
    list3.remove_if([](decltype(*list3.begin()) el) { return el == 3; });
    std::initializer_list<T> tst5 = {0, 1, 4, 5, 6, 7};
    CHECK(list3, tst5.size(), tst5.begin());

    util::list<T, util::pool_allocator<T>> list4{{0, 1, 2, 2, 2, 5, 6, 7, 3, 3}, al};
    list4.unique();
    std::initializer_list<T> tst6 = {0, 1, 2, 5, 6, 7, 3};
    CHECK(list4, tst6.size(), tst6.begin());
}

static void test_21() {
    util::pool_allocator<void> al;
    util::list<T, util::pool_allocator<T>> l{al};

    srand(0);

    int iter = 0, perc0 = 0;
    static const int iter_count = N / 1000;
    std::cout << "  0.0%" << std::flush;
    for (; iter < iter_count; ++iter) {
        int perc = (1000 * static_cast<int64_t>(iter)) / iter_count;
        if (perc > perc0) {
            std::cout << "\b\b\b\b\b\b" << std::setw(3) << (perc / 10) << "." << std::setw(0) << (perc % 10) << "%"
                      << std::flush;
            perc0 = perc;
        }

        l.clear();
        for (size_t i = 0; i < 100000; ++i) { l.emplace_back(rand() % 100000); }

        l.sort();

        size_t i = 1;
        for (auto it = std::next(l.begin()); it != l.end(); ++it, ++i) { VERIFY(!(*it < *std::prev(it))); }
        VERIFY(i == 100000);
    }

    std::cout << "\b\b\b\b\b\b" << std::flush;
}

// --------------------------------------------

template<typename Ty>
static void list_test(int iter_count, bool log = false) {
    util::list<Ty, util::global_pool_allocator<Ty>> l;
    std::list<Ty> l_ref;

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

        int act = rand() % 83;
        if ((act >= 0) && (act < 10)) {
            if (log) { std::cout << "insert one" << std::endl; }

            size_t n = rand() % (l.size() + 1);
            int val = rand() % 100;
            auto l_res = l.emplace(std::next(l.begin(), n), val);
            auto ref_res = l_ref.emplace(std::next(l_ref.begin(), n), val);
            VERIFY(std::distance(l.begin(), l_res) == std::distance(l_ref.begin(), ref_res));
        } else if ((act >= 10) && (act < 20)) {
            if (log) { std::cout << "insert" << std::endl; }

            size_t n = rand() % (l.size() + 1);
            size_t count = 1 + rand() % 5;

            T val[10];
            for (size_t i = 0; i < count; ++i) { val[i] = rand() % 100; }

            auto l_res = l.insert(std::next(l.begin(), n), (T*)val, (T*)val + count);
            auto ref_res = l_ref.insert(std::next(l_ref.begin(), n), (T*)val, (T*)val + count);
            VERIFY(std::distance(l.begin(), l_res) == std::distance(l_ref.begin(), ref_res));
        } else if ((act >= 20) && (act < 30)) {
            if (!l.empty()) {
                if (log) { std::cout << "erase one" << std::endl; }

                size_t n = rand() % l.size();
                auto l_res = l.erase(std::next(l.begin(), n));
                auto ref_res = l_ref.erase(std::next(l_ref.begin(), n));
                VERIFY(std::distance(l.begin(), l_res) == std::distance(l_ref.begin(), ref_res));
            }
        } else if ((act >= 30) && (act < 40)) {
            if (!l.empty()) {
                if (log) { std::cout << "erase" << std::endl; }

                size_t n = rand() % (1 + l.size());
                size_t count = rand() % (1 + l.size() - n);
                auto l_res = l.erase(std::next(l.begin(), n), std::next(l.begin(), n + count));
                auto ref_res = l_ref.erase(std::next(l_ref.begin(), n), std::next(l_ref.begin(), n + count));
                VERIFY(std::distance(l.begin(), l_res) == std::distance(l_ref.begin(), ref_res));
            }
        } else if ((act >= 40) && (act < 50)) {
            if (log) { std::cout << "emplace back" << std::endl; }

            int val = rand() % 100;
            l.emplace_back(val);
            l_ref.emplace_back(val);
        } else if ((act >= 50) && (act < 60)) {
            if (!l.empty()) {
                if (log) { std::cout << "pop back" << std::endl; }

                l.pop_back();
                l_ref.pop_back();
            }
        } else if ((act >= 60) && (act < 70)) {
            if (log) { std::cout << "emplace front" << std::endl; }

            int val = rand() % 100;
            l.emplace_front(val);
            l_ref.emplace_front(val);
        } else if ((act >= 70) && (act < 80)) {
            if (!l.empty()) {
                if (log) { std::cout << "pop front" << std::endl; }

                l.pop_front();
                l_ref.pop_front();
            }
        } else if (act == 80) {
            if (log) { std::cout << "clear" << std::endl; }

            l.clear();
            l_ref.clear();
        } else if (act == 81) {
            size_t sz = rand() % 100;

            if (log) {
                if (l.size() < sz) {
                    std::cout << "resize default grow" << std::endl;
                } else {
                    std::cout << "resize default trim" << std::endl;
                }
            }

            l.resize(sz);
            l_ref.resize(sz);
        } else if (act == 82) {
            size_t sz = rand() % 100;

            if (log) {
                if (l.size() < sz) {
                    std::cout << "resize grow" << std::endl;
                } else {
                    std::cout << "resize trim" << std::endl;
                }
            }

            int val = rand() % 100;
            l.resize(sz, val);
            l_ref.resize(sz, val);
        }

        CHECK(l, l_ref.size(), l_ref.begin());
        if (!l.empty()) { VERIFY((l.front() == l_ref.front()) && (l.back() == l_ref.back())); }
    }

    std::cout << "\b\b\b\b\b\b" << std::flush;
}

static void test_100() {
#if defined(USE_UTIL) && defined(USE_STD)
    list_test<T>(10 * N);
#endif
}

// --------------------------------------------

template<typename ListType>
static void performance(int iter_count) {
    ListType l;

    srand(0);

    auto start = std::clock();
    using Ty = typename ListType::value_type;
    const Ty val0 = 10;

    for (int iter = 0; iter < iter_count; ++iter) {
        int act = rand() % 83;
        if ((act >= 0) && (act < 10)) {
            l.emplace(std::next(l.begin(), rand() % (l.size() + 1)), rand() % 100);
        } else if ((act >= 10) && (act < 20)) {
            size_t n = rand() % (l.size() + 1);
            size_t count = 1 + rand() % 5;
            Ty val[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            l.insert(std::next(l.begin(), n), (Ty*)val, (Ty*)val + count);
        } else if ((act >= 20) && (act < 30)) {
            if (!l.empty()) { l.erase(std::next(l.begin(), rand() % l.size())); }
        } else if ((act >= 30) && (act < 40)) {
            if (!l.empty()) {
                size_t n = rand() % l.size();
                size_t count = 1 + rand() % (l.size() - n);
                l.erase(std::next(l.begin(), n), std::next(l.begin(), n + count));
            }
        } else if ((act >= 40) && (act < 50)) {
            l.emplace_back(val0);
        } else if ((act >= 50) && (act < 60)) {
            if (!l.empty()) { l.pop_back(); }
        } else if ((act >= 60) && (act < 70)) {
            l.emplace_front(val0);
        } else if ((act >= 70) && (act < 80)) {
            if (!l.empty()) { l.pop_front(); }
        } else if (act == 80) {
            l.clear();
        } else if (act == 81) {
            l.resize(rand() % 100);
        } else if (act == 82) {
            l.resize(rand() % 100, val0);
        }
    }

    std::cout << (std::clock() - start) << std::endl;
}

static void test_101() {
#if defined(USE_UTIL)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- util::list<T, util::global_pool_allocator<T>> performance..." << std::flush;
    performance<util::list<T, util::global_pool_allocator<T>>>(N);
    std::cout << "---------- util::list<T, util::pool_allocator<T>> performance..." << std::flush;
    performance<util::list<T, util::pool_allocator<T>>>(N);
    std::cout << "---------- util::list<T, std::allocator<T>> performance..." << std::flush;
    performance<util::list<T, std::allocator<T>>>(N);

    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- util::list<int, util::global_pool_allocator<int>> performance..." << std::flush;
    performance<util::list<int, util::global_pool_allocator<int>>>(N);
    std::cout << "---------- util::list<int, util::pool_allocator<int>> performance..." << std::flush;
    performance<util::list<int, util::pool_allocator<int>>>(N);
    std::cout << "---------- util::list<int, std::allocator<int>> performance..." << std::flush;
    performance<util::list<int, std::allocator<int>>>(N);
#endif

#if defined(USE_STD)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- std::list<T, util::global_pool_allocator<T>> performance..." << std::flush;
    performance<std::list<T, util::global_pool_allocator<T>>>(N);
    std::cout << "---------- std::list<T, util::pool_allocator<T>> performance..." << std::flush;
    performance<std::list<T, util::pool_allocator<T>>>(N);
    std::cout << "---------- std::list<T, std::allocator<T>> performance..." << std::flush;
    performance<std::list<T, std::allocator<T>>>(N);

    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- std::list<int, util::global_pool_allocator<int>> performance..." << std::flush;
    performance<std::list<int, util::global_pool_allocator<int>>>(N);
    std::cout << "---------- std::list<int, util::pool_allocator<int>> performance..." << std::flush;
    performance<std::list<int, util::pool_allocator<int>>>(N);
    std::cout << "---------- std::list<int, std::allocator<int>> performance..." << std::flush;
    performance<std::list<int, std::allocator<int>>>(N);
#endif
}

// --------------------------------------------

static void test_102() {
    std::cout << std::endl;
    std::cout << "sizeof(util::list<T>::iterator) = " << sizeof(util::list<T>::iterator) << std::endl;
    std::cout << "sizeof(util::list<T, util::global_pool_allocator<T>>) = "
              << sizeof(util::list<T, util::global_pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(util::list<T, util::pool_allocator<T>>) = " << sizeof(util::list<T, util::pool_allocator<T>>)
              << std::endl;
    std::cout << "sizeof(util::list<T, std::allocator<T>>) = " << sizeof(util::list<T, std::allocator<T>>) << std::endl;
    std::cout << "sizeof(util::impl::list_node_type<T>) = " << sizeof(util::impl::list_node_type<T>) << std::endl;
    std::cout << std::endl;
    std::cout << "sizeof(std::list<T>::iterator) = " << sizeof(std::list<T>::iterator) << std::endl;
    std::cout << "sizeof(std::list<T, util::global_pool_allocator<T>>) = "
              << sizeof(std::list<T, util::global_pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(std::list<T, util::pool_allocator<T>>) = " << sizeof(std::list<T, util::pool_allocator<T>>)
              << std::endl;
    std::cout << "sizeof(std::list<T, std::allocator<T>>) = " << sizeof(std::list<T, std::allocator<T>>) << std::endl;
}

// --------------------------------------------

std::pair<std::pair<size_t, void (*)()>*, size_t> get_list_tests() {
    static std::pair<size_t, void (*)()> _tests[] = {
        {0, test_0},   {1, test_1},     {2, test_2},     {3, test_3},     {4, test_4},   {5, test_5},   {6, test_6},
        {7, test_7},   {8, test_8},     {9, test_9},     {10, test_10},   {11, test_11}, {12, test_12}, {13, test_13},
        {14, test_14}, {15, test_15},   {16, test_16},   {17, test_17},   {18, test_18}, {19, test_19}, {20, test_20},
        {21, test_21}, {100, test_100}, {101, test_101}, {102, test_102},
    };

    return std::make_pair(_tests, sizeof(_tests) / sizeof(_tests[0]));
}
