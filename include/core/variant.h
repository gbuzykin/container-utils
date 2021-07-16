#pragma once

#include "stream.h"

#include <array>
#include <cmath>

namespace util {

enum class variant_id : unsigned {
    kInvalid = 0,
    kBoolean,
    kInteger,
    kUInteger,
    kDouble,
    kString,
    kVector2D,
    kVector3D,
    kVector4D,
    kQuaternion,
    kMatrix4x4,
    kUser0
};

template<typename Ty>
struct variant_type_impl;
template<typename Ty, variant_id TypeId, typename = void>
struct variant_type_base_impl;

class CORE_EXPORT variant {
 private:
    template<typename... Ts>
    using aligned_storage_t = std::aligned_storage<size_of<Ts...>::value, alignment_of<Ts...>::value>;
    using storage_t = aligned_storage_t<double, std::string, void*>::type;
    using id_t = variant_id;

 public:
    enum : unsigned { kMaxTypeId = 32 };
    enum : size_t { kStorageSize = sizeof(storage_t) };

    variant() : vtable_(get_impl(id_t::kInvalid)) {}
    explicit variant(id_t type) : vtable_(get_impl(type)) {
        if (vtable_->type != id_t::kInvalid) { vtable_->construct_default(&data_); }
    }

    variant(const variant& v) : vtable_(v.vtable_) {
        if (vtable_->type != id_t::kInvalid) { vtable_->construct_copy(&data_, &v.data_); }
    }
    variant(variant&& v) NOEXCEPT : vtable_(v.vtable_) {
        if (vtable_->type != id_t::kInvalid) { vtable_->construct_move(&data_, &v.data_); }
    }
    variant(id_t type, const variant& v);

    ~variant() {
        if (vtable_->type != id_t::kInvalid) { vtable_->destroy(&data_); }
    }

    template<typename Ty, typename = std::void_t<typename variant_type_impl<Ty>::is_variant_type_impl>>
    variant(const Ty& val) : vtable_(variant_type_impl<Ty>::vtable()) {
        if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::construct(&data_, val); }
    }

    template<typename Ty, typename = std::void_t<typename variant_type_impl<Ty>::is_variant_type_impl>>
    variant(Ty&& val) : vtable_(variant_type_impl<Ty>::vtable()) {
        if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::construct(&data_, std::move(val)); }
    }

    template<typename Ty, typename = std::void_t<typename variant_type_impl<Ty>::is_variant_type_impl>>
    variant(id_t type, const Ty& val);

    variant& operator=(const variant& v);
    variant& operator=(variant&& v) NOEXCEPT;

    template<typename Ty, typename = std::void_t<typename variant_type_impl<Ty>::is_variant_type_impl>>
    variant& operator=(const Ty& val);

    template<typename Ty, typename = std::void_t<typename variant_type_impl<Ty>::is_variant_type_impl>>
    variant& operator=(Ty&& val);

    variant(std::string_view s);
    variant(id_t type, std::string_view s);
    variant& operator=(std::string_view s);

    variant(const char* cstr);
    variant(id_t type, const char* cstr);
    variant& operator=(const char* cstr);

    bool valid() const { return vtable_->type != id_t::kInvalid; }
    id_t type() const { return static_cast<id_t>(vtable_->type); }

    template<typename Ty>
    Ty value() const;
    template<typename Ty>
    bool can_convert() const {
        return can_convert(variant_type_impl<Ty>::type_id);
    }
    template<typename Ty>
    void convert() {
        convert(variant_type_impl<Ty>::type_id);
    }

    bool can_convert(id_t type) const;
    void convert(id_t type);

    bool operator!=(const variant& v) const { return !(*this == v); }
    bool operator==(const variant& v) const {
        return (vtable_ == v.vtable_) && ((vtable_->type == id_t::kInvalid) || vtable_->equals(&data_, &v.data_));
    }

    bool operator>=(const variant& v) const { return !(*this < v); }
    bool operator>(const variant& v) const { return v < *this; }
    bool operator<=(const variant& v) const { return !(v < *this); }
    bool operator<(const variant& v) const {
        return (vtable_ == v.vtable_) && (vtable_->type != id_t::kInvalid) && vtable_->less(&data_, &v.data_);
    }

