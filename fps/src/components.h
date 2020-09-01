#ifndef INCLUDED_COMPONENTS_
#define INCLUDED_COMPONENTS_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


struct player_controlled{};

// mesh?
struct Xform_State
{
	glm::vec3 position;
	glm::vec4 q_rotation;
	float scale;
};

struct box3D
{
	glm::vec3 front_bot_left;
	glm::vec3 back_top_right;
	glm::vec3 center;
};

struct velocity
{
	float vx;
	float vy;
	float vz;
};

struct color_rgb
{
	float r;
	float g;
	float b;
};

struct renderable
{
	// mesh
};


inline glm::vec3 operator*(const glm::vec3& lhs, const velocity& rhs)
{
	glm::vec3 actual_rhs(rhs.vx, rhs.vy, rhs.vz);
	return lhs * actual_rhs;	
}

// components.
using position = glm::vec3;
using OOBB = box3D;

#endif;
