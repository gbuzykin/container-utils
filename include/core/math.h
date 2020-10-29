#pragma once

#include "variant.h"

#ifdef USE_QT
#    include <QColor>
#    include <QMatrix4x4>
#    include <QQuaternion>
#    include <QVector2D>
#    include <QVector3D>
#    include <QVector4D>

#    include <osg/Matrix>
#    include <osg/Quat>
#    include <osg/Vec2>
#    include <osg/Vec3>
#    include <osg/Vec4>

inline QDataStream& operator<<(QDataStream& os, const osg::Vec2& v) { return os << v.x() << v.y(); }

inline QDataStream& operator>>(QDataStream& is, osg::Vec2& v) { return is >> v.x() >> v.y(); }

inline QDataStream& operator<<(QDataStream& os, const osg::Vec3& v) { return os << v.x() << v.y() << v.z(); }

inline QDataStream& operator>>(QDataStream& is, osg::Vec3& v) { return is >> v.x() >> v.y() >> v.z(); }

inline QDataStream& operator<<(QDataStream& os, const osg::Vec4& v) { return os << v.x() << v.y() << v.z() << v.w(); }

inline QDataStream& operator>>(QDataStream& is, osg::Vec4& v) { return is >> v.x() >> v.y() >> v.z() >> v.w(); }

inline QDataStream& operator<<(QDataStream& os, const osg::Quat& q) { return os << q.x() << q.y() << q.z() << q.w(); }

inline QDataStream& operator>>(QDataStream& is, osg::Quat& q) { return is >> q.x() >> q.y() >> q.z() >> q.w(); }

inline QDataStream& operator<<(QDataStream& os, const osg::Matrix& m) {
    auto data = m.ptr();
    for (int i = 0; i < 16; i++) { os << data[i]; }
    return os;
}

inline QDataStream& operator>>(QDataStream& is, osg::Matrix& m) {
    auto data = m.ptr();
    for (int i = 0; i < 16; i++) { is >> data[i]; }
    return is;
}
#endif  // USE_QT

