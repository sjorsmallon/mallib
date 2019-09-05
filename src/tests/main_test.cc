#include "sound_device/sound_device.h"
#include "renderer/renderer.h"
#include "mgl/mgl.h"

Sound_Device sound_device; // cannot guarantee that this is in a valid state?
Window window; // same?
Config config; // same?


int main()
{
	Sound_Device* sound_device = msound::platform_create_sound_device();
	auto sound_device = std::make_unique<Sound_Device>;
	Window* window =  mdraw::platform_create_window();
	mdraw::init_opengl(window);

	
 
}