#ifdef USE_QT
    CORE_EXPORT friend QDataStream& operator<<(QDataStream& os, const variant& v);
    CORE_EXPORT friend QDataStream& operator>>(QDataStream& is, variant& v);
#endif  // USE_QT

 private:
    struct vtable_t {
        id_t type;
        const void* (*value_ptr)(const void*);
        void* (*construct_default)(void*);
        void (*construct_copy)(void*, const void*);
        void (*construct_move)(void*, void*);
        void (*destroy)(void*);
        void (*assign_copy)(void*, const void*);
        void (*assign_move)(void*, void*);
        bool (*equals)(const void*, const void*);
        bool (*less)(const void*, const void*);
#ifdef USE_QT
        void (*serialize)(QDataStream&, const void*);
        void (*deserialize)(QDataStream&, void*);
#endif  // USE_QT
        using cvt_func_t = void (*)(void*, const void*);
        std::array<cvt_func_t, kMaxTypeId> cvt;
        cvt_func_t get_cvt(id_t type) {
            assert(static_cast<unsigned>(type) < kMaxTypeId);
            return cvt[static_cast<unsigned>(type)];
        }
        void set_cvt(id_t type, cvt_func_t fn) {
            assert(static_cast<unsigned>(type) < kMaxTypeId);
            cvt[static_cast<unsigned>(type)] = fn;
        }
    };

    template<typename, variant_id, typename>
    friend struct variant_type_base_impl;

    vtable_t* vtable_;
    storage_t data_;

    static vtable_t* get_impl(id_t type);
};

template<typename Ty, typename>
variant::variant(id_t type, const Ty& val) : vtable_(get_impl(type)) {
    if (vtable_->type == id_t::kInvalid) { return; }
    auto val_vtable = variant_type_impl<Ty>::vtable();
    if (vtable_ == val_vtable) {
        variant_type_impl<Ty>::construct(&data_, val);
        return;
    }
    auto tgt = vtable_->construct_default(&data_);
    if (auto cvt_func = vtable_->get_cvt(val_vtable->type)) { cvt_func(tgt, &val); }
}

template<typename Ty, typename>
variant& variant::operator=(const Ty& val) {
    auto val_vtable = variant_type_impl<Ty>::vtable();
    if (vtable_ == val_vtable) {
        if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::assign(&data_, val); }
        return *this;
    } else if (vtable_->type != id_t::kInvalid) {
        vtable_->destroy(&data_);
    }
    vtable_ = val_vtable;
    if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::construct(&data_, val); }
    return *this;
}

template<typename Ty, typename>
variant& variant::operator=(Ty&& val) {
    auto val_vtable = variant_type_impl<Ty>::vtable();
    if (vtable_ == val_vtable) {
        if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::assign(&data_, std::move(val)); }
        return *this;
    } else if (vtable_->type != id_t::kInvalid) {
        vtable_->destroy(&data_);
    }
    vtable_ = val_vtable;
    if (vtable_->type != id_t::kInvalid) { variant_type_impl<Ty>::construct(&data_, std::move(val)); }
    return *this;
}

template<typename Ty>
Ty variant::value() const {
    if (vtable_->type != id_t::kInvalid) {
        if (auto pval = vtable_->value_ptr(&data_)) {
            auto tgt_vtable = variant_type_impl<Ty>::vtable();
            if (vtable_ == tgt_vtable) {
                return *(Ty*)pval;
            } else if (auto cvt_func = tgt_vtable->get_cvt(vtable_->type)) {
                Ty result;
                cvt_func(&result, pval);
                return result;
            }
        }
    }
    return {};
}

template<typename Ty, variant_id TypeId, typename>
struct variant_type_base_impl {
    using is_variant_type_impl = int;
    static const variant_id type_id = TypeId;
    static variant::vtable_t* vtable() { return variant::get_impl(type_id); }

    variant_type_base_impl() {
        auto tbl = vtable();
        tbl->type = type_id;
        tbl->value_ptr = value_ptr;
        tbl->construct_default = construct_default;
        tbl->construct_copy = construct_copy;
        tbl->construct_move = construct_move;
        tbl->destroy = destroy;
        tbl->assign_copy = assign_copy;
        tbl->assign_move = assign_move;
        tbl->equals = equals;
        tbl->less = less;
#ifdef USE_QT
        tbl->serialize = serialize;
        tbl->deserialize = deserialize;
#endif  // USE_QT
    }