namespace vrc {
namespace math {

struct vec2 {
    std::array<float, 2> v_;
    vec2() { v_.fill(0); }
    vec2(float x, float y) {
        v_[0] = x;
        v_[1] = y;
    }
    const float* ptr() const { return v_.data(); }
    float* ptr() { return v_.data(); }
    float x() const { return v_[0]; }
    float& x() { return v_[0]; }
    float y() const { return v_[1]; }
    float& y() { return v_[1]; }
    friend vec2 operator+(vec2 lhv, vec2 rhv) { return {}; }
    friend vec2 operator-(vec2 lhv, vec2 rhv) { return {}; }
    friend vec2 operator*(vec2 v, float s) { return {}; }
    friend vec2 operator/(vec2 v, float s) { return {}; }
    vec2& operator+=(vec2 v) { return *this; }
    vec2& operator-=(vec2 v) { return *this; }
    vec2& operator*=(float s) { return *this; }
    vec2& operator/=(float s) { return *this; }
    friend bool operator==(const vec2& lhv, const vec2& rhv) { return lhv.v_ == rhv.v_; }
    friend bool operator!=(const vec2& lhv, const vec2& rhv) { return lhv.v_ != rhv.v_; }
    friend bool operator<(const vec2& lhv, const vec2& rhv) { return lhv.v_ < rhv.v_; }
};

struct vec3 {
    std::array<float, 3> v_;
    vec3() { v_.fill(0); }
    vec3(float x, float y, float z) {
        v_[0] = x;
        v_[1] = y;
        v_[2] = z;
    }
    const float* ptr() const { return v_.data(); }
    float* ptr() { return v_.data(); }
    float x() const { return v_[0]; }
    float& x() { return v_[0]; }
    float y() const { return v_[1]; }
    float& y() { return v_[1]; }
    float z() const { return v_[2]; }
    float& z() { return v_[2]; }
    friend vec3 operator+(vec3 lhv, vec3 rhv) { return {}; }
    vec3& operator+=(vec3 v) { return *this; }
    vec3 operator-() { return {}; }
    friend bool operator==(const vec3& lhv, const vec3& rhv) { return lhv.v_ == rhv.v_; }
    friend bool operator!=(const vec3& lhv, const vec3& rhv) { return lhv.v_ != rhv.v_; }
    friend bool operator<(const vec3& lhv, const vec3& rhv) { return lhv.v_ < rhv.v_; }
};

struct vec4 {
    std::array<float, 4> v_;
    vec4() { v_.fill(0); }
    vec4(float x, float y, float z, float w) {
        v_[0] = x;
        v_[1] = y;
        v_[2] = z;
        v_[3] = w;
    }
    const float* ptr() const { return v_.data(); }
    float* ptr() { return v_.data(); }
    float x() const { return v_[0]; }
    float& x() { return v_[0]; }
    float y() const { return v_[1]; }
    float& y() { return v_[1]; }
    float z() const { return v_[2]; }
    float& z() { return v_[2]; }
    float w() const { return v_[3]; }
    float& w() { return v_[3]; }
    friend bool operator==(const vec4& lhv, const vec4& rhv) { return lhv.v_ == rhv.v_; }
    friend bool operator!=(const vec4& lhv, const vec4& rhv) { return lhv.v_ != rhv.v_; }
    friend bool operator<(const vec4& lhv, const vec4& rhv) { return lhv.v_ < rhv.v_; }
};

struct quat {
    std::array<float, 4> v_;
    quat() { v_.fill(0); }
    quat(float x, float y, float z, float w) {
        v_[0] = x;
        v_[1] = y;
        v_[2] = z;
        v_[3] = w;
    }
    const float* ptr() const { return v_.data(); }
    float* ptr() { return v_.data(); }
    float x() const { return v_[0]; }
    float& x() { return v_[0]; }
    float y() const { return v_[1]; }
    float& y() { return v_[1]; }
    float z() const { return v_[2]; }
    float& z() { return v_[2]; }
    float w() const { return v_[3]; }
    float& w() { return v_[3]; }
    friend vec3 operator*(quat q, vec3 v) { return {}; }
    friend quat operator*(quat lhq, quat rhq) { return {}; }
    quat& operator*=(quat q) { return *this; }
    quat inverse() const { return {}; }
    friend bool operator==(const quat& lhq, const quat& rhq) { return lhq.v_ == rhq.v_; }
    friend bool operator!=(const quat& lhq, const quat& rhq) { return lhq.v_ != rhq.v_; }
    friend bool operator<(const quat& lhq, const quat& rhq) { return lhq.v_ < rhq.v_; }
};

struct mat4 {
    std::array<float, 16> v_;
    mat4() {
        v_.fill(0);
        v_[0] = v_[5] = v_[10] = v_[15] = 1;
    }
    const float* ptr() const { return v_.data(); }
    float* ptr() { return v_.data(); }
    friend bool operator==(const mat4& lhm, const mat4& rhm) { return lhm.v_ == rhm.v_; }
    friend bool operator!=(const mat4& lhm, const mat4& rhm) { return lhm.v_ != rhm.v_; }
    friend bool operator<(const mat4& lhm, const mat4& rhm) { return lhm.v_ < rhm.v_; }
};

namespace refln {
enum id_t : int { left = 0, right, bottom, top, hor_center, ver_center, side_count = 4, count = 6 };

enum : unsigned {
    kNoFlags = 0,
    left_bit = 0x01,
    right_bit = 0x02,
    bottom_bit = 0x04,
    top_bit = 0x08,
    hor_center_bit = 0x10,
    ver_center_bit = 0x20,
    all_side_bits = 0xf,
    all_bits = 0x3f
};

inline unsigned bit(unsigned id) { return 1 << id; }
}  // namespace refln

class rect {
 public:
    rect() {}
    rect(float x, float y, float w, float h) : _left_bottom(x, y), _right_top(x + w, y + h) {}
    rect(vec2 lb, vec2 rt) : _left_bottom(lb), _right_top(rt) {}
    rect(vec2 lb, float w, float h) : _left_bottom(lb), _right_top(lb.x() + w, lb.y() + h) {}
    rect(const rect& r) = default;
    rect& operator=(const rect& pt) = default;

    float width() const { return _right_top.x() - _left_bottom.x(); }
    float height() const { return _right_top.y() - _left_bottom.y(); }

