#ifndef _INCLUDED_MATH_
#define _INCLUDED_MATH_

inline float clamp(const float in, const float lower_bound, const float upper_bound)
{
	return (in < lower_bound ? lower_bound : (in > upper_bound? upper_bound : in));
}


#endif