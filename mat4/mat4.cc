#include "mat4.ih"

        // Mat4&  operator*=(const Mat4 &rhs);
        // Mat4&  operator*=(const float rhs);
        // Mat4&  operator+=(const Mat4 &rhs);
        // Mat4&  operator-=(const Mat4 &rhs);

        // friend Mat4   operator*(const float lhs, const Mat4 &rhs);
        // friend Vec4f  operator*(const Vec4f &lhs, const Mat4 &rhs);
        // friend Vec3f  operator*(const Vec3f &lhs, const Mat4 &rhs);
        // friend Vec4f& operator*=(Vec4f &lhs, const Mat4 &rhs);
        // friend Vec3f& operator*=(Vec3f &lhs, const Mat4 &rhs);



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


// inline std::string Mat4::toString()
// {
//     std::stringstream ss;
//     ss << std::to_string(d_matrix[0].x) << " , " << std::to_string(d_matrix[1].x) << " , " << std::to_string(d_matrix[2].x) << " , " << std::to_string(d_matrix[3].x) << '\n'
//        << std::to_string(d_matrix[0].y) << " , " << std::to_string(d_matrix[1].y) << " , " << std::to_string(d_matrix[2].y) << " , " << std::to_string(d_matrix[3].y) << '\n'  
//        << std::to_string(d_matrix[0].z) << " , " << std::to_string(d_matrix[1].z) << " , " << std::to_string(d_matrix[2].z) << " , " << std::to_string(d_matrix[3].z) << '\n'
//        << std::to_string(d_matrix[0].w) << " , " << std::to_string(d_matrix[1].w) << " , " << std::to_string(d_matrix[2].w) << " , " << std::to_string(d_matrix[3].w) << '\n';

//     return ss.str();
// }

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

// inline Mat3 Mat4::toMat3() const
// {
//     // //mat3 is column forward, so it should be transposed:
//     // Mat3 matrix(d_matrix[0][0],  d_matrix[1][0], d_matrix[2][0],
//     //             d_matrix[0][1],  d_matrix[1][1], d_matrix[2][1],
//     //             d_matrix[0][2],  d_matrix[1][2], d_matrix[2][2]
//     //             );
//     return matrix;
// }

// inline Mat3 Mat4::normalMatrix() const
// {
//     // Mat3 matrix = toMat3(); //in toMat3, we invert before creating, so we get the right numbers in the right place.
//     // matrix.inverseSelf();
//     // matrix.transposeSelf();
//     // return matrix;
// }
