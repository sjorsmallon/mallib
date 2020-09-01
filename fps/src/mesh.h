#ifndef INCLUDED_MESH_
#define INCLUDED_MESH_
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_coords;
};

struct Mesh
{
	std::vector<glm::vec3> data;
};


#endif