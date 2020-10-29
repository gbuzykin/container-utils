#include "core/map.h"
#include "core/multiset.h"
#include "core/pool_allocator.h"
#include "core/set.h"

#include "tests.h"

#include <set>

#ifdef _DEBUG  // _DEBUG
static const int N = 500;
#else   // _DEBUG
static const int N = 5000;
#endif  // _DEBUG

static bool check_balance(util::rbtree_node_t* node, int& black, int cur_black = 0,
                          util::rbtree_node_t::color_t color = util::rbtree_node_t::color_t::kRed) {
    if ((color != util::rbtree_node_t::color_t::kBlack) && (node->color != util::rbtree_node_t::color_t::kBlack)) {
        return false;
    }
    if (node->color == util::rbtree_node_t::color_t::kBlack) { ++cur_black; }

    if (node->left) {
        if (!check_balance(node->left, black, cur_black, node->color)) { return false; }
    } else if (black > 0) {
        if (cur_black != black) { return false; }
    } else {
        black = cur_black;
    }

    if (node->right) {
        if (!check_balance(node->right, black, cur_black, node->color)) { return false; }
    } else if (black > 0) {
        if (cur_black != black) { return false; }
    } else {
        black = cur_black;
    }

    return true;
}

template<typename NodeTy, typename Alloc, typename Comp, typename InputIt>
bool check_rbtree(const util::impl::rbtree_base<NodeTy, Alloc, Comp>& t, size_t sz, InputIt src) {
    int black = 0;
    if (t.size() != sz) { return false; }
    if (!t.empty() && !check_balance(t.end().node(nullptr)->left, black)) { return false; }
    if (std::distance(t.begin(), t.end()) != sz) { return false; }
    for (auto it = t.begin(); it != t.end(); ++it) {
        if (!(*it == *src++)) { return false; }
    }
    for (auto it = t.rbegin(); it != t.rend(); ++it) {
        if (!(*it == *(--src))) { return false; }
    }
    return true;
}

#define CHECK(...) \
    if (!check_rbtree(__VA_ARGS__)) { throw std::logic_error(report_error(__FILE__, __LINE__, "rbtree mismatched")); }

#define CHECK_EMPTY(...) \
    if (((__VA_ARGS__).size() != 0) || ((__VA_ARGS__).begin() != (__VA_ARGS__).end())) { \
        throw std::logic_error(report_error(__FILE__, __LINE__, "rbtree is not empty")); \
    }

// --------------------------------------------

static void test_0() {  // empty set
    VERIFY(util::is_random_access_iterator<std::multiset<T>::iterator>::value == false);
    VERIFY(util::is_random_access_iterator<util::multiset<T>::iterator>::value == false);

    util::pool_allocator<void> al;

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s;
    CHECK_EMPTY(s);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(al);
    CHECK_EMPTY(s1);
    VERIFY(s1.get_allocator() == al);
}

static void test_3() {  // set initialized with iterator range
    util::pool_allocator<void> al;

    T tst0[] = {1};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(tst0, tst0, al);
    CHECK_EMPTY(s0);
    VERIFY(s0.get_allocator() == al);

    T tst[] = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(tst, tst + 5, al);
    CHECK(s, 5, tst);
    VERIFY(s.get_allocator() == al);
}

static void test_4() {  // set with initializer
    util::pool_allocator<void> al;

    std::initializer_list<T> tst0;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(tst0, al);
    CHECK_EMPTY(s0);
    VERIFY(s0.get_allocator() == al);

    std::initializer_list<T> tst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(tst, al);
    CHECK(s, tst.size(), tst.begin());
    VERIFY(s.get_allocator() == al);
}

static void test_5() {  // initializer assignment
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s;

    std::initializer_list<T> tst0;
    s = tst0;  // from empty to empty
    CHECK_EMPTY(s);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    s = tst1;  // from empty
    CHECK(s, tst1.size(), tst1.begin());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    s = tst3;  // from non-empty
    CHECK(s, tst3.size(), tst3.begin());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    s = tst4;  // to less size
    CHECK(s, tst4.size(), tst4.begin());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    s = tst5;  // to the same size
    CHECK(s, tst5.size(), tst5.begin());

    std::initializer_list<T> tst6;
    s = tst6;  // to empty
    CHECK_EMPTY(s);
}

