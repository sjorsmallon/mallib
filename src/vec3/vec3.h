#ifndef INCLUDED_VEC3_
#define INCLUDED_VEC3_

// math: invsqrt

// supports aggregate initialization, POD type
// cannot specify constructors.
// helper functions are in vector_transform.
// some operator overloading in order to facilitate 
// many-used operations.
// struct Vec3

// {

// }

union Vec3
{
    struct
    {
        float x;
        float y;
        float z;
    };
    struct 
    {
        float r;
        float g;
        float b;
    };
    float data[3];
};

// we can do a vec3 fuzzycompare, since we're dealing with floating point
// numbers.
// equality operator makes no sense here, as it is ill-defined.

inline Vec3 operator+(const Vec3 lhs, const Vec3 rhs)
{
    Vec3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;

    return result;
}

inline Vec3 operator-(const Vec3 lhs, const Vec3 rhs)
{
    Vec3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;

    return result;
}

// free functions

inline Vec3 operator*(const float lhs, const Vec3& rhs)
{
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

inline Vec3 operator+(const float lhs, const Vec3& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
}

#endif


