#ifndef INCLUDED_DRAW_3D_
#define INCLUDED_DRAW_3D_

#include <vector>
#include "render_system.h"
#include <glad/glad.h>

const float DEFAULT_LINE_WIDTH = 2.f;

struct Draw_Style
{
    bool wireframe;
    float line_width;

    glm::vec4 color;
    glm::vec4 highlight_color;
};

// Debug 
inline void draw_AABB(glm::vec3 min, glm::vec3 max, Draw_Style style)
{
    // Optimizing Triangle Strips for Fast Rendering
    // https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
    // unfold triangle mesh from min and max.
    // 4 3 7
    // 8 5 3
    // 1 4 2
    // 7 6 5
    // 2 1 
    // draw a triangle_strip : 
    // point 1 is max,
    // point 8 is min.

    // glm::vec3 p1 = max; // front top right
    // glm::vec3 p2 = glm::vec3(min.x, max.y, max.z); // front top left
    // glm::vec3 p3 = glm::vec3(max.x, min.y, max.z); // front bot right
    // glm::vec3 p4 = glm::vec3(min.x, min.y, max.z); // front bot left

    // glm::vec3 p5 = glm::vec3(max.x, max.y, min.z); // back top right
    // glm::vec3 p6 = glm::vec3(min.x, max.y, min.z); // back top left
    // glm::vec3 p7 = glm::vec3(max.x, min.y, min.z); // back bot right
    // glm::vec3 p8 = min; // back bot left
    // std::vector<glm::vec3> positions{
    //     p4, p3, p7,
    //     p8, p5, p3,
    //     p1, p4, p2,
    //     p7, p6, p5,
    //     p2, p1};


    glm::vec3 p0 = glm::vec3(min.x, min.y, max.z); // front bot left
    glm::vec3 p1 = glm::vec3(max.x, min.y, max.z); // front bot right
    glm::vec3 p2 = glm::vec3(min.x, max.y, max.z); // front top left
    glm::vec3 p3 = max;
    glm::vec3 p4 = min;
    glm::vec3 p5 = glm::vec3(max.x, min.y, min.z); // back bot right
    glm::vec3 p6 = glm::vec3(min.x, max.y, min.z); // back top left
    glm::vec3 p7 = glm::vec3(max.x, max.y, min.z); // back top right;

    std::vector<glm::vec3> positions{
        p7, p6, p3, p2, p0, p6, p4, 
        p7, p5, p3, p1, p0, p5, p4
    };
    
    Draw_Request draw_request{};
    draw_request.primitive = GL_TRIANGLE_STRIP;
    draw_request.color = style.color;
    draw_request.highlight_color = style.highlight_color;

    draw_request.line_width = style.line_width;
    draw_request.wireframe = style.wireframe;
    //@Memory(Sjors): copy?
    draw_request.vertices = std::move(positions);

    // submit to be drawn as triangle strip.
    submit_debug_draw_request(draw_request);
}

//@Note(Sjors): for draw_polyline: wireframe is always handled as if it were true.
// Debug 
inline void draw_polyline(std::vector<glm::vec3>& points, Draw_Style style)
{
    Draw_Request draw_request{};
    draw_request.primitive = GL_LINE_STRIP;
    draw_request.color = style.color;

    draw_request.highlight_color = style.color;
    draw_request.wireframe = true;
    
    draw_request.line_width = style.line_width;
    //@memory(Sjors): copy?
    draw_request.vertices = points;

    // submit to be drawn as line strip.
    submit_debug_draw_request(draw_request);
}

#endif

