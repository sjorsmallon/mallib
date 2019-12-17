#ifndef INCLUDED_MVEC_
#define INCLUDED_MVEC_
#include "../vec4/vec4.h"
#include "../vec3/vec3.h"
#include "../vec2/vec2.h"
#include <fmt/format.h>
#include <fmt/core.h>
//@Performance: is it cheaper to pass by value or by reference?
namespace fmt {
   template <>
    struct formatter<Vec4> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Vec4 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f} {:.3f} {:.3f}\n",
            lhs[0], lhs[1], lhs[2], lhs[3]);
      }
    };

    template <>
    struct formatter<Vec3> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Vec3 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f} {:.3f}\n",
            lhs[0], lhs[1], lhs[2]);
      }
    };

    template <>
    struct formatter<Vec2> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Vec2 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f}\n",
            lhs[0], lhs[1]);
      }
    };

    template <>
    struct formatter<Vec2i> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Vec2i &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "{:.3f} {:.3f}\n",
            lhs[0], lhs[1]);
      }
    };

}


namespace vec
{
  // vec3

    float dot(const Vec3& lhs, const Vec3& rhs);
    Vec3 cross(const Vec3& lhs, const Vec3& rhs);
    Vec3 lerp(const Vec3& lhs, const Vec3& rhs, const float ratio);
    void normalize(Vec3& lhs);
    void normalize(Vec4& lhs);
 	//vec4
};

// mathy functions.

inline float vec::dot(const Vec3& lhs, const Vec3& rhs)
{
    return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

inline Vec3 vec::cross(const Vec3& lhs, const Vec3& rhs)
{
    Vec3 result = {};

    result.x = lhs.y * rhs.z - lhs.z * rhs.y; //aybz − azby
    result.y = lhs.z * rhs.x - lhs.x * rhs.z; //azbx − axbz
    result.z = lhs.x * rhs.y - lhs.y * rhs.x; //axby − aybx

    return result;
}
inline Vec3 vec::lerp(const Vec3& lhs, const Vec3& rhs, const float t)
{
    //@Refactor: do we assume this is nonzero?
    return  (t * lhs + (1.0f - t) * rhs);
}

inline void vec::normalize(Vec3& lhs)
{
    //@FIXME: what if lhs is zero?
    // assert(lhs.x + lhs.y +lhs.z > 0)
    float one_over_sum = 1.0f / (lhs.x + lhs.y + lhs.z);
    lhs.x *= one_over_sum;
    lhs.y *= one_over_sum;
    lhs.z *= one_over_sum;
}

inline void vec::normalize(Vec4& lhs)
{
  //@FIXME: what if lhs is zero?
    // assert(lhs.x + lhs.y +lhs.z > 0)
  float one_over_sum = 1.0f / (lhs.x + lhs.y + lhs.z + lhs.w);
  lhs.x *= one_over_sum;
  lhs.y *= one_over_sum;
  lhs.z *= one_over_sum;
  lhs.w *= one_over_sum;
}


#endif