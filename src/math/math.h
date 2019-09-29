#ifndef INCLUDED_MATH_
#define INCLUDED_MATH_
#include <cmath>

namespace math
{
    static float invSqrt(const float x);

    static inline const float PI = 3.14159265358979323846f;

    static inline const float INF = 1e30f;

    static inline const float DEG2RAD = PI / 180.0f;

    static inline const float RAD2DEG = 180.f / PI;

    static inline const float SEC2MS = 1000.0f;

    static inline const float MS2SEC = 0.001f;

    
};

inline float Math::invSqrt(const float x)
{
    return ( x > FLT_SMALLEST_NON_DENORMAL ) ? sqrtf(1.0f/ x) : INF;
}

#endif