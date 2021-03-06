#include "util_rbtree.h"

namespace util {

namespace impl {

//-----------------------------------------------------------------------------
// Node handle implementation

template<typename NodeTy, typename Alloc, typename NodeHandle, typename = void>
class rbtree_node_handle_getters : protected std::allocator_traits<Alloc>::template rebind_alloc<NodeTy> {
 protected:
    using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeTy>;
    using node_t = NodeTy;

 public:
    using value_type = typename node_t::value_type;
    rbtree_node_handle_getters() NOEXCEPT_IF(std::is_nothrow_default_constructible<alloc_type>::value) {}
    explicit rbtree_node_handle_getters(const alloc_type& alloc) NOEXCEPT : alloc_type(alloc) {}
    ~rbtree_node_handle_getters() = default;
    rbtree_node_handle_getters(const rbtree_node_handle_getters&) = delete;
    rbtree_node_handle_getters& operator=(const rbtree_node_handle_getters&) = delete;
    rbtree_node_handle_getters(rbtree_node_handle_getters&& other) : alloc_type(std::move(other)) {}
    rbtree_node_handle_getters& operator=(rbtree_node_handle_getters&& other) {
        alloc_type::operator=(std::move(other));
        return *this;
    }
    value_type& value() const { return node_t::get_value(static_cast<const NodeHandle*>(this)->node_); }
};

template<typename NodeTy, typename Alloc, typename NodeHandle>
class rbtree_node_handle_getters<NodeTy, Alloc, NodeHandle, std::void_t<typename NodeTy::mapped_type>>
    : protected std::allocator_traits<Alloc>::template rebind_alloc<NodeTy> {
 protected:
    using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeTy>;
    using node_t = NodeTy;

 public:
    using key_type = typename node_t::key_type;
    using mapped_type = typename node_t::mapped_type;
    rbtree_node_handle_getters() NOEXCEPT_IF(std::is_nothrow_default_constructible<alloc_type>::value) {}
    explicit rbtree_node_handle_getters(const alloc_type& alloc) NOEXCEPT : alloc_type(alloc) {}
    ~rbtree_node_handle_getters() = default;
    rbtree_node_handle_getters(const rbtree_node_handle_getters&) = delete;
    rbtree_node_handle_getters& operator=(const rbtree_node_handle_getters&) = delete;
    rbtree_node_handle_getters(rbtree_node_handle_getters&& other) : alloc_type(std::move(other)) {}
    rbtree_node_handle_getters& operator=(rbtree_node_handle_getters&& other) {
        alloc_type::operator=(std::move(other));
        return *this;
    }
    key_type& key() const {
        return const_cast<key_type&>(node_t::get_value(static_cast<const NodeHandle*>(this)->node_).first);
    }
    mapped_type& mapped() const { return node_t::get_value(static_cast<const NodeHandle*>(this)->node_).second; }
};

template<typename Traits, typename NodeTy>
class rbtree_node_handle
    : public rbtree_node_handle_getters<NodeTy, typename Traits::allocator_type, rbtree_node_handle<Traits, NodeTy>> {
 private:
    using super = rbtree_node_handle_getters<NodeTy, typename Traits::allocator_type, rbtree_node_handle>;
    using alloc_type = typename super::alloc_type;
    using node_t = typename super::node_t;

 public:
    using allocator_type = typename Traits::allocator_type;

    rbtree_node_handle() = default;
    rbtree_node_handle(rbtree_node_handle&& nh) NOEXCEPT : super(std::move(nh)) {
        node_ = nh.node_;
        nh.node_ = nullptr;
    }

    rbtree_node_handle& operator=(rbtree_node_handle&& nh) NOEXCEPT {
        assert(std::addressof(nh) != this);
        if (std::addressof(nh) == this) { return *this; }
        if (node_) { Traits::helpers::delete_node(*this, node_); }
        super::operator=(std::move(nh));
        node_ = nh.node_;
        nh.node_ = nullptr;
        return *this;
    }

    ~rbtree_node_handle() {
        if (node_) { Traits::helpers::delete_node(*this, node_); }
    }

    allocator_type get_allocator() const { return static_cast<const alloc_type&>(*this); }
    bool empty() const NOEXCEPT { return node_ == nullptr; }
    explicit operator bool() const NOEXCEPT { return node_ != nullptr; }
    void swap(rbtree_node_handle& nh) NOEXCEPT {
        if (std::addressof(nh) == this) { return; }
        std::swap(static_cast<alloc_type&>(*this), static_cast<alloc_type&>(nh));
        std::swap(node_, nh.node_);
    }

 protected:
    rbtree_node_t* node_ = nullptr;

    template<typename, typename, typename, typename>
    friend class rbtree_node_handle_getters;
    template<typename, typename, typename>
    friend class rbtree_base;
    template<typename, typename, typename>
    friend class rbtree;
    template<typename, typename, typename>
    friend class rbtree_multi;

    explicit rbtree_node_handle(const alloc_type& alloc) NOEXCEPT : super(alloc) {}
    rbtree_node_handle(const alloc_type& alloc, rbtree_node_t* node) NOEXCEPT : super(alloc), node_(node) {}
};

}  // namespace impl

}  // namespace util

namespace std {
template<typename Traits, typename NodeTy>
void swap(util::impl::rbtree_node_handle<Traits, NodeTy>& nh1, util::impl::rbtree_node_handle<Traits, NodeTy>& nh2)
    NOEXCEPT_IF(NOEXCEPT_IF(nh1.swap(nh2))) {
    nh1.swap(nh2);
}
}  // namespace std
