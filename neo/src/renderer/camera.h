#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA
#include <mgl/vec3.h>


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
	    mgl::vec3 position{0.0f,0.0f,0.0f};
	    mgl::vec3 target{0.0f,0.0f, -1.0f};
	    mgl::vec3 up{0.0f, 1.0f, 0.0f};
	    float near_clip_plane = 0.1f;
	    float far_clip_plane = 1000.0f;
	    float field_of_view = 90.0f;
		
		// orthographic projection does not require any other fields.    
	    // float pitch;
	    // float orbit;
	    // float dolly;
	    // float focal_length;

	};
	static inline Camera default_camera()
	{
		Camera camera; 
		return camera;
	};


}





#endif