// --------------------------------------------

static void test_6() {  // copy constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al1), s(tst, al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s01(s0);
    CHECK_EMPTY(s01);
    VERIFY(s01.get_allocator() == al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s02(s0, al2);
    CHECK_EMPTY(s02);
    VERIFY(s02.get_allocator() == al2);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(s);
    CHECK(s1, tst.size(), tst.begin());
    VERIFY(s1.get_allocator() == al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s2(s, al2);
    CHECK(s2, tst.size(), tst.begin());
    VERIFY(s2.get_allocator() == al2);
}

static void test_7() {  // move constructor
    util::pool_allocator<void> al1, al2;

    std::initializer_list<T> tst = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al1), s(tst, al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s01(std::move(s0));
    CHECK_EMPTY(s01);
    VERIFY(s01.get_allocator() == al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s02(std::move(s01), al2);
    CHECK_EMPTY(s02);
    VERIFY(s02.get_allocator() == al2);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s03(std::move(s02), al2);
    CHECK_EMPTY(s03);
    VERIFY(s03.get_allocator() == al2);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(std::move(s));
    CHECK(s1, tst.size(), tst.begin());
    VERIFY(s1.get_allocator() == al1);
    CHECK_EMPTY(s);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s2(std::move(s1), al2);
    CHECK(s2, tst.size(), tst.begin());
    VERIFY(s2.get_allocator() == al2);
    VERIFY(s1.size() == s2.size());  // different allocators -> per-element movement

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s3(std::move(s2), al2);
    CHECK(s3, tst.size(), tst.begin());
    VERIFY(s3.get_allocator() == al2);
    CHECK_EMPTY(s2);
}

// --------------------------------------------

static void test_8() {  // copy assignment, same allocator
    util::pool_allocator<void> al;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(al);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al);
    s = s0;  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(tst1, al);
    s = s1;  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s3(tst3, al);
    s = s3;  // from non-empty
    CHECK(s, tst3.size(), tst3.begin());
    VERIFY(s.get_allocator() == al);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s4(tst4, al);
    s = s4;  // to less size
    CHECK(s, tst4.size(), tst4.begin());
    VERIFY(s.get_allocator() == al);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s5(tst5, al);
    s = s5;  // to the same size
    CHECK(s, tst5.size(), tst5.begin());
    VERIFY(s.get_allocator() == al);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s6(al);
    s = s6;  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al);
}

static void test_9() {  // copy assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al2);
    s = s0;  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(tst1, al2);
    s = s1;  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al1);

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s3(tst3, al2);
    s = s3;  // from non-empty
    CHECK(s, tst3.size(), tst3.begin());
    VERIFY(s.get_allocator() == al1);

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s4(tst4, al2);
    s = s4;  // to less size
    CHECK(s, tst4.size(), tst4.begin());
    VERIFY(s.get_allocator() == al1);

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s5(tst5, al2);
    s = s5;  // to the same size
    CHECK(s, tst5.size(), tst5.begin());
    VERIFY(s.get_allocator() == al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s6(al2);
    s = s6;  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);
}

static void test_10() {  // copy assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s(al1);

    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s0(al2);
    s = s0;  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s1(tst1, al1);
    s = s1;  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s2(tst2, al2);
    s = s2;  // from non-empty
    CHECK(s, tst2.size(), tst2.begin());
    VERIFY(s.get_allocator() == al2);

    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s3(al1);
    s = s3;  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);
}

// --------------------------------------------

static void test_11() {  // move assignment, same allocator
    util::pool_allocator<void> al;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(al);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al);
    s = std::move(s0);  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(tst1, al);
    s = std::move(s1);  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al);
    CHECK_EMPTY(s1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s2(tst2, al);
    s = std::move(s2);  // from non-empty
    CHECK(s, tst2.size(), tst2.begin());
    VERIFY(s.get_allocator() == al);
    CHECK_EMPTY(s2);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s3(al);
    s = std::move(s3);  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al);
}