    float left() const { return _left_bottom.x(); }
    float hor_center() const { return .5f * (left() + right()); }
    float right() const { return _right_top.x(); }

    float bottom() const { return _left_bottom.y(); }
    float ver_center() const { return .5f * (bottom() + top()); }
    float top() const { return _right_top.y(); }

    float refln_pos(refln::id_t ln) const {
        switch (ln) {
            case refln::id_t::right: return right();
            case refln::id_t::bottom: return bottom();
            case refln::id_t::top: return top();
            case refln::id_t::hor_center: return hor_center();
            case refln::id_t::ver_center: return ver_center();
            default: return left();
        }

        return left();
    }

    vec2 left_bottom() const { return _left_bottom; }
    vec2 left_center() const { return {left(), ver_center()}; }
    vec2 left_top() const { return {left(), top()}; }

    vec2 right_bottom() const { return {right(), bottom()}; }
    vec2 right_center() const { return {right(), ver_center()}; }
    vec2 right_top() const { return _right_top; }

    vec2 center_bottom() const { return {hor_center(), bottom()}; }
    vec2 center() const { return {hor_center(), ver_center()}; }
    vec2 center_top() const { return {hor_center(), top()}; }

    bool valid() const { return (width() >= 0) && (height() >= 0); }

    bool contains(vec2 pt) const {
        return (pt.x() >= left()) && (pt.x() <= right()) && (pt.y() >= bottom()) && (pt.y() <= top());
    }

    float& left() { return _left_bottom.x(); }
    float& bottom() { return _left_bottom.y(); }
    float& right() { return _right_top.x(); }
    float& top() { return _right_top.y(); }
    vec2& left_bottom() { return _left_bottom; }
    vec2& right_top() { return _right_top; }

    void move_hor(float off) {
        _left_bottom.x() += off;
        _right_top.x() += off;
    }
    void move_ver(float off) {
        _left_bottom.y() += off;
        _right_top.y() += off;
    }

    void set_left_bottom(vec2 pt) {
        left() = pt.x();
        bottom() = pt.y();
    }
    void set_left_top(vec2 pt) {
        left() = pt.x();
        top() = pt.y();
    }
    void set_right_bottom(vec2 pt) {
        right() = pt.x();
        bottom() = pt.y();
    }
    void set_right_top(vec2 pt) {
        right() = pt.x();
        top() = pt.y();
    }

    void move_left_to(float l) {
        float w = width();
        _left_bottom.x() = l;
        _right_top.x() = l + w;
    }
    void move_hor_center_to(float c) {
        float w = width();
        _left_bottom.x() = c - .5f * w;
        _right_top.x() = c + .5f * w;
    }
    void move_right_to(float r) {
        float w = width();
        _left_bottom.x() = r - w;
        _right_top.x() = r;
    }

    void move_bottom_to(float b) {
        float h = height();
        _left_bottom.y() = b;
        _right_top.y() = b + h;
    }
    void move_ver_center_to(float c) {
        float h = height();
        _left_bottom.y() = c - .5f * h;
        _right_top.y() = c + .5f * h;
    }
    void move_top_to(float t) {
        float h = height();
        _left_bottom.y() = t - h;
        _right_top.y() = t;
    }

    void move_left_bottom_to(vec2 pt) {
        move_left_to(pt.x());
        move_bottom_to(pt.y());
    }
    void move_left_center_to(vec2 pt) {
        move_left_to(pt.x());
        move_ver_center_to(pt.y());
    }
    void move_left_top_to(vec2 pt) {
        move_left_to(pt.x());
        move_top_to(pt.y());
    }

    void move_right_bottom_to(vec2 pt) {
        move_right_to(pt.x());
        move_bottom_to(pt.y());
    }
    void move_right_center_to(vec2 pt) {
        move_right_to(pt.x());
        move_ver_center_to(pt.y());
    }
    void move_right_top_to(vec2 pt) {
        move_right_to(pt.x());
        move_top_to(pt.y());
    }

    void move_center_bottom_to(vec2 pt) {
        move_hor_center_to(pt.x());
        move_bottom_to(pt.y());
    }
    void move_center_to(vec2 pt) {
        move_hor_center_to(pt.x());
        move_ver_center_to(pt.y());
    }
    void move_center_top_to(vec2 pt) {
        move_hor_center_to(pt.x());
        move_top_to(pt.y());
    }

