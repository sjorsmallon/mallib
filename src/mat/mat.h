#ifndef INCLUDED_MAT_
#define INCLUDED_MAT_

#include <fmt/format.h>
#include <fmt/core.h> // for printing mat3, mat4.

#include "../mat4/mat4.h"
#include "../mat3/mat3.h"
#include "../vec3/vec3.h"
#include "../quaternion/quaternion.h"
#include "../xform_state/xform_state.h"

// print definitions for Mat3 and Mat4.
//@Refactor: we need to move these to somewhere.
namespace fmt {
    template <>
    struct formatter<Mat3> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Mat3 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "\n|{:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f}|\n",
            lhs[0][0], lhs[0][1], lhs[0][2],
            lhs[1][0], lhs[1][1], lhs[1][2],
            lhs[2][0], lhs[2][1], lhs[2][2]);
      }
    };

    template <>
    struct formatter<Mat4> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Mat4 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(),
            "\n|{:.3f} {:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f} {:.3f}|\n",lhs[0][0], lhs[0][1], lhs[0][2], lhs[0][3],
            lhs[1][0], lhs[1][1], lhs[1][2], lhs[1][3],
            lhs[2][0], lhs[2][1], lhs[2][2], lhs[2][3],
            lhs[3][0], lhs[3][1], lhs[3][2], lhs[3][3]);
      }
    };

    template <>
    struct formatter<Xform_State> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Xform_State &state, FormatContext &ctx) {
        return format_to(ctx.out(),
            "\nx: {:.3f}, y: {:.3f}, z: {:.3f}\nq_x: {:.3f}, q_y: {:.3f}, q_z: {:.3f}, q_w: {:.3f}\nscale:{:.3f}\n",
            state.position.x, state.position.y, state.position.z,
            state.q_orientation.x, state.q_orientation.y, state.q_orientation.z, state.q_orientation.w,
            state.scale
            );
      }
    };

};

//idea: instead of creating a translation / scale / rotation matrix everytime,
// keep a static one in the namespace?
namespace mat
{
    //@Refactor: move this to math?
    static inline const float PI = 3.1415926535;
    static inline const float DEG2RAD = mat::PI / 180.0f;
    static inline const float RAD2DEG = 180.0f / mat::PI;


    // mat4
    Mat4 mat4_from_xform_state(const Xform_State& state);
    Mat4 mat4_from_quat(const Vec4& quaternion);
    Mat4 mat4_from_mat3(const Mat4& lhs);
    Mat4 mat4_from_row_vec3(const Vec3& v0, const Vec3& v1, const Vec3& v2);

    Mat4 mat4_identity();
    void to_identity(Mat4 &matrix);


    Mat4 translate(const Mat4& matrix, const Vec3& vector);
    Mat4 scale(const Mat4& matrix, const float scale_factor);
    Mat4 scale(const float s);
    Mat4 rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z); //expects identity
    Mat4 perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane);

    Mat4 translation(const Vec3& position);
    Mat4 view(const Vec3& eye, const Vec3& center, const Vec3& up);
    Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up);
    Mat4 ortho( const float left,
                const float right,
                const float top,
                const float bottom,
                const float z_near,
                const float z_far
              );

    // mat3
    Mat3 normal_transform(const Mat4& model_view_matrix);
    Mat3 mat3_from_mat4(const Mat4& matrix);

    // @Note: return reference for chaining?
    // try_inverse, since it can fail.
    bool try_inverse(Mat3& lhs);
    void transpose(Mat3& lhs); 
    inline float determinant(Mat3& lhs);


};

inline Mat4 mat::mat4_from_row_vec3(const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
    return {v0.x, v0.y, v0.z, 0,
            v1.x, v1.y, v1.z, 0,
            v2.x, v2.y, v2.z, 0,
            0,      0,     0, 1};
}

inline Mat4 mat::mat4_from_quat(const Vec4& quaternion)
{
    Vec3 v0 = rotate_by_quat(Vec3{1,0,0}, quaternion);  
    Vec3 v1 = rotate_by_quat(Vec3{0,1,0}, quaternion);
    Vec3 v2 = rotate_by_quat(Vec3{0,0,1}, quaternion);

    Mat4 result =  mat::mat4_from_row_vec3(v0, v1, v2); // in mat4.

    return result;
}

inline Mat4 mat::mat4_from_xform_state(const Xform_State& state)
{
    Mat4 model_matrix       = mat::mat4_identity();
    Mat4 rotation_matrix    = mat4_from_quat(state.q_orientation); // in mat4.
    Mat4 translation_matrix = mat::translation(state.position);

    model_matrix[0][0] *= state.scale;
    model_matrix[1][1] *= state.scale;
    model_matrix[2][2] *= state.scale;

    model_matrix *= rotation_matrix;
    model_matrix *= translation_matrix;
      
    return model_matrix;
}

