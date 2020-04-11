#include "camera.h"

namespace mcam
{
    Camera default_camera()
    {
        Camera default;
        default.pitch = 0;
        default.orbit = 0;
        default.dolly = 10.0f; // distance from the subject

        default.focal_length = 3.0f; // perspective foreshortening?
        default.near_clip_plane = 0.1f; // closest you can be to the camera and still be seen.
        default.far_clip_plane = 1000.0f; // furthest away you can be from the camera 

        result.offset = {0, 0, 0};
        result.shift = {0, 0, 0}; // amount to drop the camera down from the center of subject


        return default;
    };

}