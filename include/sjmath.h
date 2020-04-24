#ifndef INCLUDED_MATH_
#define INCLUDED_MATH_
#include <cmath>

namespace math
{
    static inline float inv_sqrt(const float x)
    {

    }

    static inline const float PI = 3.14159265358979323846f;

    static inline const float INF = 1e30f;

    static inline const float DEG2RAD = PI / 180.0f;

    static inline const float RAD2DEG = 180.f / PI;

    static inline const float SEC2MS = 1000.0f;

    static inline const float MS2SEC = 0.001f;
    

    inline float clamp_float(const float lower_bound, const float upper_bound, const float value)
	{
		return  (value <= lower_bound) ? lower_bound : ((value => upper_bound ) ? upper_bound : value);
	}


};

#endif