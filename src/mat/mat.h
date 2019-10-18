#ifndef INCLUDED_MAT_
#define INCLUDED_MAT_

#include "../mat4/mat4.h"
#include "../vec3/vec3.h"
#include "../quaternion/quaternion.h"
#include "../xform_state/xform_state.h"

//idea: instead of creating a translation / scale / rotation matrix everytime,
// keep a static one in the namespace?

namespace mat
{
    // mat4
    Mat4 from_xform_state(const Xform_State& state);
    // Mat4 from_quaternion(const Quaternion& quaternion);
    Mat4 mat4_identity();

    Mat4 translate(const Mat4& matrix, const vec::Vec3& vector);
    Mat4 scale(const Mat4& matrix, const float scale_factor);
    Mat4 scale(const float s);
    Mat4 rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z); //expects identity

    void to_identity(Mat4 &matrix);
    
    Mat4 perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane);

    Mat4 translation(const vec::Vec3& position);
    Mat4 view(const vec::Vec3& eye, const vec::Vec3& center, const vec::Vec3& up);
    Mat4 look_at(const vec::Vec3& eye, const vec::Vec3& center, const vec::Vec3& up);

    Mat4 ortho( const float left,
                const float right,
                const float top,
                const float bottom,
                const float z_near,
                const float z_far
              );
    //@Refactor: this becomes unclear when we can use from_xform_state or from_row_vec3 for both mat3 and mat4. 
    // we should separate them.
    Mat4 from_row_vec3(const vec::Vec3& v0, const vec::Vec3& v1, const vec::Vec3& v2);

    // Mat3 from_col_vec3();
};

inline Mat4 mat::from_xform_state(const Xform_State& state)
{
      Mat4 model_matrix       = mat::mat4_identity();
      // Mat4 rotation_matrix    = mat::from_quaternion(xform.q_orientation);
      Mat4 rotation_matrix    = mat::mat4_identity();
      Mat4 translation_matrix = mat::translation(state.position);
      model_matrix *= state.scale;
      // model_matrix *= rotation_matrix;
      model_matrix *= translation_matrix;
      return model_matrix;
}

// inline Mat4 mat::from_quaternion()
// {

// }


inline Mat4 mat::translation(const vec::Vec3& position)
{
    return {
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0, 1
            };
}


inline Mat4 mat::scale(const float s)
{
    return {
              s, 0, 0, 0,
              0, s, 0, 0,
              0, 0, s, 0,
              0, 0, 0, 1
           };
}

inline Mat4 mat::mat4_identity()
{
  return {
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1
         };
}

// inline void mat::to_identity(Mat4& lhs)
// {
//    // nothing happens.
// }

inline Mat4 mat::ortho(const float left,
                           const float right,
                           const float top,
                           const float bot,
                           const float z_near,
                           const float z_far
                           )
{

    return Mat4{2.0f / right - left, 0.0f, 0.0f, - (right + left / right - left),
                                    0.0f,  2.0f / top - bot, 0.0f, - ( top + bot / top - bot),
                                    0.0f, 0.0f, (-2.0f / (z_far - z_near)), -(z_far + z_near / z_far - z_near),
                                    0.0f,  0.0f,  0.0f,  1};  
}


//@Incomplete: inline assignment instead of creating and returning?
//@Incomplete: is this inverted because of the column vs row major?
inline Mat4 mat::perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane)
{
    Mat4 matrix = {};
    const float PI = 3.1415926535;
    const float DEG2RAD = PI / 180.0f;
    const float RAD2DEG = 180.0f / PI;

    float rad_fov = fov_y * DEG2RAD;
    float tan_half_fov = tanf(rad_fov / 2.0f);
      
    matrix[0][0] = 1.0f /(aspect_ratio * tan_half_fov);
    matrix[1][1] = 1.0f / tan_half_fov;
    matrix[2][2] = - (far_plane + near_plane) / (far_plane - near_plane);
    matrix[2][3] = - (2.0f * far_plane * near_plane) / (far_plane - near_plane);
    matrix[3][2] = - 1.0f;
    matrix[3][3] = 0.0f;
}


inline Mat4 mat::view(const vec::Vec3& eye, const vec::Vec3& center, const vec::Vec3& up)
{
   // modeled after gluLookAt. 

   vec::Vec3 forward = center - eye; // f = coord_system
   vec::normalize(forward);
   vec::Vec3 tmp  = {0,1,0};
   vec::Vec3 right = vec::cross(vec::normalize(tmp), forward);
   vec::Vec3 up_norm = up;
   vec::normalize(up);

   // The up vector must not be parallel to the line of sight from the
   //           eye point to the reference point.
    return {
              s.x,  s.y,  s.z, 0,
              up.x,  up.y,  up.z, 0,
             -forward.x, -forward.y, -forward.z, 0,
                0,    0,    0, 1
           };
}

// we assume that mat4 is ROW MAJOR here.
// if they are not, we can swap these two functions.
inline Mat4 mat::from_row_vec3(const vec::Vec3& v0, const vec::Vec3& v1, const vec::Vec3& v2)
{
    return {v0.x, v0.y, v0.z, 0,
            v1.x, v1.y, v1.z, 0,
            v2.x, v2.y, v2.z, 0,
            0,      0,     0, 1};
}

inline Mat4 mat::from_col_vec3(const vec::Vec3& v0, const vec::Vec3& v1, const vec::Vec3& v2)
{
    return {v0.x, v1.x , v2.x, 0,
            v0.y, v1.y,  v2.y, 0,
            v0.z, v1.z, v2.z,  0,
            0,    0,    0,      1};
}

#endif