static void test_12() {  // move assignment, different allocators, friendly allocator
    util::pool_allocator<void> al1, al2;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s(al1);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s0(al2);
    s = std::move(s0);  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(tst1, al1);
    s = std::move(s1);  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al1);
    CHECK_EMPTY(s1);

    std::initializer_list<T> tst2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s2(tst2, al2);
    s = std::move(s2);  // from non-empty
    CHECK(s, tst2.size(), tst2.begin());
    VERIFY(s.get_allocator() == al2);
    CHECK_EMPTY(s2);

    util::multiset<T, std::less<T>, util::pool_allocator<T>> s3(al1);
    s = std::move(s3);  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);
}

static void test_13() {  // move assignment, different allocators, unfriendly allocator
    unfriendly_pool_allocator<void> al1, al2;
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s(al1);

    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s0(al2);
    s = std::move(s0);  // from empty to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);
    VERIFY(s0.size() == s.size());

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s1(tst1, al2);
    s = std::move(s1);  // from empty
    CHECK(s, tst1.size(), tst1.begin());
    VERIFY(s.get_allocator() == al1);
    VERIFY(s1.size() == s.size());

    std::initializer_list<T> tst3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s3(tst3, al2);
    s = std::move(s3);  // from non-empty
    CHECK(s, tst3.size(), tst3.begin());
    VERIFY(s.get_allocator() == al1);
    VERIFY(s3.size() == s.size());

    std::initializer_list<T> tst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s4(tst4, al2);
    s = std::move(s4);  // to less size
    CHECK(s, tst4.size(), tst4.begin());
    VERIFY(s.get_allocator() == al1);
    VERIFY(s4.size() == s.size());

    std::initializer_list<T> tst5 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s5(tst5, al2);
    s = std::move(s5);  // to the same size
    CHECK(s, tst5.size(), tst5.begin());
    VERIFY(s.get_allocator() == al1);
    VERIFY(s5.size() == s.size());

    util::multiset<T, std::less<T>, unfriendly_pool_allocator<T>> s6(al2);
    s = std::move(s6);  // to empty
    CHECK_EMPTY(s);
    VERIFY(s.get_allocator() == al1);
    VERIFY(s6.size() == s.size());
}

// --------------------------------------------

static void test_18() {  // swap
    util::pool_allocator<void> al1, al2;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(al1), s2(al2);

    s1.swap(s2);
    CHECK_EMPTY(s1);
    CHECK_EMPTY(s2);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    s1 = tst1;
    s1.swap(s2);
    CHECK_EMPTY(s1);
    CHECK(s2, tst1.size(), tst1.begin());

    s1.swap(s2);
    CHECK(s1, tst1.size(), tst1.begin());
    CHECK_EMPTY(s2);

    std::initializer_list<T> tst2 = {10, 11, 12, 13, 14, 15, 16, 17};
    s2 = tst2;
    s1.swap(s2);
    CHECK(s1, tst2.size(), tst2.begin());
    CHECK(s2, tst1.size(), tst1.begin());
}

// --------------------------------------------

static void test_19() {
    util::map<int, int> m;
    auto comp = m.value_comp();
    auto b = comp(std::make_pair(4, 2), std::make_pair(2, 3));
}

static void test_20() {
    util::pool_allocator<void> al;
    util::multiset<T, std::less<T>, util::pool_allocator<T>> s1(al), s2(al);

    std::initializer_list<T> tst1 = {1, 2, 3, 4, 5};
    s1 = tst1;
    auto nh = s1.extract(3);
    s2.insert(std::move(nh));

    std::initializer_list<T> tst2 = {1, 2, 4, 5};
    CHECK(s1, tst2.size(), tst2.begin());

    std::initializer_list<T> tst3 = {3};
    CHECK(s2, tst3.size(), tst3.begin());

    decltype(nh) nh2;
    decltype(nh) nh3{s1.extract(4)};

    util::set<T, std::less<T>, util::pool_allocator<T>> s3(al);
    nh = s1.extract(5);
    auto result = s3.insert(std::move(nh));
    VERIFY(result.inserted);

    s1.emplace(5);
    nh = s1.extract(5);
    result = s3.insert(std::move(nh));
    VERIFY(!result.inserted);

    using value_type = std::decay_t<decltype(nh)>::value_type;
    value_type v = std::move(nh3.value());
    VERIFY(v == 4);

    util::map<T, std::string, std::less<T>, util::pool_allocator<std::pair<const T, std::string>>> m(al);
    m.emplace(1, "aaa");
    auto nh4 = m.extract(1);
    using key_type = std::decay_t<decltype(nh4)>::key_type;
    using mapped_type = std::decay_t<decltype(nh4)>::mapped_type;
    key_type k = std::move(nh4.key());
    mapped_type obj = std::move(nh4.mapped());
    VERIFY(k == 1);
    VERIFY(obj == "aaa");
}

