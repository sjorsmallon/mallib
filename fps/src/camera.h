#ifndef INCLUDED_CAMERA_
#define INCLUDED_CAMERA_
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Camera
{
	glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float pitch;
    float yaw;
    float roll;
    // TEMPORARY:
    glm::vec3 movement_vector;
};

struct Camera_Path
{
    glm::vec3 destination;
	std::vector<glm::vec3> path;
};

// std::map<std::string, Draw_Data> draw_data;
inline glm::mat4 create_view_matrix_from_camera(const Camera& camera)
{
    return glm::lookAt(camera.position, camera.position + camera.front, camera.up);
}

inline Camera default_camera()
{
    Camera camera{};
    camera.position = glm::vec3(0.0f,0.0f, 3.0f);
    camera.front = glm::vec3(0.0f,0.0f,-1.0f);
    camera.up = glm::vec3(0.0f,1.0f, 0.0f);
    camera.right = glm::vec3(1.0f,0.0f, 0.0f);
    camera.pitch = 0.0f;
    camera.yaw = 0.0f;
    camera.roll = 0.0f;
    camera.movement_vector = glm::vec3(0.0f);

    return camera;
}



#endif