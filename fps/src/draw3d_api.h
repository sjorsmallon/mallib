#ifndef INCLUDED_DRAW3D_API_
#define INCLUDED_DRAW3D_API_
#include <glm/mat4.hpp>
#include <glm/glm.hpp>




// inline void draw_arrow(glm::vec3 start_in, glm::vec3 end_in, glm::vec3 color_in, float thickness_in = 1.0f)
// {
// 	// https://math.stackexchange.com/questions/2563909/find-points-on-a-plane

// 	glm::vec3 normal = glm::normalize(end - start_in);
// 	float distance  = glm::distance(end_in, start_in);

// 	// plane equation:
// 	// (Ax + By + Cz + D = 0)
// 	// new point: (x + 1, y, z + u)
// 	// new point:(x, y + 1, z+ v)

// 	float u = -start_in.x / start_in.z;
// 	float v = -start_in.y / start_in.z;

// 	glm::vec3 new_point{start_in.x + thickness_in, start_in.y, start_in.z + u};
// 	glm::vec3 perpendicular = glm::normalize(new_point - start_in);

// 	std::vector<float> triangle_test{
// 		start_in.x,
// 		start_in.y,
// 		start_in.z,
// 		0.0f,
// 		0.0f,
// 		1.0f,
// 		color_in.x,
// 		color_in.y,
// 		color_in.z,
// 		end_in.x,
// 		end_in.y,
// 		end_in.z,
// 		0.0f,
// 		0.0f,
// 		1.0f,
// 		color_in.x,
// 		color_in.y,
// 		color_in.z
// 	};

// 	g_debug_draw_data.insert(std::end(g_debug_draw_data), std::begin(triangle_test), std::end(triangle_test));
// };





#endif