inline Mat4 mat::translation(const Vec3& position)
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
    Mat4 matrix = {}; // identity?

    float rad_fov = fov_y * mat::DEG2RAD;
    float tan_half_fov = tanf(rad_fov / 2.0f);
      
    matrix[0][0] = 1.0f /(aspect_ratio * tan_half_fov);
    matrix[1][1] = 1.0f / tan_half_fov;
    matrix[2][2] = - (far_plane + near_plane) / (far_plane - near_plane);
    matrix[2][3] = - (2.0f * far_plane * near_plane) / (far_plane - near_plane);
    matrix[3][2] = - 1.0f;
    matrix[3][3] = 0.0f;

    return matrix;
}


inline Mat4 mat::view(const Vec3& eye, const Vec3& center, const Vec3& up)
{
   // modeled after gluLookAt. 

   Vec3 forward = center - eye; // f = coord_system
   vec::normalize(forward);
   Vec3 tmp = {0,1,0};
   vec::normalize(tmp);
   Vec3 right = vec::cross(tmp, forward);
   Vec3 up_norm = up;
   vec::normalize(up_norm);

   // The up vector must not be parallel to the line of sight from the
   //           eye point to the reference point.
    return {    right.x,    right.y,    right.z, 0,
              up_norm.x,  up_norm.y,  up_norm.z, 0,
             -forward.x, -forward.y, -forward.z, 0,
                      0,          0,          0, 1};
}
   
inline Mat3 mat::mat3_from_mat4(const Mat4& matrix)
{
    return {matrix[0][0], matrix[0][1], matrix[0][2],
            matrix[1][0], matrix[1][1], matrix[1][2],
            matrix[2][0], matrix[2][1], matrix[2][2]};
}

inline bool mat::try_inverse(Mat3& lhs)
{
    Mat3 inverse = {};

    inverse[0][0] = lhs[1][1] * lhs[2][2] - lhs[2][1] * lhs[1][2];
    inverse[0][1] = lhs[2][1] * lhs[0][2] - lhs[0][1] * lhs[2][2];
    inverse[0][2] = lhs[0][1] * lhs[1][2] - lhs[1][1] * lhs[0][2];

    double det = lhs[0][0] * inverse[0][0] + lhs[1][0] * inverse[0][1] + lhs[2][0] * inverse[0][2];

    // if det is too small, the inverse does not exist.
    // if ( ) {
    //     return false;
    // }
    double inv_det = 1.0f / det;
    inverse[1][0] = lhs[2][0] * lhs[1][2] - lhs[1][0] * lhs[2][2];
    inverse[2][0] = lhs[1][0] * lhs[2][1] - lhs[2][0] * lhs[1][1];
    inverse[1][1] = lhs[0][0] * lhs[2][2] - lhs[2][0] * lhs[0][2];
    inverse[2][1] = lhs[2][0] * lhs[0][1] - lhs[0][0] * lhs[2][1];
    inverse[1][2] = lhs[1][0] * lhs[0][2] - lhs[0][0] * lhs[1][2];
    inverse[2][2] = lhs[0][0] * lhs[1][1] - lhs[1][0] * lhs[0][1];

    lhs[0][0] = inverse[0][0] * inv_det;

    lhs[1][0] = inverse[1][0] * inv_det;
    lhs[2][0] = inverse[2][0] * inv_det;

    lhs[0][1] = inverse[0][1] * inv_det;
    lhs[1][1] = inverse[1][1] * inv_det;
    lhs[2][1] = inverse[2][1] * inv_det;

    lhs[0][2] = inverse[0][2] * inv_det;
    lhs[1][2] = inverse[1][2] * inv_det;
    lhs[2][2] = inverse[2][2] * inv_det;

    return true;
}


inline float mat::determinant(Mat3& lhs)
{
    //det(A) = a11a22a33 + a12a23a31 + a13a21a32 
    //         -a13a22a31 - a12a21a33 - a11a23a32.

    return {   lhs[0][0] * lhs[1][1] * lhs[2][2]
             + lhs[0][1] * lhs[1][2] * lhs[2][0]
             + lhs[0][2] * lhs[1][0] * lhs[2][1]
             - lhs[0][2] * lhs[1][1] * lhs[2][0]
             - lhs[0][1] * lhs[1][0] * lhs[2][2]
             - lhs[0][0] * lhs[1][2] * lhs[2][1] }; 
}

// @Refactor: either write templated swap or include it from utility.
static void swap(float & lhs, float& rhs)
{
    float t = lhs;
    lhs = rhs;
    rhs = t;
}

inline void mat::transpose(Mat3& lhs)
{
    swap(lhs[0][1], lhs[1][0]);
    swap(lhs[0][2], lhs[2][0]);
    swap(lhs[1][2], lhs[2][1]);
}

inline Mat3 mat::normal_transform(const Mat4& model_view_matrix)
{
    // normal matrix is calculated from the modelview
    Mat3 normal_matrix = mat3_from_mat4(model_view_matrix);
    if (!try_inverse(normal_matrix))
    { 
        fmt::print("[mat] normal_transform: try_inverse failed.\n");
    }
    transpose(normal_matrix);

    return normal_matrix;
}


#endif