static void test_21() {
    util::map<std::string, std::string> m;
    m.insert_or_assign("a", "apple");
    m.insert_or_assign("b", "bannana");
    m.insert_or_assign("c", "cherry");
    m.insert_or_assign("c", "clementine");

    VERIFY(m["a"] == "apple");
    VERIFY(m["b"] == "bannana");
    VERIFY(m["c"] == "clementine");
}

static void test_22() {
    util::map<std::string, std::string> m;

    m.try_emplace("a", "a");
    m.try_emplace("b", "abcd");
    m.try_emplace("c", 10, 'c');
    m.try_emplace("c", "Won't be inserted");

    VERIFY(m["a"] == "a");
    VERIFY(m["b"] == "abcd");
    VERIFY(m["c"] == std::string(10, 'c'));
}

static void test_23() {
    util::pool_allocator<void> al;
    util::map<int, std::string, std::less<int>, util::pool_allocator<std::pair<const int, std::string>>> u(al);
    using init_type = std::initializer_list<std::decay_t<decltype(u)>::value_type>;
    util::map<int, std::string, std::less<int>, util::pool_allocator<std::pair<const int, std::string>>> ma(
        init_type{{1, "apple"}, {5, "pear"}, {10, "banana"}}, al);
    util::map<int, std::string, std::less<int>, util::pool_allocator<std::pair<const int, std::string>>> mb(
        init_type{{2, "zorro"}, {4, "batman"}, {5, "X"}, {8, "alpaca"}}, al);
    u.merge(ma);
    u.merge(mb);
    CHECK_EMPTY(ma);
    VERIFY(mb[5] == "X");

    VERIFY(u[1] == "apple");
    VERIFY(u[2] == "zorro");
    VERIFY(u[4] == "batman");
    VERIFY(u[5] == "pear");
    VERIFY(u[8] == "alpaca");
    VERIFY(u[10] == "banana");

    u.insert(std::make_pair(11, "dddd"));
    VERIFY(u[11] == "dddd");
}

// --------------------------------------------

template<typename Ty>
void rbtree_test(int iter_count, bool log = false) {
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

        util::set<Ty, util::less<>, util::global_pool_allocator<Ty>> s, s1, s2, s3;
        std::set<Ty, util::less<>> s_ref;

        for (size_t i = 0; i < 100; ++i) { s1.emplace(rand() % 500); }
        for (size_t i = 0; i < 1100; ++i) { s2.emplace(rand() % 500); }
        for (size_t i = 0; i < 1000; ++i) {
            int val = rand() % 500;
            s3.emplace(val);
            s_ref.emplace(val);
            CHECK(s3, s_ref.size(), s_ref.begin());
        }

        s = s1;
        s = s2;
        s = s3;

        CHECK(s, s_ref.size(), s_ref.begin());

        for (size_t i = 0; i < 250; ++i) {
            int val = rand() % 500;

            auto it = s.find(val);
            auto lower = s.lower_bound(val);
            auto upper = s.upper_bound(val);
            auto range = s.equal_range(val);

            VERIFY(lower == range.first);
            VERIFY(upper == range.second);

            auto it_ref = s_ref.find(val);
            auto lower_ref = s_ref.lower_bound(val);
            auto upper_ref = s_ref.upper_bound(val);

            VERIFY(s.count(val) == s_ref.count(val));

            VERIFY(std::distance(s.begin(), it) == std::distance(s_ref.begin(), it_ref));
            VERIFY(std::distance(s.begin(), lower) == std::distance(s_ref.begin(), lower_ref));
            VERIFY(std::distance(s.begin(), upper) == std::distance(s_ref.begin(), upper_ref));

            if ((lower != s.end()) && (lower == upper)) {
                it = s.erase(lower);
            } else {
                it = s.erase(lower, upper);
            }

            if ((lower_ref != s_ref.end()) && (lower_ref == upper_ref)) {
                it_ref = s_ref.erase(lower_ref);
            } else {
                it_ref = s_ref.erase(lower_ref, upper_ref);
            }

            VERIFY(std::distance(s.begin(), it) == std::distance(s_ref.begin(), it_ref));
            CHECK(s, s_ref.size(), s_ref.begin());
        }

        VERIFY(s.empty() == s_ref.empty());

        for (size_t i = 0; i < 500; ++i) {
            int val = rand() % 500;
            VERIFY(s.erase(val) == s_ref.erase(val));
            CHECK(s, s_ref.size(), s_ref.begin());
        }

        VERIFY(s.empty() == s_ref.empty());

        while (!s_ref.empty()) {
            int val = rand() % 500;
            auto it = s.lower_bound(val);
            if (it != s.end()) { it = s.erase(it); }

            auto it_ref = s_ref.lower_bound(val);
            if (it_ref != s_ref.end()) { it_ref = s_ref.erase(it_ref); }

            VERIFY(std::distance(s.begin(), it) == std::distance(s_ref.begin(), it_ref));
            CHECK(s, s_ref.size(), s_ref.begin());
        }
    }

    std::cout << "\b\b\b\b\b\bOK!   " << std::endl;
}

