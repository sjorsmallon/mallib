#ifndef INCLUDED_MAT4_
#define INCLUDED_MAT4_
#include "../vector/vec4f.h"
#include "../vector/vec3f.h"
#include <string> //for toString
#include <cstring> //memcpy
#include <cmath> //tanh
#include <sstream> // for toString
#include "mat3.h"

#define PI 3.14159265358979323846f //@Cleanup: move these to math
#define DEG2RAD  PI / 180.0f
#define RAD2DEG  180.f / PI

// internal column major, but behaves as row major from outside perspective.
// is a class for that exact reason.

class Mat4
{
    Vec4 d_matrix[4];

    public:
        Mat4() {};
        // identity constructor?
        explicit Mat4(const Vec4f &x, const Vec4f &y, const Vec4f &z, const Vec4f &w);
        explicit Mat4(const float xx, const float xy, const float xz, const float xw,
                      const float yx, const float yy, const float yz, const float yw,
                      const float zx, const float zy, const float zz, const float zw,
                      const float wx, const float wy, const float wz, const float ww);
        // explicit Mat4(const  Mat3 &rotation, const Vec3 &translation); we'll see how to fix this!
        explicit Mat4(const float source[4][4]);

  
        const Vec4f &operator[](int index) const;
        Vec4f& operator[](int index);
        Mat4   operator*(const float rhs) const;
        Vec4f  operator*(const Vec4f &rhs) const;
        Vec3f  operator*(const Vec3f &rhs) const;
        Mat4   operator*(const Mat4 &rhs) const;
        Mat4   operator+(const Mat4 &rhs) const;
        Mat4   operator-(const Mat4 &rhs) const;
        Mat4&  operator*=(const Mat4 &rhs);
        Mat4&  operator*=(const float rhs);
        Mat4&  operator+=(const Mat4 &rhs);
        Mat4&  operator-=(const Mat4 &rhs);

        friend Mat4   operator*(const float lhs, const Mat4 &rhs);
        friend Vec4f  operator*(const Vec4f &lhs, const Mat4 &rhs);
        friend Vec3f  operator*(const Vec3f &lhs, const Mat4 &rhs);
        friend Vec4f& operator*=(Vec4f &lhs, const Mat4 &rhs);
        friend Vec3f& operator*=(Vec3f &lhs, const Mat4 &rhs);

        bool compare(const Mat4 &rhs) const;
        bool operator==(const Mat4 &rhs) const;
        bool operator!=(const Mat4 &rhs) const;

        //Hacky stuff here.
        Mat3 toMat3() const;
        Mat3 normalMatrix() const;
§
    
        // void zero();
        // void toIdentity();

        // bool isIdentity();
        // bool isDiagonal();

        // std::string toString();
        
        const float *data() const;
        float *data();

};

// this behavior is not yet correct. Do we want to feed column vectors here too?

inline Mat4::Mat4(const Vec4f &x, const Vec4f &y, const Vec4f &z, const Vec4f &w)
{
    d_matrix[0] = x;
    d_matrix[1] = y;
    d_matrix[2] = z;
    d_matrix[3] = w;
}

// note: the internal data is thus "transposed".
inline  Mat4::Mat4(   const float xx, const float xy, const float xz, const float xw,
                      const float yx, const float yy, const float yz, const float yw,
                      const float zx, const float zy, const float zz, const float zw,
                      const float wx, const float wy, const float wz, const float ww)
:
    d_matrix(Vec4{xx, yx, zx, wx},
             Vec4{xy, yy, zy, wy},
             Vec4{xz, yz, zz, wz},
             VEc4{xw, yw, zw, ww})
{
    // d_matrix[0][0]= xx; d_matrix[1][0] =xy; d_matrix[2][0] =xz; d_matrix[3][0] =xw;
    // d_matrix[0][1]= yx; d_matrix[1][1] =yy; d_matrix[2][1] =yz; d_matrix[3][1] =yw; 
    // d_matrix[0][2]= zx; d_matrix[1][2] =zy; d_matrix[2][2] =zz; d_matrix[3][2] =zw;
    // d_matrix[0][3]= wx; d_matrix[1][3] =wy; d_matrix[2][3] =wz; d_matrix[3][3] =ww;
}

// inline void Mat4::zero()
// {
//   memset( d_matrix, 0, sizeof( Mat4 ) );
// }

// inline void Mat4::toIdentity()
// {
//     memset( d_matrix, 0, sizeof( Mat4 ) );
//     d_matrix[0][0] = 1;
//     d_matrix[1][1] = 1;
//     d_matrix[2][2] = 1;
//     d_matrix[3][3] = 1;
// }

