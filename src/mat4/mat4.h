#ifndef INCLUDED_MAT4_
#define INCLUDED_MAT4_
#include "../vec3/vec3.h"
#include "../xform_state/xform_state.h"

// ROW MAJOR. otherwise my brain breaks w.r.t. initialization.
struct Mat4
{
    float data[4][4]; // we can always change this. 

    float *operator[](size_t idx)
    {
    	//assert(idx < 4)
    	return data[idx];
    }
    float const *operator[](size_t idx) const
    {
        return data[idx];
    }
};

// free mat4 functions.

//@Todo: is this matmul row major?
inline Mat4 operator*(Mat4& lhs, Mat4& rhs)
{
    const float *m1Ptr, *m2Ptr;
    float *dstPtr;
    Mat4 dst;

    m1Ptr = reinterpret_cast<const float *>(&lhs);
    m2Ptr = reinterpret_cast<const float *>(&rhs);
    dstPtr = reinterpret_cast<float *>(&dst);

    for (int i = 0; i < 4; i++ ) {
        for (int j = 0; j < 4; j++ ) {
            *dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
            + m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
            + m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
            + m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
            dstPtr++;
        }
        m1Ptr += 4;
    }
    return dst;
}

inline Mat4& operator*=(Mat4& lhs, const float factor)
{
    lhs[0][0] *= factor;
    lhs[1][1] *= factor;
    lhs[2][2] *= factor;

    return lhs;
}

inline Mat4& operator*=(Mat4& lhs, Mat4& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}


// we assume that mat4 is ROW MAJOR here.
// if they are not, we can swap these two functions.
inline Mat4 from_row_vec3(const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
    return {v0.x, v0.y, v0.z, 0,
            v1.x, v1.y, v1.z, 0,
            v2.x, v2.y, v2.z, 0,
            0,      0,     0, 1};
}

inline Mat4 from_col_vec3(const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
    return {v0.x, v1.x , v2.x, 0,
            v0.y, v1.y,  v2.y, 0,
            v0.z, v1.z, v2.z,  0,
            0,    0,    0,      1};
}

// convert quaternion to matrix
// non-optimized
inline Mat4 from_quat(const Vec4& quaternion)
{
    Vec3 v0 = rotate_by_quat(Vec3{0,0,0}, quaternion);    // in xform_state.
    Vec3 v1 = rotate_by_quat(Vec3{0,1,0}, quaternion);
    Vec3 v2 = rotate_by_quat(Vec3{0,0,1}, quaternion);

    Mat4 result = from_row_vec3(v0, v1, v2); // in mat4.

    return result;
}


#endif