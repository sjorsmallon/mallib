#ifndef INCLUDED_vec3_
#define INCLUDED_vec3_

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

namespace mgl
{
    union vec3
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
        
        float operator[](size_t idx)
        {
            return data[idx];
        }
        float const operator[](size_t idx) const
        {
            return data[idx];
        }
    };

    union uivec3
    {
        struct
        {
            unsigned int x;
            unsigned int y;
            unsigned int z;
        };
        unsigned int data[3];

    };
 
};


// vec3u operators
inline mgl::uivec3& operator-=(mgl::uivec3& lhs, float rhs)
{   
    lhs.x -= rhs;
    lhs.y -= rhs;
    lhs.z -= rhs;
    return lhs;
}

// vec3 operators
inline mgl::vec3 operator-(mgl::vec3 lhs, mgl::vec3 rhs)
{
    return {lhs.x -rhs.x , lhs.y - rhs.y, lhs.z - rhs.z};
}
inline mgl::vec3 operator+(mgl::vec3 lhs, mgl::vec3 rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

inline mgl::vec3 operator*(const mgl::vec3 lhs, const float factor)
{
    return {lhs.x * factor, lhs.y * factor, lhs.z * factor};
}

inline mgl::vec3 operator*(const float factor, const mgl::vec3 rhs)
{
    return {factor * rhs.x, factor * rhs.y, factor * rhs.z};
}

#endif


