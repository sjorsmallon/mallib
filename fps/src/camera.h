#ifndef INCLUDED_CAMERA_
#define INCLUDED_CAMERA_
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

struct Camera
{
	glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float pitch;
    float yaw;
    float roll;
};

struct Video_Camera : public Camera
{
	glm::vec3 destination;
	std::vector<glm::vec3> path;
};




#endif