    static const void* value_ptr(const void* p) { return p; }
    template<typename Arg>
    static void construct(void* p, Arg&& val) {
        new (p) Ty(std::forward<Arg>(val));
    }
    static void* construct_default(void* p) {
        new (p) Ty();
        return p;
    }
    static void construct_copy(void* p, const void* src) { new (p) Ty(*(const Ty*)src); }
    static void construct_move(void* p, void* src) { new (p) Ty(std::move(*(Ty*)src)); }
    static void destroy(void* p) { ((Ty*)p)->~Ty(); }

    template<typename Arg>
    static void assign(void* p, Arg&& val) {
        *(Ty*)p = std::forward<Arg>(val);
    }
    static void assign_copy(void* p, const void* src) { *(Ty*)p = *(const Ty*)src; }
    static void assign_move(void* p, void* src) { *(Ty*)p = std::move(*(Ty*)src); }
    static bool equals(const void* lh, const void* rh) { return *(Ty*)lh == *(Ty*)rh; }
    static bool less(const void* lh, const void* rh) { return *(Ty*)lh < *(Ty*)rh; }
    template<typename Ty2>
    static void cast_cvt(void* tgt, const void* src) {
        *(Ty*)tgt = static_cast<Ty>(*(Ty2*)src);
    }
#ifdef USE_QT
    static void serialize(QDataStream& os, const void* p) { os << *(Ty*)p; }
    static void deserialize(QDataStream& is, void* p) { is >> *(Ty*)p; }
#endif  // USE_QT
};

template<typename Ty, variant_id TypeId>
struct variant_type_base_impl<
    Ty, TypeId,
    std::enable_if_t<(sizeof(Ty) > variant::kStorageSize) || !std::is_nothrow_move_constructible<Ty>::value ||
                     !std::is_nothrow_move_assignable<Ty>::value>> {
    using is_variant_type_impl = int;
    static const variant_id type_id = TypeId;
    static variant::vtable_t* vtable() { return variant::get_impl(type_id); }

    variant_type_base_impl() {
        auto tbl = vtable();
        tbl->type = type_id;
        tbl->value_ptr = value_ptr;
        tbl->construct_default = construct_default;
        tbl->construct_copy = construct_copy;
        tbl->construct_move = construct_move;
        tbl->destroy = destroy;
        tbl->assign_copy = assign_copy;
        tbl->assign_move = assign_move;
        tbl->equals = equals;
        tbl->less = less;
#ifdef USE_QT
        tbl->serialize = serialize;
        tbl->deserialize = deserialize;
#endif  // USE_QT
    }

    static const void* value_ptr(const void* p) { return *(const Ty**)p; }
    template<typename Arg>
    static void construct(void* p, Arg&& val) {
        *(Ty**)p = new Ty(std::forward<Arg>(val));
    }
    static void* construct_default(void* p) { return (*(Ty**)p = new Ty()); }
    static void construct_copy(void* p, const void* src) {
        if (auto pval = *(const Ty**)src) {
            *(Ty**)p = new Ty(*pval);
        } else {
            *(Ty**)p = nullptr;
        }
    }
    static void construct_move(void* p, void* src) {
        *(Ty**)p = *(Ty**)src;
        *(Ty**)src = nullptr;
    }
    static void destroy(void* p) { delete *(Ty**)p; }

    template<typename Arg>
    static void assign(void* p, Arg&& val) {
        if (*(Ty**)p) {
            **(Ty**)p = std::forward<Arg>(val);
        } else {
            *(Ty**)p = new Ty(std::forward<Arg>(val));
        }
    }
    static void assign_copy(void* p, const void* src) {
        if (auto pval = *(const Ty**)src) {
            assign(p, *pval);
        } else {
            delete *(Ty**)p;
        }
    }
    static void assign_move(void* p, void* src) {
        delete *(Ty**)p;
        *(Ty**)p = *(Ty**)src;
        *(Ty**)src = nullptr;
    }
    static bool equals(const void* lh, const void* rh) {
        return (*(Ty**)lh && *(Ty**)rh) ? (**(Ty**)lh == **(Ty**)rh) : false;
    }
    static bool less(const void* lh, const void* rh) {
        return (*(Ty**)lh && *(Ty**)rh) ? (**(Ty**)lh < **(Ty**)rh) : false;
    }
    template<typename Ty2>
    static void cast_cvt(void* tgt, const void* src) {
        *(Ty*)tgt = static_cast<Ty>(*(Ty2*)src);
    }
#ifdef USE_QT
    static void serialize(QDataStream& os, const void* p) { os << ((*(Ty**)p) ? **(Ty**)p : Ty()); }
    static void deserialize(QDataStream& is, void* p) {
        if (!*(Ty**)p) { *(Ty**)p = new Ty(); }
        is >> **(Ty**)p;
    }
#endif  // USE_QT
};

