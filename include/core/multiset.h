#pragma once

#include "rbtree_multi.h"

namespace util {

//-----------------------------------------------------------------------------
// Multiset front-end

template<typename Key, typename Comp, typename Alloc>
class set;

template<typename Key, typename Comp = std::less<Key>, typename Alloc = std::allocator<Key>>
class multiset : public impl::rbtree_multi<impl::set_node_type<Key>, Alloc, Comp> {
 private:
    using super = impl::rbtree_multi<impl::set_node_type<Key>, Alloc, Comp>;
    using alloc_traits = typename super::alloc_traits;
    using alloc_type = typename super::alloc_type;
    using node_t = typename super::node_t;

 public:
    using allocator_type = typename super::allocator_type;
    using value_type = typename super::value_type;
    using key_compare = typename super::key_compare;
    using value_compare = typename super::key_compare;

    multiset() = default;
    explicit multiset(const allocator_type& alloc)
        NOEXCEPT_IF(std::is_nothrow_default_constructible<key_compare>::value)
        : super(alloc) {}
    explicit multiset(const key_compare& comp, const allocator_type& alloc = allocator_type()) : super(comp, alloc) {}

#if __cplusplus < 201703L
    multiset(const multiset&) = default;
    multiset& operator=(const multiset&) = default;
    multiset(multiset&& other) : super(std::move(other)) {}
    multiset& operator=(multiset&& other) {
        super::operator=(std::move(other));
        return *this;
    }
    ~multiset() = default;
#endif  // __cplusplus

    multiset(std::initializer_list<value_type> init, const allocator_type& alloc) : super(alloc) {
        this->tidy_invoke([&]() { this->insert_impl(init.begin(), init.end()); });
    }

    multiset(std::initializer_list<value_type> init, const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
        : super(comp, alloc) {
        this->tidy_invoke([&]() { this->insert_impl(init.begin(), init.end()); });
    }

    multiset& operator=(std::initializer_list<value_type> init) {
        this->assign_impl(init.begin(), init.end(), typename node_t::is_value_copy_assignable());
        return *this;
    }

    template<typename InputIt, typename = std::enable_if_t<is_input_iterator<InputIt>::value>>
    multiset(InputIt first, InputIt last, const allocator_type& alloc) : super(alloc) {
        this->tidy_invoke([&]() { this->insert_impl(first, last); });
    }

    template<typename InputIt, typename = std::enable_if_t<is_input_iterator<InputIt>::value>>
    multiset(InputIt first, InputIt last, const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
        : super(comp, alloc) {
        this->tidy_invoke([&]() { this->insert_impl(first, last); });
    }

    multiset(const multiset& other, const allocator_type& alloc) : super(other, alloc) {}
    multiset(multiset&& other, const allocator_type& alloc) : super(std::move(other), alloc) {}

    void swap(multiset& other) NOEXCEPT_IF(std::is_nothrow_swappable<key_compare>::value) {
        if (std::addressof(other) == this) { return; }
        this->swap_impl(other, typename alloc_traits::propagate_on_container_swap());
    }

    value_compare value_comp() const { return this->get_compare(); }

    template<typename Comp2>
    void merge(set<Key, Comp2, Alloc>& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(set<Key, Comp2, Alloc>&& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(multiset<Key, Comp2, Alloc>& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(multiset<Key, Comp2, Alloc>&& other) {
        this->merge_impl(std::move(other));
    }
};

#if __cplusplus >= 201703L
template<typename InputIt, typename Comp = std::less<typename std::iterator_traits<InputIt>::value_type>,
         typename Alloc = std::allocator<typename std::iterator_traits<InputIt>::value_type>>
multiset(InputIt, InputIt, Comp = Comp(), Alloc = Alloc())
    -> multiset<typename std::iterator_traits<InputIt>::value_type, Comp, Alloc>;
template<typename Key, typename Comp = std::less<Key>, typename Alloc = std::allocator<Key>>
multiset(std::initializer_list<Key>, Comp = Comp(), Alloc = Alloc()) -> multiset<Key, Comp, Alloc>;
template<typename InputIt, typename Alloc>
multiset(InputIt, InputIt, Alloc) -> multiset<typename std::iterator_traits<InputIt>::value_type,
                                              std::less<typename std::iterator_traits<InputIt>::value_type>, Alloc>;
template<typename Key, typename Alloc>
multiset(std::initializer_list<Key>, Alloc) -> multiset<Key, std::less<Key>, Alloc>;
#endif  // __cplusplus

template<typename Key, typename Comp, typename Alloc>
bool operator==(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    if (lh.size() != rh.size()) { return false; }
    return std::equal(lh.begin(), lh.end(), rh.begin());
}

template<typename Key, typename Comp, typename Alloc>
bool operator<(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    return std::lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
}

template<typename Key, typename Comp, typename Alloc>
bool operator!=(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    return !(lh == rh);
}
template<typename Key, typename Comp, typename Alloc>
bool operator<=(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    return !(rh < lh);
}
template<typename Key, typename Comp, typename Alloc>
bool operator>(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    return rh < lh;
}
template<typename Key, typename Comp, typename Alloc>
bool operator>=(const multiset<Key, Comp, Alloc>& lh, const multiset<Key, Comp, Alloc>& rh) {
    return !(lh < rh);
}

}  // namespace util

namespace std {
template<typename Key, typename Comp, typename Alloc>
void swap(util::multiset<Key, Comp, Alloc>& s1, util::multiset<Key, Comp, Alloc>& s2)
    NOEXCEPT_IF(NOEXCEPT_IF(s1.swap(s2))) {
    s1.swap(s2);
}
}  // namespace std