static void test_100() {
#if defined(USE_UTIL) && defined(USE_STD)
    rbtree_test<T>(10 * N);
#endif
}

// --------------------------------------------

template<typename SetType>
int performance(int iter_count) {
    int result = 0;
    SetType s;

    srand(0);

    auto start = std::clock();
    for (int iter = 0; iter < 2 * iter_count; ++iter) {
        s.clear();
        for (int cnt = 0; cnt < 1000; ++cnt) s.emplace(rand() % 500);
    }
    std::cout << " ins/clear=" << (std::clock() - start) << std::flush;

    start = std::clock();
    for (int iter = 0; iter < 100 * iter_count; ++iter)
        for (auto it = s.begin(); it != s.end();) { result += static_cast<int>(*it++); }
    std::cout << " fwd=" << (std::clock() - start) << std::flush;

    start = std::clock();
    for (int iter = 0; iter < 100 * iter_count; ++iter)
        for (auto it = s.rbegin(); it != s.rend();) { result += static_cast<int>(*it++); }
    std::cout << " bwd=" << (std::clock() - start) << std::flush;

    start = std::clock();
    for (int iter = 0; iter < 1000 * iter_count; ++iter) {
        int val = rand() % 500;

        auto lower = s.lower_bound(val);
        auto upper = s.upper_bound(val);
        auto range = s.equal_range(val);

        result += (int)std::distance(lower, upper);
        result += (int)std::distance(range.first, range.second);
    }
    std::cout << " find=" << (std::clock() - start) << std::flush;

    start = std::clock();
    for (int iter = 0; iter < iter_count; ++iter) {
        s.clear();
        for (size_t i = 0; i < 1000; ++i) s.emplace(rand() % 500);

        for (size_t i = 0; i < 10; ++i) {
            for (auto it = s.begin(); it != s.end();) { result += static_cast<int>(*it++); }
            for (auto it = s.rbegin(); it != s.rend();) { result += static_cast<int>(*it++); }
        }

        for (size_t i = 0; i < 1000; ++i) {
            int val = rand() % 500;

            auto lower = s.lower_bound(val);
            auto upper = s.upper_bound(val);
            auto range = s.equal_range(val);

            result += (int)std::distance(lower, upper);
            result += (int)std::distance(range.first, range.second);

            s.erase(lower, upper);
        }
    }
    std::cout << " integral=" << (std::clock() - start) << std::endl;

    return result;
}

