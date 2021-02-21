#include "mallet.h"
#include <cstdio>
#include <vector>
#include <string>
#include "plane.h"
#include "AABB.h"

#include "bsp.h" // build_leaf_storing_bsp
#include "draw3d.h"

namespace
{
	Map f_default_map;

	// converts read AABBs to polygons and inserts them.
	// expects a file_reader at the FIRST nonempty line.
	// e.g.:
	// :/ AAABB
	// { * <--- here
	// an AABB section is :
	// {
	//   min f f f
	//   max f f f
	// }
	void parse_AABB_section(Map& map, File_Reader& file_reader, std::string& line)
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
		for (auto&& mallet_AABB: AABBs)
		{
			AABB aabb{};
			aabb.min = mallet_AABB.min;
			aabb.max = mallet_AABB.max;

			map.polygons.emplace_back(polygon_from_AABB(aabb));

			map.AABBs.push_back(aabb);
		}
		std::vector<Polygon>& polygons = map.polygons;
		BSP bsp = build_leaf_storing_bsp(polygons);
	}
}

//@Temporary
void draw_AABBs()
{
	Draw_Style draw_style{};

	draw_style.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	// draw_style.wireframe = true;
	for (size_t idx = 0; idx != 3; ++idx)
	{
		auto& aabb = f_default_map.AABBs[idx];
		draw_AABB(aabb.min, aabb.max, draw_style);
	}

	// draw floor in different color
	draw_style.color = glm::vec4(0.9f, 0.0f ,0.6f, 1.0f);
	// draw_style.wireframe = true;
	draw_style.line_width = 2.0f;

	draw_AABB(f_default_map.AABBs[3].min, f_default_map.AABBs[3].max, draw_style);
}


Map& get_default_map()
{
	return f_default_map;
}


void load_map_from_file(const char* file_path)
{	
	File_Reader file_reader;
	read_file(file_reader, file_path);

	Map& destination_map = f_default_map;

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
				parse_AABB_section(destination_map, file_reader, line);
			}
		}
	}
}

