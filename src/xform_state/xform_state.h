#ifndef INCLUDE_XFORM_STATE_
#define INCLUDE_XFORM_STATE_
#include "../quaternion/quaternion.h"
#include "../vec3/vec3.h"

struct Xform_State
{
    Quaternion orientation;
    Vec3 position;
    float scale;
};

#endif