static void test_101() {
#if defined(USE_UTIL)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- util::multiset<T, util::less<>, util::global_pool_allocator<T>> performance..."
              << std::flush;
    performance<util::multiset<T, util::less<>, util::global_pool_allocator<T>>>(N);
    std::cout << "---------- util::multiset<T, util::less<>, util::pool_allocator<T>> performance..." << std::flush;
    performance<util::multiset<T, util::less<>, util::pool_allocator<T>>>(N);
    std::cout << "---------- util::multiset<T, util::less<>, std::allocator<T>>> performance..." << std::flush;
    performance<util::multiset<T, util::less<>, std::allocator<T>>>(N);

    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- util::multiset<int, util::less<>, util::global_pool_allocator<int>> "
                 "performance..."
              << std::flush;
    performance<util::multiset<int, util::less<>, util::global_pool_allocator<int>>>(N);
    std::cout << "---------- util::multiset<int, util::less<>, util::pool_allocator<int>> performance..." << std::flush;
    performance<util::multiset<int, util::less<>, util::pool_allocator<int>>>(N);
    std::cout << "---------- util::multiset<int, util::less<>, std::allocator<int>>> performance..." << std::flush;
    performance<util::multiset<int, util::less<>, std::allocator<int>>>(N);
#endif

#if defined(USE_STD)
    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- std::multiset<T, util::less<>, util::global_pool_allocator<T>> performance..."
              << std::flush;
    performance<std::multiset<T, util::less<>, util::global_pool_allocator<T>>>(N);
    std::cout << "---------- std::multiset<T, util::less<>, util::pool_allocator<T>> performance..." << std::flush;
    performance<std::multiset<T, util::less<>, util::pool_allocator<T>>>(N);
    std::cout << "---------- std::multiset<T, util::less<>, std::allocator<T>> performance..." << std::flush;
    performance<std::multiset<T, util::less<>, std::allocator<T>>>(N);

    std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
    std::cout << "---------- std::multiset<int, util::less<>, util::global_pool_allocator<int>> "
                 "performance..."
              << std::flush;
    performance<std::multiset<int, util::less<>, util::global_pool_allocator<int>>>(N);
    std::cout << "---------- std::multiset<int, util::less<>, util::pool_allocator<int>> performance..." << std::flush;
    performance<std::multiset<int, util::less<>, util::pool_allocator<int>>>(N);
    std::cout << "---------- std::multiset<int, util::less<>, std::allocator<int>> performance..." << std::flush;
    performance<std::multiset<int, util::less<>, std::allocator<int>>>(N);
#endif
}

// --------------------------------------------

static void test_102() {
    std::cout << std::endl;
    std::cout << "sizeof(util::set<T>::iterator) = " << sizeof(util::set<T>::iterator) << std::endl;
    std::cout << "sizeof(util::set<T, std::less<T>, util::global_pool_allocator<T>>) = "
              << sizeof(util::set<T, std::less<T>, util::global_pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(util::set<T, std::less<T>, util::pool_allocator<T>>) = "
              << sizeof(util::set<T, std::less<T>, util::pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(util::set<T, std::less<T>, std::allocator<T>>) = "
              << sizeof(util::set<T, std::less<T>, std::allocator<T>>) << std::endl;
    std::cout << "sizeof(util::impl::set_node_type<T>) = " << sizeof(util::impl::set_node_type<T>) << std::endl;
    std::cout << std::endl;
    std::cout << "sizeof(std::set<T>::iterator) = " << sizeof(std::set<T>::iterator) << std::endl;
    std::cout << "sizeof(std::set<T, std::less<T>, util::global_pool_allocator<T>>) = "
              << sizeof(std::set<T, std::less<T>, util::global_pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(std::set<T, std::less<T>, util::pool_allocator<T>>) = "
              << sizeof(std::set<T, std::less<T>, util::pool_allocator<T>>) << std::endl;
    std::cout << "sizeof(std::set<T, std::less<T>, std::allocator<T>>) = "
              << sizeof(std::set<T, std::less<T>, std::allocator<T>>) << std::endl;
}

// --------------------------------------------

std::pair<std::pair<size_t, void (*)()>*, size_t> get_rbtree_tests() {
    static std::pair<size_t, void (*)()> _tests[] = {
        {0, test_0},   {3, test_3},   {4, test_4},   {5, test_5},   {6, test_6},     {7, test_7},     {8, test_8},
        {9, test_9},   {10, test_10}, {11, test_11}, {12, test_12}, {13, test_13},   {18, test_18},   {19, test_19},
        {20, test_20}, {21, test_21}, {22, test_22}, {23, test_23}, {100, test_100}, {101, test_101}, {102, test_102},
    };

    return std::make_pair(_tests, sizeof(_tests) / sizeof(_tests[0]));
}
