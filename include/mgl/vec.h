#ifndef INCLUDED_MVEC_
#define INCLUDED_MVEC_
#include <mgl/vec4.h>
#include <mgl/vec3.h>
#include <mgl/vec2.h>
#include <fmt/format.h>
#include <fmt/core.h>

//@Performance: is it cheaper to pass by value or by reference?
namespace fmt {
   template <>
    struct formatter<mgl::vec4> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::vec4 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f} {:.3f} {:.3f}\n",
            lhs[0], lhs[1], lhs[2], lhs[3]);
      }
    };

    template <>
    struct formatter<mgl::vec3> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::vec3 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f} {:.3f}\n",
            lhs[0], lhs[1], lhs[2]);
      }
    };

    template <>
    struct formatter<mgl::vec2> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::vec2 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f}\n",
            lhs[0], lhs[1]);
      }
    };

    template <>
    struct formatter<mgl::ivec2> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::ivec2 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f}\n",
            lhs[0], lhs[1]);
      }
    };

}

namespace mgl
{
  // vec3

    float dot(const vec3& lhs, const vec3& rhs);
    vec3 cross(const vec3& lhs, const vec3& rhs);
    vec3 lerp(const vec3& lhs, const vec3& rhs, const float ratio);
    // void normalize(vec3& lhs);
    // void normalize(vec4& lhs);
    vec3 normalize(const mgl::vec3& lhs);
    vec4 normalize(const mgl::vec4& lhs);
    float* value_ptr(vec3& lhs);
 	//vec4
    float dot(const vec4& lhs, const vec4& rhs);
    float* value_ptr(vec4& lhs);

  //vec2
    float* value_ptr(vec2& lhs);

  //vec2i
    int32_t* value_ptr(ivec2& lhs);

    vec4 make_vec4(const vec3& lhs, float w);
    vec3 make_vec3(const vec4& rhs);
};

inline mgl::vec3 mgl::make_vec3(const vec4& rhs)
{
  return vec3{rhs.x, rhs.y, rhs.z};
}

inline mgl::vec4 mgl::make_vec4(const vec3& lhs, float w)
{
  return vec4{lhs.x, lhs.y, lhs.z, w};
}

inline int32_t* mgl::value_ptr(mgl::ivec2& lhs)
{
  return &lhs.data[0];
}

inline float* mgl::value_ptr(mgl::vec2& lhs)
{
  return &lhs.data[0];
}

inline float* mgl::value_ptr(mgl::vec3& lhs)
{
  return &lhs.data[0];
}

inline float* mgl::value_ptr(mgl::vec4& lhs)
{
  return &lhs.data[0];
}

// mathy functions.

inline float mgl::dot(const mgl::vec3& lhs, const mgl::vec3& rhs)
{
    return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

inline float mgl::dot(const mgl::vec4& lhs, const mgl::vec4& rhs)
{
  return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
}

inline mgl::vec3 mgl::cross(const mgl::vec3& lhs, const mgl::vec3& rhs)
{
    mgl::vec3 result = {};

    result.x = lhs.y * rhs.z - lhs.z * rhs.y; //aybz − azby
    result.y = lhs.z * rhs.x - lhs.x * rhs.z; //azbx − axbz
    result.z = lhs.x * rhs.y - lhs.y * rhs.x; //axby − aybx

    return result;
}
inline mgl::vec3 mgl::lerp(const mgl::vec3& lhs, const mgl::vec3& rhs, const float t)
{
    //@Refactor: do we assume this is nonzero?
    return  (t * lhs + (1.0f - t) * rhs);
}

inline mgl::vec3 mgl::normalize(const mgl::vec3& lhs)
{
    vec3 result = lhs;
    //@FIXME: what if lhs is zero?
    // assert(lhs.x + lhs.y +lhs.z > 0)
    float one_over_sum =  1.0f / sqrt(powf(lhs.x,2.0f) + powf(lhs.y, 2.0f) + powf(lhs.z, 2.0f));
    
    result.x *= one_over_sum;
    result.y *= one_over_sum;
    result.z *= one_over_sum;
    return result;
}

inline mgl::vec4 mgl::normalize(const mgl::vec4& lhs)
{
  vec4 result = lhs;
  //@FIXME: what if lhs is zero?
    // assert(lhs.x + lhs.y +lhs.z > 0)
  
  float one_over_sum =  1.0f / sqrt(powf(lhs.x,2.0f) + powf(lhs.y, 2.0f) + powf(lhs.z, 2.0f) + powf(lhs.w, 2.0f));

  result.x *= one_over_sum;
  result.y *= one_over_sum;
  result.z *= one_over_sum;
  result.w *= one_over_sum;
  return result;
}


#endif