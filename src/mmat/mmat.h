#ifndef INCLUDED_MATRIX_TRANSFORM_
#define INCLUDED_MATRIX_TRANSFORM_
#include "../mat4/mat4.h"
#include "../vec3/vec3.h"
#include "../quaternion/quaternion.h"
//idea: instead of creating a translation / scale / rotation matrix everytime,
// keep a static one in the namespace?


namespace mmat
{
    Mat4 from_xform_state(const Xform_State& state);
    Mat4 from_quaternion(const Quaternion& quaternion);
    Mat4 mat4_identity();

    Mat4 translate(const Mat4& matrix, const Vec3& vector);
    Mat4 scale(const Mat4& matrix, const float scale_factor);
    Mat4 rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z); //expects identity

    void to_identity(Mat4 &matrix);
    
    Mat4 perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane);


    Mat4 view(const Vec3& eye, const Vec3& center, const Vec3& up);
    Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up);

    Mat4 orthographic3D( const float left,
                         const float right,
                         const float top,
                         const float bottom,
                         const float z_near,
                         const float z_far
                        );

}

inline mat4 mmat::scale(const float s)
{
    return {
              s, 0, 0, 0,
              0, s, 0, 0,
              0, 0, s, 0,
              0, 0, 0, 1
           }
}

inline Mat4 mmat::mat4_identity()
{
  return {
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1
         };
}

inline Mat4 mmat::orthographic3D(const float left,
                           const float right,
                           const float top,
                           const float bottom,
                           const float z_near,
                           const float z_far
                           )
{
    return mat4{2.0f / right - left, 0.0f, 0.0f, - (right + left / right - left),
                                    0.0f,  2.0f / top - bot, 0.0f, - ( top + bot / top - bot),
                                    0.0f, 0.0f, (-2.0f / (z_far - z_near)), -(z_far + z_near / z_far - z_near),
                                    0.0f,  0.0f,  0.0f,  1};
}





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


#endif