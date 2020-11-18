#ifndef INCLUDED_DRAW3D_API_
#define INCLUDED_DRAW3D_API_
#include <glm/mat4.hpp>
#include <glm/glm.hpp>



 // @temporary:
void put_arrow(const glm::vec3 start_in, const glm::vec3 end_in, const glm::vec3 color_in, const float thickness_in = 1.0f)
{
    // https://math.stackexchange.com/questions/2563909/find-points-on-a-plane

    const glm::vec3 normal = glm::normalize(end_in - start_in);
    const float distance  = glm::distance(end_in, start_in);

    const glm::vec3 distance_per_axis = end_in - start_in;

    const float epsilon = 0.0001f;

    // plane equation:
    // (Ax + By + Cz + D = 0)
    // new point: (x + 1, y, z + u)
    // new point: (x, y + 1, z + v)
    // assuming C is not 0:
    // μ= −A/C 
    // ν=−B/C

    const float u = -start_in.x / start_in.z;
    const float v = -start_in.y / start_in.z;

    const glm::vec3 first_point{start_in.x + thickness_in, start_in.y, start_in.z + u};
    const glm::vec3 second_point{start_in.x, start_in.y + thickness_in, start_in.z + v};

    glm::vec3 second_axis = glm::normalize(first_point - start_in);
    // if z is 0: second_axis just becomes z.

    if (distance_per_axis.z  < epsilon) second_axis = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 third_axis = glm::cross(normal, second_axis);

    glm::vec3 first_plane_point = start_in + third_axis;
    glm::vec3 second_plane_point = start_in + second_axis;


    // slab
    // stop at 80% of head
    const glm::vec3 slab_end = start_in + (0.8f * distance) * normal; 
    glm::vec3 back_top_left  = start_in + (-0.5f * second_axis) + (0.5f * third_axis);
    glm::vec3 back_top_right = start_in + (0.5f * second_axis) +  (0.5f * third_axis);
    glm::vec3 back_bot_left  = start_in + (-0.5f * second_axis) + (-0.5f * third_axis);
    glm::vec3 back_bot_right = start_in + (0.5f * second_axis) +  (-0.5f * third_axis);


    glm::vec3 front_top_left  =  slab_end +  (-0.5f * second_axis) + (0.5f * third_axis);
    glm::vec3 front_top_right =  slab_end + (0.5f * second_axis) + (0.5f * third_axis);
    glm::vec3 front_bot_left  =  slab_end + (-0.5f * second_axis) + (-0.5f * third_axis);
    glm::vec3 front_bot_right =  slab_end + (0.5f * second_axis) + (-0.5f * third_axis);

    // pyramid on top:
    glm::vec3 head_back_top_left  = slab_end + (-0.8f * second_axis) + (0.8f * third_axis);
    glm::vec3 head_back_top_right = slab_end + (0.8f * second_axis) + (0.8f * third_axis);
    glm::vec3 head_back_bot_left  = slab_end + (-0.8f * second_axis) + (-0.8f * third_axis);
    glm::vec3 head_back_bot_right = slab_end + (0.8f * second_axis) + (-0.8f * third_axis);
    glm::vec3 head_top = end_in;


    std::vector<float> buffer
    {
        // back face
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        // front face
        front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        // left
        front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        // right
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
         // bottom
        front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        // top
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
        back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
        back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
        front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z            
    };

    std::vector<float> head_buffer{
        // back face
        head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        // sides of the pyramid: left:
        head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            //
        head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        // rihgt:
        head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            //
        head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        // bot:
        head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,
        head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
        head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
        // top:
        head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,
        head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,  
        head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z
    };
  
    g_debug_draw_data.insert(std::end(g_debug_draw_data), std::begin(buffer), std::end(buffer));
    g_debug_draw_data.insert(std::end(g_debug_draw_data), std::begin(head_buffer), std::end(head_buffer));
};


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

