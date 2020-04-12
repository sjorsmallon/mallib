#include <camera.h>


const cam::Camera& default_camera()
{
	static cam::Camera default_camera;
	return default_camera;
};
