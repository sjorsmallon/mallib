#ifndef INCLUDED_AABB_
#define INCLUDED_AABB_

#include <glm/vec3.hpp>

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
};

#endif // INCLUDED_AABB_