    void expand(vec2 off) {
        _left_bottom -= off;
        _right_top += off;
    }
    void collapse(vec2 off) {
        _left_bottom += off;
        _right_top -= off;
    }

    void expand(float off) {
        _left_bottom -= vec2(off, off);
        _right_top += vec2(off, off);
    }
    void collapse(float off) {
        _left_bottom += vec2(off, off);
        _right_top -= vec2(off, off);
    }

    rect expanded(vec2 off) const { return {_left_bottom - off, _right_top + off}; }
    rect collapsed(vec2 off) const { return {_left_bottom + off, _right_top - off}; }

    rect expanded(float off) const { return {_left_bottom - vec2(off, off), _right_top + vec2(off, off)}; }
    rect collapsed(float off) const { return {_left_bottom + vec2(off, off), _right_top - vec2(off, off)}; }

    rect operator*(float s) const { return {_left_bottom * s, _right_top * s}; }
    rect operator/(float s) const { return {_left_bottom / s, _right_top / s}; }

    rect& operator*=(float s) {
        _left_bottom *= s;
        _right_top *= s;
        return *this;
    }
    rect& operator/=(float s) {
        _left_bottom /= s;
        _right_top /= s;
        return *this;
    }

    rect operator+(vec2 off) const { return {_left_bottom + off, _right_top + off}; }
    rect operator+(const rect& off) const { return {_left_bottom + off._left_bottom, _right_top + off._right_top}; }
    rect operator-(vec2 off) const { return {_left_bottom - off, _right_top - off}; }
    rect operator-(const rect& off) const { return {_left_bottom - off._left_bottom, _right_top - off._right_top}; }

    rect& operator+=(vec2 off) {
        _left_bottom += off;
        _right_top += off;
        return *this;
    }
    rect& operator+=(const rect& off) {
        _left_bottom += off._left_bottom;
        _right_top += off._right_top;
        return *this;
    }
    rect& operator-=(vec2 off) {
        _left_bottom -= off;
        _right_top -= off;
        return *this;
    }
    rect& operator-=(const rect& off) {
        _left_bottom -= off._left_bottom;
        _right_top -= off._right_top;
        return *this;
    }

    void intersect(const rect& r) {
        if (r.left() > left()) { left() = r.left(); }
        if (r.bottom() > bottom()) { bottom() = r.bottom(); }
        if (r.right() < right()) { right() = r.right(); }
        if (r.top() < top()) { top() = r.top(); }
    }

    void combine(const rect& r) {
        if (r.left() < left()) { left() = r.left(); }
        if (r.bottom() < bottom()) { bottom() = r.bottom(); }
        if (r.right() > right()) { right() = r.right(); }
        if (r.top() > top()) { top() = r.top(); }
    }

    rect intersection(const rect& r) const {
        rect res = *this;
        res.intersect(r);
        return res;
    }
    rect combination(const rect& r) const {
        rect res = *this;
        res.combine(r);
        return res;
    }
#ifdef USE_QT
    friend QDataStream& operator<<(QDataStream& os, const rect& r) { return os << r._left_bottom << r._right_top; }
    friend QDataStream& operator>>(QDataStream& is, rect& r) { return is >> r._left_bottom >> r._right_top; }
#endif  // USE_QT

 private:
    vec2 _left_bottom;
    vec2 _right_top;
};

class CORE_EXPORT pos_6d {
 public:
    pos_6d() {}
    pos_6d(const pos_6d& pos) : _position(pos._position), _orient(pos._orient) {}
    explicit pos_6d(const vec3& position) : _position(position) {}
    pos_6d(const vec3& position, const quat& orient) : _position(position), _orient(orient) {}

    pos_6d& operator=(const pos_6d& pos) {
        if (&pos != this) {
            _position = pos._position;
            _orient = pos._orient;
        }
        return *this;
    }

    const vec3& position() const { return _position; }
    vec3& position() { return _position; }

    const quat& orient() const { return _orient; }
    quat& orient() { return _orient; }

    pos_6d operator*(const pos_6d& pos) const {
        pos_6d res = *this;
        res.post_mult(pos);
        return res;
    }

