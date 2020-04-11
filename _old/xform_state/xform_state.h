#ifndef INCLUDE_XFORM_STATE_
#define INCLUDE_XFORM_STATE_

#include "../vec/vec.h" // dot, cross 
#include "../vec3/vec3.h"
#include "../vec4/vec4.h"
#include <fmt/core.h>
#include "../mat4/mat4.h"
#include <cmath> // sin,cos 

struct Xform_State
{
	Vec3 position;
	Vec4 q_orientation;
	float scale;
};


inline Vec4 q_rotation(const Vec3& from, const Vec3& to)
{
	Vec4 quaternion = {};
	quaternion.w = 1.0f + vec::dot(to, from); // The order does not matter here right?
	
	Vec3 cross_product = vec::cross(from, to);

	quaternion.x = cross_product.x;
	quaternion.y = cross_product.y;
	quaternion.z = cross_product.z;

	normalize(quaternion);
	return quaternion;
}

// angle + axis, axis must be normalized. 
//This function does NOT! normalize the axis.
inline Vec4 q_rotation(float angle_in_rad, const Vec3& axis)
{
	Vec4 quaternion = {};
	float sina = std::sin(angle_in_rad / 2.0f);
	quaternion.w = std::cos(angle_in_rad / 2.0f);

	quaternion.x = sina * axis.x;
	quaternion.y = sina * axis.y;
	quaternion.z = sina * axis.z;

	return quaternion;
}

// Quaternion-Quaternion product
// non-optimized
inline Vec4 q_product(const Vec4& lhs, const Vec4& rhs)
{
	// Quaternion formulas:
	// r.a = p.a * q.a - Dot(p.v,q.v);
	// r.v = p.a * q.v + q.a * p.v + Cross(p.v,q.v);
	Vec4 product = {};

	product.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;

	product.x = lhs.w * rhs.x + rhs.w * lhs.x + lhs.y * rhs.z - lhs.z * rhs.y;
	
	product.y = lhs.w * rhs.y + rhs.w * lhs.y + lhs.z * rhs.x - lhs.x * rhs.z;
	
	product.z = lhs.w * rhs.z + rhs.w * lhs.z + lhs.x * rhs.y - lhs.y * rhs.x;

	return product;
}

/// Q (x,0) Q*
// rotate vec3 by quad
// non-optimized
inline Vec3 rotate_by_quat(const Vec3& lhs, const Vec4& quat_p)
{
	// q = P (lhs,0)
	Vec4 quat_q = {}; 
	quat_q.x = quat_p.w * lhs.x + quat_p.y * lhs.z - quat_p.z * lhs.y;
	quat_q.y = quat_p.w * lhs.y + quat_p.z * lhs.x - quat_p.x * lhs.z;
	quat_q.z = quat_p.w * lhs.z + quat_p.x * lhs.y - quat_p.y * lhs.x;
	
	quat_q.w = - quat_p.x * lhs.x - quat_p.y * lhs.y - quat_p.z * lhs.z;

	// r = q P*
	Vec3 result = {};
	result.x = quat_q.w * -quat_p.x + quat_p.w * quat_q.x - quat_q.y * quat_p.z + quat_q.z * quat_p.y;
	result.y = quat_q.w * -quat_p.y + quat_p.w * quat_q.y - quat_q.z * quat_p.x + quat_q.x * quat_p.z;
	result.z = quat_q.w * -quat_p.z + quat_p.w * quat_q.z - quat_q.x * quat_p.y + quat_q.y * quat_p.x;

	return result;
}

// Conjugate
inline Vec4 conjugate(const Vec4& quaternion) 
{
	return {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w};
}


//@Refactor: naming consistency?
// rotate a quat by another
inline Vec4 rotate_by_quat(const Vec4& quat_lhs, const Vec4& quat_rhs)
{
	// should unwrap this for efficiency
	//return (*this) * r * (*this).conjugate();

	Vec4 result = q_product(quat_lhs, quat_rhs);
	result = q_product(result, conjugate(quat_lhs));
	
	return result; 
}


// Length Squared
// inline float Vec4::lengthsqrd() const
// {
// 	return Sqr(a) + Sqr(v0) + Sqr(v1) + Sqr(v2);
// }

// Length
// inline float Quaternion::length() const
// {
// 	return sqrt( lengthsqrd() );
// }

#endif