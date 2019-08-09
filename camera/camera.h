#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA

struct Camera
{
    Vec3 offset;
    Vec3 shift;

    float pitch;
    float orbit;
    float dolly;

    float focal_length;
    float near_clip_plane;
    float far_clip_plane;
}

namespace mcam
{
    Camera default_camera();
}

#endif