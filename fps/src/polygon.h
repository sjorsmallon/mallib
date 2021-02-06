#ifndef INCLUDED_POLYGON_
#define INCLUDED_POLYGON_
#include <glm/vec3.hpp>
#include <vector>
#include "AABB.h"

struct Face
{

};

struct Polygon
{
	glm::vec3 min; // minimum in all axes ( not any particular vertex)
	glm::vec3 max; // maximum in all axes ( not any particular vertex)
	std::vector<glm::vec3> vertices; // positions; 
	std::vector<Face> faces; // non self-polygon-intersecting faces?
	// Xform_State xform_state;
};

inline Polygon polygon_from_AABB(const AABB& aabb)
{
	// max is front top right,
	// min is back bot left
	// we have 6 faces.
	float min_x = aabb.min.x;
	float min_y = aabb.min.y;
	float min_z = aabb.min.z;

	float max_x = aabb.max.x;
	float max_y = aabb.max.y;
	float max_z = aabb.max.z;

	glm::vec3 front_top_right = aabb.max;
	glm::vec3 front_bot_left(min_x, min_y, max_z);
	glm::vec3 front_bot_right(max_x, min_y, max_z); 
	glm::vec3 front_top_left(min_x, max_y, max_z);

	glm::vec3 back_bot_left = aabb.min;
	glm::vec3 back_top_right(max_x, max_y, min_z);
	glm::vec3 back_top_left( min_x, max_y, min_z);
	glm::vec3 back_bot_right(max_x, min_y, min_z);

	glm::vec3 front_normal  = glm::vec3(0.f, 0.f,  1.f);
	glm::vec3 back_normal   = glm::vec3(0.f, 0.f, -1.f);
 	
	glm::vec3 left_normal   = glm::vec3(-1.f, 0.f, 0.f);
	glm::vec3 right_normal  = glm::vec3( 1.f, 0.f, 0.f);

	glm::vec3 top_normal    = glm::vec3(0.f,  1.f, 0.f);
	glm::vec3 bottom_normal = glm::vec3(0.f, -1.f, 0.f);

	Polygon polygon{};
	polygon.min = aabb.min;
	polygon.max = aabb.max;
	polygon.vertices.insert(polygon.vertices.end(), {
		front_top_right,
		front_bot_left, 
		front_bot_right, 
		front_top_left, 
		back_bot_left, 
		back_top_right, 
		back_top_left, 
		back_bot_right});

	return std::move(polygon);
}


#endif // INCLUDED_POLYGON
