#include "vec3.h"
// helper functions

Vec3::Vec3()
//:
{}

Vec3::Vec3(float x, float y, float z)
:
    this->x(x),
    this->y(y),
    this->z(z)
{}

Vec3::Vec3(float x)
:
    this->x(x),
    this->y(x),
    this->z(x)
{}



// inline void Vec3::set( float x, float y ,float z)
// {
//     this->x = x,
//     this->y = y,
//     this->z = z
// }

// inline void Vec3::zero()
// {
//     this->x = 0.0f;
//     this->y = 0.0f;
//     this->z = 0.0f;
// }

//inline float Vec3::normalize()
// {
//     float squareLength, inverseLength;
//     squareLength = x * x + y * y + z * z;
//     inverseLength = Math::invSqrt(squareLength);
//     x *= inverseLength;
//     y *= inverseLength;
//     z *= inverseLength;

//     return inverseLength * squareLength;
// }