#ifndef INCLUDED_LIGHT_
#define INCLUDED_LIGHT_
#include <glm/vec4.hpp>

using bool32 = uint32_t;

//@VOLATILE: if the layout of this struct changes, all shaders will need to be changed as well.
struct Light
{
	glm::vec4 position;
	glm::vec4 color;
	bool32 on;
	float linear;
	float quadratic;
	float radius;
};


#endif