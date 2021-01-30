#ifndef INCLUDED_MALLET_
#define INCLUDED_MALLET_
#include "file.h"
#include <glm/vec3.hpp>
#include <cstdio>


const int MAP_DEFAULT_X = 8092;
const int MAP_DEFAULT_Y = 8092;
const int MAP_DEFAULT_Z = 8092;
const int MAP_DEFAULT_GRID_X = 128;
const int MAP_DEFAULT_GRID_Y = 128;

// struct Slab
// {
// 	glm::vec3 min;
// 	glm::vec3 max;
// 	glm::vec3 center;
// };

struct Mallet_AABB
{
	glm::vec3 min;
	glm::vec3 max;
};

struct Face
{

};

struct Polygon
{
	std::vector<glm::vec3> vertices; // positions; 
	std::vector<Face> faces; // non self-polygon-intersecting faces?
	// Xform_State xform_state;
};

//@Fixme(Sjors): disallow copy etc etc etc.
struct Map
{
	std::vector<Polygon> polygons;
	glm::ivec3 dimensions{MAP_DEFAULT_X,MAP_DEFAULT_Y,MAP_DEFAULT_Z};
};


// expects a file_reader at the FIRST nonempty line
// e.g.:
// :/ AAABB
// { * <--- here
inline void parse_AABB_section(File_Reader& file_reader, std::string& line)
{
	std::vector<Mallet_AABB> AABBs;
	Mallet_AABB* current_AABB = nullptr;
	bool building_AABB = false;

	while(true)
	{
		// opening brace ('{')
		{
			if (line[0] == '{')
			{
				if (building_AABB)
				{
					logr::report_error("[parse_AABB_section] found '{{' while already building AABB. no nested AABB allowed!\n");
				}

				building_AABB = true;
				AABBs.push_back({});
				current_AABB = &AABBs.back();
			}
		}

		// min %f %f %f
		{
			bool result = maybe_get_nonempty_line(file_reader, line);
			if (!result) logr::report_error("[parse_AABB_section]: while searching for 'min', found EOF. no closing '}}' found.\n");

			eat_leading_whitespace(line);

			sscanf(line.c_str(),"min %f %f %f", &current_AABB->min.x, &current_AABB->min.y, &current_AABB->min.z);
			
		}
		
		// max %f %f %f
		{
			bool result = maybe_get_nonempty_line(file_reader, line);
			if (!result) logr::report_error("[parse_AABB_section]: While searching for 'max', found EOF. no closing '}}' found.\n");

			eat_leading_whitespace(line);
			sscanf(line.c_str(),"max %f %f %f", &current_AABB->max.x, &current_AABB->max.y, &current_AABB->max.z);
		}

		// closing brace: '}'
		{
			bool result = maybe_get_nonempty_line(file_reader, line);
			if (!result) logr::report_error("[parse_AABB_section]: no closing '}}' found.\n");
	 
			if (line[0] == '}')
			{
				logr::report("[parse_AABB_section]: succesfully parsed AABB.\n");
				building_AABB = false;
			}
		}

		// get the next line.
		bool result = maybe_get_nonempty_line(file_reader, line);
		if (!result)
		{
			if (building_AABB) logr::report_error("[parse_AABB_section] done parsing. breaking the while loop.\n");
			break;
		}
	}

	// we have a vector of mallet_AABB. post-process them to polygons.
	for (auto&& AABB: AABBs)
	{
		logr::report("AABB.min: {}\n", AABB.min);
		logr::report("AABB.max: {}\n", AABB.max);		
		// max is front top right,
		// min is back bot left
		// we have 6 faces.
		float min_x = AABB.min.x;
		float min_y = AABB.min.y;
		float min_z = AABB.min.z;

		float max_x = AABB.max.x;
		float max_y = AABB.max.y;
		float max_z = AABB.max.z;

		glm::vec3 front_top_right = AABB.max;
		glm::vec3 front_bot_left(min_x, min_y, max_z);
		glm::vec3 front_bot_right(max_x, min_y, max_z); 
		glm::vec3 front_top_left(min_x, max_y, max_z);

		glm::vec3 back_bot_left = AABB.min;
		glm::vec3 back_top_right(max_x, max_y, min_z);
		glm::vec3 back_top_left( min_x, max_y, min_z);
		glm::vec3 back_bot_right(max_x, min_y, min_z);

		glm::vec3 front_normal  = glm::vec3(0.f, 0.f,  1.f);
		glm::vec3 back_normal   = glm::vec3(0.f, 0.f, -1.f);
	 	
		glm::vec3 left_normal   = glm::vec3(-1.f, 0.f, 0.f);
		glm::vec3 right_normal  = glm::vec3( 1.f, 0.f, 0.f);

		glm::vec3 top_normal    = glm::vec3(0.f,  1.f, 0.f);
		glm::vec3 bottom_normal = glm::vec3(0.f, -1.f, 0.f);
	}
}




// map contains static geometry.
inline void load_map_from_file(const char* file_path)
{	
	File_Reader file_reader;
	read_file(file_reader, file_path);

	std::string line{};

	while (maybe_get_nonempty_line(file_reader, line))
	{
		// case: '#': comment
		if (line[0] == '#') continue;

		// sections
		if (line[0] == ':' && line[1] == '/')
		{
			// AABB section
			if (line.find("AABB") != std::string::npos) // found section line with AABB present.
			{
				bool success = maybe_get_nonempty_line(file_reader, line);
				if (!success) logr::report_error("[load_map_from_file] no valid AABB declarations found.\n");
				parse_AABB_section(file_reader, line);

			}
		}
	}
}


#endif