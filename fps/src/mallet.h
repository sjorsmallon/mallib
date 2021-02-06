#ifndef INCLUDED_MALLET_
#define INCLUDED_MALLET_

#include <glm/vec3.hpp>

#include "file.h"
#include "polygon.h"

const int MAP_DEFAULT_DIM_X = 8092;
const int MAP_DEFAULT_DIM_Y = 8092;
const int MAP_DEFAULT_DIM_Z = 8092;
const int MAP_DEFAULT_GRID_X = 128;
const int MAP_DEFAULT_GRID_Y = 128;

struct Mallet_AABB
{
	glm::vec3 min;
	glm::vec3 max;
};


//@Fixme(Sjors): disallow copy etc etc etc.
struct Map
{
	std::vector<Polygon> polygons;
	glm::ivec3 dimensions{MAP_DEFAULT_DIM_X,MAP_DEFAULT_DIM_Y,MAP_DEFAULT_DIM_Z};
	std::vector<AABB> AABBs;

};

// map contains static geometry.
void load_map_from_file(const char* file_path);


//@Temporary:
void draw_AABBs();


#endif