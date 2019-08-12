#ifndef INCLUDED_MATH_
#define INCLUDED_MATH_
#include <cmath>
#define IEEE_FLT_MANTISSA_BITS  23


class Math
{
    public:
        static void Init();

        static float invSqrt(const float x);

        static const float PI;

        static const float DEG2RAD;

        static const float RAD2DEG;

        static const float SEC2MS;

        static const float MS2SEC;

        static const float INF;

        static const float FLT_SMALLEST_NON_DENORMAL;

};

inline float Math::invSqrt(const float x)
{
    return ( x > FLT_SMALLEST_NON_DENORMAL ) ? sqrtf(1.0f/ x) : INF;
}

#endif