    pos_6d& operator*=(const pos_6d& pos) {
        post_mult(pos);
        return *this;
    }

    void post_mult(const pos_6d& pos) {
        _position = pos._orient * _position + pos._position;
        _orient *= pos._orient;
    }

    void pre_mult(const pos_6d& pos) {
        _position += _orient * pos._position;
        _orient = pos._orient * _orient;
    }

    vec3 pre_mult(const vec3& v) const { return _orient * v + _position; }

    void invert() {
        _orient = _orient.inverse();
        _position = _orient * (-_position);
    }

    pos_6d inverse() const {
        pos_6d pos = *this;
        pos.invert();
        return pos;
    }

    void eye_dir_up(vec3& eye, vec3& dir, vec3& up) const;
    static pos_6d make_look_at(const vec3& eye, const vec3& dir, const vec3& up);
    static pos_6d from_eye_dir_up(const vec3& eye, const vec3& dir, const vec3& up);

    mat4 matrix() const;
    static pos_6d from_matrix(const mat4& m);

    friend vec3 operator*(const vec3& v, const pos_6d& pos) { return pos.pre_mult(v); }
#ifdef USE_QT
    friend QDataStream& operator<<(QDataStream& os, const pos_6d& pos) { return os << pos._position << pos._orient; }
    friend QDataStream& operator>>(QDataStream& is, pos_6d& pos) { return is >> pos._position >> pos._orient; }
#endif  // USE_QT

 private:
    vec3 _position;
    quat _orient;
};

CORE_EXPORT vec3 make_nonzero(const vec3& v);
CORE_EXPORT quat from_krylov_to_quat(const vec3& v);
CORE_EXPORT vec3 from_quat_to_krylov(const quat& q);

}  // namespace math
}  // namespace vrc