inline bool Mat4::compare(const Mat4 &rhs) const
{
    const float *ptr1, *ptr2;
    ptr1 = reinterpret_cast<const float *>(d_matrix);
    ptr2 = reinterpret_cast<const float *>(rhs.d_matrix);
    for (int idx = 0; idx != 4 * 4; ++i)
    {
        if ( ptr1[i] != ptr2[i] )
        {
            return false;
        }
    }
    return true;
}


inline Mat4::Mat4( const float source[ 4 ][ 4 ] )
{
  memcpy( d_matrix, source, 4 * 4 * sizeof( float ) );
}

inline const Vec4f &Mat4::operator[](int index) const
{
    if (index < 0 || index > 4) {}
    return d_matrix[index];
}

inline Vec4f &Mat4::operator[](int index)
{
    return d_matrix[index];
}

inline Mat4 Mat4::operator*(const float rhs) const
{
    return Mat4 (
                    d_matrix[0][0] * rhs, d_matrix[1][0] * rhs, d_matrix[2][0] * rhs, d_matrix[3][0] * rhs,
                    d_matrix[0][1] * rhs, d_matrix[1][1] * rhs, d_matrix[2][1] * rhs, d_matrix[3][1] * rhs,
                    d_matrix[0][2] * rhs, d_matrix[1][2] * rhs, d_matrix[2][2] * rhs, d_matrix[3][2] * rhs,
                    d_matrix[0][3] * rhs, d_matrix[1][3] * rhs, d_matrix[2][3] * rhs, d_matrix[3][3] * rhs
                );
}

inline Vec4f Mat4::operator*(const Vec4f &rhs) const
{
    return Vec4f(
                    d_matrix[0][0] * rhs.x + d_matrix[1][0] * rhs.y + d_matrix[2][0] * rhs.z + d_matrix[3][0] * rhs.w,
                    d_matrix[0][1] * rhs.x + d_matrix[1][1] * rhs.y + d_matrix[2][1] * rhs.z + d_matrix[3][1] * rhs.w,
                    d_matrix[0][2] * rhs.x + d_matrix[1][2] * rhs.y + d_matrix[2][2] * rhs.z + d_matrix[3][2] * rhs.w,
                    d_matrix[0][3] * rhs.x + d_matrix[1][3] * rhs.y + d_matrix[2][3] * rhs.z + d_matrix[3][3] * rhs.w
                );
}

/*
inline Vec3f Mat4::operator*(const Vec3f &rhs) const
{
    float s = d_matrix[3].x * rhs.x + d_matrix[3].y * rhs.y + d_matrix[3].z * rhs.z + d_matrix[4].w;
    
    
    if (s = 0.0f)
        return Vec3f( 0.0f, 0.0f, 0.0f);
    if (s == 1.0f)
    {
        return Vec3f(
                      d_matrix[0].x * rhs.x + d_matrix[0].y * rhs.y + d_matrix[0].z * rhs.z + d_matrix[0].w,
                      d_matrix[1].x * rhs.x + d_matrix[1].y * rhs.y + d_matrix[1].z * rhs.z + d_matrix[1].w,
                      d_matrix[2].x * rhs.x + d_matrix[2].y * rhs.y + d_matrix[2].z * rhs.z + d_matrix[2].w 
                    );                    
    }
    else
    {
        float invS = 1.0f / s;
        return Vec3f(
                      (d_matrix[0].x * rhs.x + d_matrix[0].y * rhs.y + d_matrix[0].z * rhs.z + d_matrix[0].w) * invS,
                      (d_matrix[1].x * rhs.x + d_matrix[1].y * rhs.y + d_matrix[1].z * rhs.z + d_matrix[1].w) * invS,
                      (d_matrix[2].x * rhs.x + d_matrix[2].y * rhs.y + d_matrix[2].z * rhs.z + d_matrix[2].w) * invS 
                    );
    }
}

*/

