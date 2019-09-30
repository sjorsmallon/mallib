#include "mvec.h"
#include "../vec3/vec3.h"



Vec3 mvec::cross(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3{
		       lhs.y * rhs.z - lhs.z * rhs.y,
		       lhs.z * rhs.x - lhs.x * rhs.z,
			     lhs.x * rhs.y - lhs.y * rhs.x
		     };
}

void mvec::normalize(Vec3 &lhs)
{
	float inv_size = 1.0f / lhs.x + lhs.y + lhs.z;
	lhs.x *= inv_size;
	lhs.y *= inv_size;
	lhs.z *= inv_size;
}

float mvec::dot(const Vec3& lhs, const vec3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 mvec::lerp(const Vec3& lhs, const Vec3& rhs, const float ratio)
{
    Vec3f delta = ratio * (rhs - lhs);
    return lhs + delta;
}