namespace util {

#ifdef USE_QT
template<>
struct qt_type_converter<QVector2D> {
    using is_qt_type_converter = int;
    QVector2D to_qt(const vrc::math::vec2& val) { return QVector2D(val.x(), val.y()); }
    template<class Ty>
    Ty from_qt(const QVector2D& val) {
        return {val.x(), val.y()};
    }
};

template<>
struct qt_type_converter<QVector3D> {
    using is_qt_type_converter = int;
    QVector3D to_qt(const vrc::math::vec3& val) { return QVector3D(val.x(), val.y(), val.z()); }
    template<class Ty>
    Ty from_qt(const QVector3D& val) {
        return {val.x(), val.y(), val.z()};
    }
};

template<>
struct qt_type_converter<QVector4D> {
    using is_qt_type_converter = int;
    QVector4D to_qt(const vrc::math::vec4& val) { return QVector4D(val.x(), val.y(), val.z(), val.w()); }
    template<class Ty>
    Ty from_qt(const QVector4D& val) {
        return {val.x(), val.y(), val.z(), val.w()};
    }
};

template<>
struct qt_type_converter<QColor> {
    using is_qt_type_converter = int;
    QColor to_qt(const vrc::math::vec4& val) { return QColor::fromRgbF(val.x(), val.y(), val.z(), val.w()); };
    template<class Ty>
    Ty from_qt(const QColor& val) {
        return {val.redF(), val.greenF(), val.blueF(), val.alphaF()};
    }
};

template<>
struct qt_type_converter<QQuaternion> {
    using is_qt_type_converter = int;
    QQuaternion to_qt(const vrc::math::quat& val) { return QQuaternion(val.w(), val.x(), val.y(), val.z()); }
    template<class Ty>
    Ty from_qt(const QQuaternion& val) {
        return {val.x(), val.y(), val.z(), val.scalar()};
    }
};

template<>
struct qt_type_converter<QMatrix4x4> {
    using is_qt_type_converter = int;
    CORE_EXPORT QMatrix4x4 to_qt(const vrc::math::mat4& val);
    template<class Ty>
    Ty from_qt(const QMatrix4x4& val);
};

template<>
struct qt_type_converter<QVariant> {
    using is_qt_type_converter = int;
    CORE_EXPORT QVariant to_qt(const variant& val);
    template<class Ty>
    Ty from_qt(const QVariant& val);
};

template<>
CORE_EXPORT vrc::math::mat4 qt_type_converter<QMatrix4x4>::from_qt(const QMatrix4x4& val);
template<>
CORE_EXPORT variant qt_type_converter<QVariant>::from_qt(const QVariant& val);
#endif  // USE_QT

template<>
struct string_converter<vrc::math::vec2> : string_converter_base<vrc::math::vec2> {
    static void from_string(std::string_view s, vrc::math::vec2& val) {
        separate_words(s, ',', val.ptr(), 2, util::from_string<float>);
    }
    static std::string to_string(const vrc::math::vec2& val) {
        return join_strings(util::make_range(val.ptr(), val.ptr() + 2), " ", util::to_string<float>);
    }
};

template<>
struct string_converter<vrc::math::vec3> : string_converter_base<vrc::math::vec3> {
    static void from_string(std::string_view s, vrc::math::vec3& val) {
        separate_words(s, ',', val.ptr(), 3, util::from_string<float>);
    }
    static std::string to_string(const vrc::math::vec3& val) {
        return join_strings(util::make_range(val.ptr(), val.ptr() + 3), " ", util::to_string<float>);
    }
};

template<>
struct string_converter<vrc::math::vec4> : string_converter_base<vrc::math::vec4> {
    static void from_string(std::string_view s, vrc::math::vec4& val) {
        separate_words(s, ',', val.ptr(), 4, util::from_string<float>);
    }
    static std::string to_string(const vrc::math::vec4& val) {
        return join_strings(util::make_range(val.ptr(), val.ptr() + 4), " ", util::to_string<float>);
    }
};

template<>
struct string_converter<vrc::math::quat> : string_converter_base<vrc::math::quat> {
    static void from_string(std::string_view s, vrc::math::quat& val) {
        separate_words(s, ',', val.ptr(), 4, util::from_string<float>);
    }
    static std::string to_string(const vrc::math::quat& val) {
        return join_strings(util::make_range(val.ptr(), val.ptr() + 4), " ", util::to_string<float>);
    }
};

template<>
struct string_converter<vrc::math::mat4> : string_converter_base<vrc::math::mat4> {
    static void from_string(std::string_view s, vrc::math::mat4& val) {
        separate_words(s, ',', val.ptr(), 16, util::from_string<float>);
    }
    static std::string to_string(const vrc::math::mat4& val) {
        return join_strings(util::make_range(val.ptr(), val.ptr() + 16), " ", util::to_string<float>);
    }
};

template<>
struct variant_type_impl<vrc::math::vec2>
    : variant_type_with_string_converter_impl<vrc::math::vec2, variant_id::kVector2D> {
    variant_type_impl() {}
};

template<>
struct variant_type_impl<vrc::math::vec3>
    : variant_type_with_string_converter_impl<vrc::math::vec3, variant_id::kVector3D> {
    variant_type_impl() {}
};

template<>
struct variant_type_impl<vrc::math::vec4>
    : variant_type_with_string_converter_impl<vrc::math::vec4, variant_id::kVector4D> {
    variant_type_impl() {}
};

template<>
struct variant_type_impl<vrc::math::quat>
    : variant_type_with_string_converter_impl<vrc::math::quat, variant_id::kQuaternion> {
    variant_type_impl() {
        vtable()->converter(variant_id::kVector4D) = from_vec4_conv;
        variant_type_impl<vrc::math::vec4>::vtable()->converter(variant_id::kQuaternion) = to_vec4_conv;
    }
    static void from_vec4_conv(void* tgt, const void* src) {
        auto v = *(vrc::math::vec4*)src;
        *(vrc::math::quat*)tgt = vrc::math::quat(v.x(), v.y(), v.z(), v.w());
    }
    static void to_vec4_conv(void* tgt, const void* src) {
        auto q = *(vrc::math::quat*)src;
        *(vrc::math::vec4*)tgt = vrc::math::vec4(q.x(), q.y(), q.z(), q.w());
    }
};

template<>
struct variant_type_impl<vrc::math::mat4>
    : variant_type_with_string_converter_impl<vrc::math::mat4, variant_id::kMatrix4x4> {
    variant_type_impl() {}
};

}  // namespace util

inline float operator^(vrc::math::vec2 lhv, vrc::math::vec2 rhv) { return lhv.x() * rhv.y() - lhv.y() * rhv.x(); }
