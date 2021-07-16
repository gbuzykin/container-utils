#pragma once

#include "rbtree_multi.h"

namespace util {

//-----------------------------------------------------------------------------
// Multimap front-end

template<typename Key, typename Ty, typename Comp, typename Alloc>
class map;

template<typename Key, typename Ty, typename Comp = std::less<Key>,
         typename Alloc = std::allocator<std::pair<const Key, Ty>>>
class multimap : public impl::rbtree_multi<impl::map_node_type<Key, Ty>, Alloc, Comp> {
 private:
    using super = impl::rbtree_multi<impl::map_node_type<Key, Ty>, Alloc, Comp>;
    using alloc_traits = typename super::alloc_traits;
    using alloc_type = typename super::alloc_type;
    using node_t = typename super::node_t;

 public:
    using allocator_type = typename super::allocator_type;
    using mapped_type = typename node_t::mapped_type;
    using value_type = typename super::value_type;
    using key_compare = typename super::key_compare;
    using value_compare = typename super::value_compare_func;
    using iterator = typename super::iterator;
    using const_iterator = typename super::const_iterator;

    multimap() = default;
    explicit multimap(const allocator_type& alloc)
        NOEXCEPT_IF(std::is_nothrow_default_constructible<key_compare>::value)
        : super(alloc) {}
    explicit multimap(const key_compare& comp, const allocator_type& alloc = allocator_type()) : super(comp, alloc) {}

#if __cplusplus < 201703L
    multimap(const multimap&) = default;
    multimap& operator=(const multimap&) = default;
    multimap(multimap&& other) : super(std::move(other)) {}
    multimap& operator=(multimap&& other) {
        super::operator=(std::move(other));
        return *this;
    }
    ~multimap() = default;
#endif  // __cplusplus

    multimap(std::initializer_list<value_type> init, const allocator_type& alloc) : super(alloc) {
        this->tidy_invoke([&]() { this->insert_impl(init.begin(), init.end()); });
    }

    multimap(std::initializer_list<value_type> init, const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
        : super(comp, alloc) {
        this->tidy_invoke([&]() { this->insert_impl(init.begin(), init.end()); });
    }

    multimap& operator=(std::initializer_list<value_type> init) {
        this->assign_impl(init.begin(), init.end(), typename node_t::is_value_copy_assignable());
        return *this;
    }

    template<typename InputIt, typename = std::enable_if_t<is_input_iterator<InputIt>::value>>
    multimap(InputIt first, InputIt last, const allocator_type& alloc) : super(alloc) {
        this->tidy_invoke([&]() { this->insert_impl(first, last); });
    }

    template<typename InputIt, typename = std::enable_if_t<is_input_iterator<InputIt>::value>>
    multimap(InputIt first, InputIt last, const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
        : super(comp, alloc) {
        this->tidy_invoke([&]() { this->insert_impl(first, last); });
    }

    multimap(const multimap& other, const allocator_type& alloc) : super(other, alloc) {}
    multimap(multimap&& other, const allocator_type& alloc) : super(std::move(other), alloc) {}

    void swap(multimap& other) NOEXCEPT_IF(std::is_nothrow_swappable<key_compare>::value) {
        if (std::addressof(other) == this) { return; }
        this->swap_impl(other, typename alloc_traits::propagate_on_container_swap());
    }

    value_compare value_comp() const { return value_compare(this->get_compare()); }

    template<typename Comp2>
    void merge(map<Key, Ty, Comp2, Alloc>& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(map<Key, Ty, Comp2, Alloc>&& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(multimap<Key, Ty, Comp2, Alloc>& other) {
        this->merge_impl(std::move(other));
    }
    template<typename Comp2>
    void merge(multimap<Key, Ty, Comp2, Alloc>&& other) {
        this->merge_impl(std::move(other));
    }
};

#if __cplusplus >= 201703L
template<typename InputIt,
         typename Comp = std::less<std::remove_const_t<typename std::iterator_traits<InputIt>::value_type::first_type>>,
         typename Alloc =
             std::allocator<std::pair<std::add_const_t<typename std::iterator_traits<InputIt>::value_type::first_type>,
                                      typename std::iterator_traits<InputIt>::value_type::second_type>>>
multimap(InputIt, InputIt, Comp = Comp(), Alloc = Alloc())
    -> multimap<std::remove_const_t<typename std::iterator_traits<InputIt>::value_type::first_type>,
                typename std::iterator_traits<InputIt>::value_type::second_type, Comp, Alloc>;
template<typename Key, typename Ty, typename Comp = std::less<Key>,
         typename Alloc = std::allocator<std::pair<const Key, Ty>>>
multimap(std::initializer_list<std::pair<Key, Ty>>, Comp = Comp(), Alloc = Alloc()) -> multimap<Key, Ty, Comp, Alloc>;
template<typename InputIt, typename Alloc>
multimap(InputIt, InputIt, Alloc)
    -> multimap<std::remove_const_t<typename std::iterator_traits<InputIt>::value_type::first_type>,
                typename std::iterator_traits<InputIt>::value_type::second_type,
                std::less<std::remove_const_t<typename std::iterator_traits<InputIt>::value_type::first_type>>, Alloc>;
template<typename Key, typename Ty, typename Allocator>
multimap(std::initializer_list<std::pair<Key, Ty>>, Allocator) -> multimap<Key, Ty, std::less<Key>, Allocator>;
#endif  // __cplusplus

template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator==(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    if (lh.size() != rh.size()) { return false; }
    return std::equal(lh.begin(), lh.end(), rh.begin());
}

template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator<(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    return std::lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
}

template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator!=(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    return !(lh == rh);
}
template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator<=(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    return !(rh < lh);
}
template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator>(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    return rh < lh;
}
template<typename Key, typename Ty, typename Comp, typename Alloc>
bool operator>=(const multimap<Key, Ty, Comp, Alloc>& lh, const multimap<Key, Ty, Comp, Alloc>& rh) {
    return !(lh < rh);
}

}  // namespace util

namespace std {
template<typename Key, typename Ty, typename Comp, typename Alloc>
void swap(util::multimap<Key, Ty, Comp, Alloc>& m1, util::multimap<Key, Ty, Comp, Alloc>& m2)
    NOEXCEPT_IF(NOEXCEPT_IF(m1.swap(m2))) {
    m1.swap(m2);
}
}  // namespace std