template<>
struct variant_type_impl<std::string> : variant_type_base_impl<std::string, variant_id::kString> {};

inline variant::variant(std::string_view s) : variant(static_cast<std::string>(s)) {}
inline variant::variant(id_t type, std::string_view s) : variant(type, static_cast<std::string>(s)) {}
inline variant& variant::operator=(std::string_view s) { return *this = static_cast<std::string>(s); }

inline variant::variant(const char* cstr) : variant(std::string(cstr)) {}
inline variant::variant(id_t type, const char* cstr) : variant(type, std::string(cstr)) {}
inline variant& variant::operator=(const char* cstr) { return *this = std::string(cstr); }

template<typename Ty, variant_id TypeId>
struct variant_type_with_string_converter_impl : variant_type_base_impl<Ty, TypeId> {
    using super = variant_type_base_impl<Ty, TypeId>;
    variant_type_with_string_converter_impl() {
        super::vtable()->set_cvt(variant_id::kString, from_string_cvt);
        variant_type_impl<std::string>::vtable()->set_cvt(super::type_id, to_string_cvt);
    }
    static void from_string_cvt(void* tgt, const void* src) { *(Ty*)tgt = from_string<Ty>(*(std::string*)src); }
    static void to_string_cvt(void* tgt, const void* src) { *(std::string*)tgt = to_string(*(Ty*)src); }
};

template<>
struct variant_type_impl<bool> : variant_type_with_string_converter_impl<bool, variant_id::kBoolean> {
    template<typename Ty>
    static void cast_to_bool_cvt(void* tgt, const void* src) {
        *(bool*)tgt = *(Ty*)src != 0;
    }
    variant_type_impl() {
        vtable()->set_cvt(variant_id::kInteger, cast_to_bool_cvt<int>);
        vtable()->set_cvt(variant_id::kUInteger, cast_to_bool_cvt<unsigned>);
        vtable()->set_cvt(variant_id::kDouble, cast_to_bool_cvt<double>);
    }
};

template<>
struct variant_type_impl<int> : variant_type_with_string_converter_impl<int, variant_id::kInteger> {
    variant_type_impl() {
        vtable()->set_cvt(variant_id::kBoolean, cast_cvt<bool>);
        vtable()->set_cvt(variant_id::kUInteger, cast_cvt<unsigned>);
        vtable()->set_cvt(variant_id::kDouble, cast_cvt<double>);
    }
};

template<>
struct variant_type_impl<unsigned> : variant_type_with_string_converter_impl<unsigned, variant_id::kUInteger> {
    variant_type_impl() {
        vtable()->set_cvt(variant_id::kBoolean, cast_cvt<bool>);
        vtable()->set_cvt(variant_id::kInteger, cast_cvt<int>);
        vtable()->set_cvt(variant_id::kDouble, cast_cvt<double>);
    }
};

template<>
struct variant_type_impl<double> : variant_type_with_string_converter_impl<double, variant_id::kDouble> {
    variant_type_impl() {
        vtable()->set_cvt(variant_id::kBoolean, cast_cvt<bool>);
        vtable()->set_cvt(variant_id::kInteger, cast_cvt<int>);
        vtable()->set_cvt(variant_id::kUInteger, cast_cvt<unsigned>);
    }
};

}  // namespace util
