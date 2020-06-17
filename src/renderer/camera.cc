#include <camera.h>


const cam::Camera& cam::default_camera()
{
	static cam::Camera default_camera;
	return default_camera;
};
