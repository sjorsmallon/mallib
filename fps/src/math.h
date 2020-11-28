#ifndef _INCLUDED_MATH_
#define _INCLUDED_MATH_

inline float clamp(const float in, const float lower_bound, const float upper_bound)
{
	return (in < lower_bound ? lower_bound : (in > upper_bound? upper_bound : in));
}


inline bool ray_intersects_sphere(const glm::vec3& ray_origin, const glm::vec3& ray_direction, const glm::vec3& sphere_center, const float radius)
{
	//ray_sphere intersection
	glm::vec3 oc = ray_origin - sphere_center;
	float a = glm::dot(ray_direction, ray_direction);
	float b = 2.0 * glm::dot(oc, ray_direction);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	return (discriminant > 0.0f);
	

	// if(discriminant < 0.0)
	// {
	// 	return -1.0;
	// }
	// else
	// {
	// 	float numerator = -b - sqrt(discriminant);
	// 	if (numerator > 0.0f) return numerator / (2.0f * a);
	// }
	// 	numerator = -b + sqrt(discriminant);
	// 	if numerator > 0.0 {
	// 	return numerator / (2.0 * a);
	// }
	// else
	// {
	// 	return -1;
	// }


}

// inline bool ray_intersects_cube(const glm::vec3& ray_origin, const glm::vec3& ray_direction, const Box_3D& bounds)
// {

// }




#endif