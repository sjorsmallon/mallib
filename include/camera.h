#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA
#include <vec3.h>


namespace cam
{
	enum class Control_Mode
	{
		CAM_ORBIT = 0,
		CAM_FPS,
		CAM_ISOMETRIC
	};

	struct Camera
	{
		Control_Mode control_mode = Control_Mode::CAM_ORBIT;
	    Vec3 position{0.0f,0.0f,0.0f};
	    Vec3 target{0.0f,0.0f, -1.0f};
	    Vec3 up{0.0f, 1.0f, 0.0f};
	    float near_clip_plane = 0.1f;
	    float far_clip_plane = 1000.0f;
	    float field_of_view = 90.0f;
		
		// orthographic projection does not require any other fields.    
	    // float pitch;
	    // float orbit;
	    // float dolly;
	    // float focal_length;

	};
	const Camera& default_camera();
}





#endif