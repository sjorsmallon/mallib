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

//@Note: because the union has anonymous structs, and these are similar/ identical in layout,
// we can inspect the values of it via any of these anonymous structs. 
// that means we can either use x/y/z or r/g/b. 
// That's useful for functions like lerp().
namespace vec
{
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


    // Vec3 operators
    inline Vec3 operator+(Vec3 lhs, Vec3 rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    inline Vec3 operator*(const Vec3 lhs, const float factor)
    {
        return {lhs.x * factor, lhs.y * factor, lhs.z * factor};
    }

    inline Vec3 operator*(const float factor, const Vec3 rhs)
    {
        return {factor * rhs.x, factor * rhs.y, factor * rhs.z};
    }

    inline Vec3 lerp(Vec3 lhs, Vec3 rhs, float t)
    {
        return  (t * lhs + (1.0f - t) * rhs);
    }


}




// we can do a vec3 fuzzycompare, since we're dealing with floating point
// numbers.
// equality operator makes no sense here, as it is ill-defined.

// inline Vec3 operator+(const Vec3 lhs, const Vec3 rhs)
// {
//     Vec3 result;
//     result.x = lhs.x + rhs.x;
//     result.y = lhs.y + rhs.y;
//     result.z = lhs.z + rhs.z;

//     return result;
// }

// inline Vec3 operator-(const Vec3 lhs, const Vec3 rhs)
// {
//     Vec3 result;
//     result.x = lhs.x - rhs.x;
//     result.y = lhs.y - rhs.y;
//     result.z = lhs.z - rhs.z;

//     return result;
// }

// // free functions

// inline Vec3 operator*(const float lhs, const Vec3& rhs)
// {
//     return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
// }

// inline Vec3 operator+(const float lhs, const Vec3& rhs)
// {
//     return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
// }

#endif


