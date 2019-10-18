#ifndef INCLUDED_VEC3_
#define INCLUDED_VEC3_

// math: invsqrt

// supports aggregate initialization, POD type
// cannot specify constructors.
// helper functions are in vector_transform.
// some operator overloading in order to facilitate 
// many-used operations.

//@Refactor: this namespace thing. Is it very useful?

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

    union Vec3u
    {
        struct
        {
            unsigned int x;
            unsigned int y;
            unsigned int z;
        };
        unsigned int data[3];
    };
    
    // vec3u operators
    inline Vec3u& operator-=(Vec3u& lhs, float rhs)
    {   
        lhs.x -= rhs;
        lhs.y -= rhs;
        lhs.z -= rhs;
        return lhs;
    }

    // Vec3 operators
    inline Vec3 operator-(Vec3 lhs, Vec3 rhs)
    {
        return {lhs.x -rhs.x , lhs.y - rhs.y, lhs.z - rhs.z};
    }
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

    inline Vec3& normalize(Vec3& lhs)
    {
        float one_over_sum = 1.0f / (lhs.x + lhs.y + lhs.z);
        lhs.x *= one_over_sum;
        lhs.y *= one_over_sum;
        lhs.z *= one_over_sum;

        return lhs;
    }

    inline float dot(Vec3 lhs, Vec3 rhs)
    {
        return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
    }

    inline Vec3 cross(Vec3 lhs, Vec3 rhs)
    {
        Vec3 result = {};

        result.x = lhs.y * rhs.z - lhs.z * rhs.y; //aybz − azby
        result.y = lhs.z * rhs.x - lhs.x * rhs.z; //azbx − axbz
        result.z = lhs.x * rhs.y - lhs.y * rhs.x; //axby − aybx

        return result;
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