inline Mat4 Mat4::operator*(const Mat4 &rhs) const
{
    int i, j;
    const float *m1Ptr, *m2Ptr;
    float *dstPtr;
    Mat4 dst;

    m1Ptr = reinterpret_cast<const float *>(this);
    m2Ptr = reinterpret_cast<const float *>(&rhs);
    dstPtr = reinterpret_cast<float *>(&dst);

    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 4; j++ ) {
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


inline Mat4 Mat4::operator+(const Mat4 &rhs) const
{
    return Mat4(
                d_matrix[0][0] + rhs[0][0], d_matrix[1][0] + rhs[1][0], d_matrix[2][0] + rhs[2][0], d_matrix[3][0] + rhs[3][0],
                d_matrix[0][1] + rhs[0][1], d_matrix[1][1] + rhs[1][1], d_matrix[2][1] + rhs[2][1], d_matrix[3][1] + rhs[3][1],
                d_matrix[0][2] + rhs[0][2], d_matrix[1][2] + rhs[1][2], d_matrix[2][2] + rhs[2][2], d_matrix[3][2] + rhs[3][2],
                d_matrix[0][3] + rhs[0][3], d_matrix[1][3] + rhs[1][3], d_matrix[2][3] + rhs[2][3], d_matrix[3][3] + rhs[3][3]
               );
} 

inline Mat4 Mat4::operator-(const Mat4 &rhs) const
{
    return Mat4(
                d_matrix[0][0] - rhs[0][0], d_matrix[1][0] - rhs[1][0], d_matrix[2][0] - rhs[2][0], d_matrix[3][0] - rhs[3][0],
                d_matrix[0][1] - rhs[0][1], d_matrix[1][1] - rhs[1][1], d_matrix[2][1] - rhs[2][1], d_matrix[3][1] - rhs[3][1],
                d_matrix[0][2] - rhs[0][2], d_matrix[1][2] - rhs[1][2], d_matrix[2][2] - rhs[2][2], d_matrix[3][2] - rhs[3][2],
                d_matrix[0][3] - rhs[0][3], d_matrix[1][3] - rhs[1][3], d_matrix[2][3] - rhs[2][3], d_matrix[3][3] - rhs[3][3]
               );
}

inline Mat4 &Mat4::operator*=(const float rhs) 
{
    d_matrix[0][0] *= rhs; d_matrix[1][0] *= rhs; d_matrix[2][0] *= rhs; d_matrix[3][0] *= rhs;
    d_matrix[0][1] *= rhs; d_matrix[1][1] *= rhs; d_matrix[2][1] *= rhs; d_matrix[3][1] *= rhs;
    d_matrix[0][2] *= rhs; d_matrix[1][2] *= rhs; d_matrix[2][2] *= rhs; d_matrix[3][2] *= rhs;
    d_matrix[0][3] *= rhs; d_matrix[1][3] *= rhs; d_matrix[2][3] *= rhs; d_matrix[3][3] *= rhs;
    
    return *this;
}

inline Mat4 &Mat4::operator*=(const Mat4 &rhs)
{
     *this = (*this) * rhs;
     return *this;
}

inline Mat4 &Mat4::operator+=(const Mat4 &rhs)
{
    d_matrix[0][0] += rhs[0][0]; d_matrix[1][0] += rhs[1][0]; d_matrix[2][0] += rhs[2][0]; d_matrix[3][0] += rhs[3][0];
    d_matrix[0][1] += rhs[0][1]; d_matrix[1][1] += rhs[1][1]; d_matrix[2][1] += rhs[2][1]; d_matrix[3][1] += rhs[3][1];
    d_matrix[0][2] += rhs[0][2]; d_matrix[1][2] += rhs[1][2]; d_matrix[2][2] += rhs[2][2]; d_matrix[3][2] += rhs[3][2];
    d_matrix[0][3] += rhs[0][3]; d_matrix[1][3] += rhs[1][3]; d_matrix[2][3] += rhs[2][3]; d_matrix[3][3] += rhs[3][3];
    
    return *this;
}

inline Mat4 &Mat4::operator-=(const Mat4 &rhs)
{
    d_matrix[0][0] -= rhs[0][0]; d_matrix[1][0] -= rhs[1][0]; d_matrix[2][0] -= rhs[2][0]; d_matrix[3][0] -= rhs[3][0];
    d_matrix[0][1] -= rhs[0][1]; d_matrix[1][1] -= rhs[1][1]; d_matrix[2][1] -= rhs[2][1]; d_matrix[3][1] -= rhs[3][1];
    d_matrix[0][2] -= rhs[0][2]; d_matrix[1][2] -= rhs[1][2]; d_matrix[2][2] -= rhs[2][2]; d_matrix[3][2] -= rhs[3][2];
    d_matrix[0][3] -= rhs[0][3]; d_matrix[1][3] -= rhs[1][3]; d_matrix[2][3] -= rhs[2][3]; d_matrix[3][3] -= rhs[3][3];

    return *this;
}

inline Mat4 operator*(const float lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * float
}

inline Vec4f operator*(const Vec4f &lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * vector4;
}

inline Vec3f operator*(const Vec3f &lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * vector3;
}


inline Vec4f &operator*=(Vec4f &lhs, const Mat4 &rhs)
{
    lhs = rhs * lhs;
    return lhs;
}

inline bool Mat4::operator==(const Mat4 &rhs) const 
{
  return compare(rhs);
}

inline bool Mat4::operator!=(const Mat4 &rhs) const 
{
  return !compare(rhs);
}

// inline std::string Mat4::toString()
// {
//     std::stringstream ss;
//     ss << std::to_string(d_matrix[0].x) << " , " << std::to_string(d_matrix[1].x) << " , " << std::to_string(d_matrix[2].x) << " , " << std::to_string(d_matrix[3].x) << '\n'
//        << std::to_string(d_matrix[0].y) << " , " << std::to_string(d_matrix[1].y) << " , " << std::to_string(d_matrix[2].y) << " , " << std::to_string(d_matrix[3].y) << '\n'  
//        << std::to_string(d_matrix[0].z) << " , " << std::to_string(d_matrix[1].z) << " , " << std::to_string(d_matrix[2].z) << " , " << std::to_string(d_matrix[3].z) << '\n'
//        << std::to_string(d_matrix[0].w) << " , " << std::to_string(d_matrix[1].w) << " , " << std::to_string(d_matrix[2].w) << " , " << std::to_string(d_matrix[3].w) << '\n';

//     return ss.str();
// }

inline float *Mat4::data()
{
    return d_matrix[0].data();
}

inline const float *Mat4::data() const
{
    return d_matrix[0].data();
}

inline Mat3 Mat4::toMat3() const
{
    // //mat3 is column forward, so it should be transposed:
    // Mat3 matrix(d_matrix[0][0],  d_matrix[1][0], d_matrix[2][0],
    //             d_matrix[0][1],  d_matrix[1][1], d_matrix[2][1],
    //             d_matrix[0][2],  d_matrix[1][2], d_matrix[2][2]
    //             );
    return matrix;
}

inline Mat3 Mat4::normalMatrix() const
{
    // Mat3 matrix = toMat3(); //in toMat3, we invert before creating, so we get the right numbers in the right place.
    // matrix.inverseSelf();
    // matrix.transposeSelf();
    // return matrix;
}

// inline Mat4 &Mat4::translateSelf(float x, float y, float z)
// {
//   d_matrix[3][0] = x;
//   d_matrix[3][1] = y;
//   d_matrix[3][2] = z;
  
//   return *this;
// }


// inline void Mat4::toPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
// {
//   float rad_fov = fov * DEG2RAD;
//   float tan_half_fov = tanf(rad_fov / 2.0f);
    
//   d_matrix[0][0] = 1.0f /(aspectRatio * tan_half_fov);
//   d_matrix[1][1] = 1.0f / tan_half_fov;
//   d_matrix[2][2] = -  (farPlane + nearPlane) / (farPlane - nearPlane);
//   d_matrix[2][3] = - (2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
//   d_matrix[3][2] = -1;
//   d_matrix[3][3] = 0;
          
// }
          

// inline Mat4 &Mat4::transposeSelf()
// {
//   float temp;
//   int row, col;
  
//   for (row = 0; row <4; row++)
//   {
//     for (col = row +1; col < 4; col++)
//     {
//       temp = d_matrix[row][col];
//       d_matrix[row][col] = d_matrix[col][row];
//       d_matrix[col][row] = temp;
//     }
//   }
//   return *this;
// }
  
// inline Mat4 &Mat4::scaleSelf(float factor)
// {
//     d_matrix[0][0] *= factor;
//     d_matrix[1][1] *= factor;
//     d_matrix[2][2] *= factor;
  
//     return *this;
// }
          
// inline Mat4 &Mat4::rotateSelf(int degrees_x, int degrees_y, int degrees_z)
// {
//     // the matrix should be in identity.
//     // toIdentity();

//     float rad_x = degrees_x * PI / 180.0f;
//     float rad_y = degrees_y * PI / 180.0f;
//     float rad_z = degrees_z * PI / 180.0f;

//     float cosx = cos(rad_x);
//     float sinx = sin(rad_x);
    
//     float cosy = cos(rad_y) ;
//     float siny = sin(rad_y);
    
//     float cosz = cos(rad_z);
//     float sinz = sin(rad_z);
    
//     Mat4 x_axis(1,   0,      0,   0,
//                    0,  cosx, -sinx,  0,
//                    0,  sinx,  cosx,  0,
//                 0,  0,      0,    1);
    
//     Mat4 y_axis(cosy,  0,   siny,   0,
//                 0,     1,      0,   0,
//                -siny,  0,    cosy,  0,
//                 0,     0,      0,   1);
    
//     Mat4 z_axis(cosz, -sinz,   0,   0,
//                 sinz,  cosz,   0,   0,
//                 0,     0,     1,   0,
//                 0,     0,     0,   1);
    
//     *this *= x_axis; 
//     *this *= y_axis;
//     *this *= z_axis;

//     return *this;
// }


          
#endif

