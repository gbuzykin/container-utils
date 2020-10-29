#include "core/math.h"

using namespace vrc;
using namespace vrc::math;

static util::variant_type_impl<vec2> g_vec2_variant_type;
static util::variant_type_impl<vec3> g_vec3_variant_type;
static util::variant_type_impl<vec4> g_vec4_variant_type;
static util::variant_type_impl<quat> g_quat_variant_type;
static util::variant_type_impl<mat4> g_mat4_variant_type;
