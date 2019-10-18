#ifndef INCLUDED_VEC4_
#define INCLUDED_VEC4_

//@Note: is this a problem with matrices?
union Vec4
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct 
    {
        float r;
        float g;
        float b;
        float a;
    };
    float data[4];
};

//@Refactor: restructure the vec3 /vec4 inconsistencies. now vec3 lives in the vec namespace.
// do we want all to live in the vec namespace?
inline void normalize(Vec4& lhs)
{
    float one_over_sum =  1.0f / lhs.x + lhs.y + lhs.z + lhs.w;
    lhs.x *= one_over_sum;
    lhs.y *= one_over_sum;
    lhs.z *= one_over_sum;
    lhs.w *= one_over_sum;
}


#endif