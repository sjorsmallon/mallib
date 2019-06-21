#ifndef INCLUDED_MVEC_
#define INCLUDED_MVEC_
#include "../vec3/vec3.h"

//@Performance: is it cheaper to pass by value or by reference?
namespace mvec
{
  // vec3
	void normalize(Vec3& vector);

 	float dot(const Vec3& lhs, const Vec3& rhs);

	Vec3 cross(const Vec3& lhs, const Vec3& rhs);
 	Vec3 lerp(const Vec3& lhs, const Vec3& rhs, const float ratio);
 	//vec4

};

#endif