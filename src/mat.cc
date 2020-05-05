#include <mgl/mat.h>

#define PI 3.14159265359f
//@Incomplete: when do we need identity? keep a static const identity matrix
// in the namespace? (see id's implementation)

//@Refactor: this creates a copy. better to do in place? or does it not matter?
// void mat::to_identity(Mat4& matrix)
// {
//    matrix = mat::identity();
// }



// Mat4 mat::view(const Vec3& eye, const Vec3& center, const Vec3& up)
// {
//    // modeled after gluLookAt. 

//    Vec3 forward = center - eye; // f = coord_system
//    mvec::normalize(forward);
//    Vec3 tmp  = {0,1,0};
//    Vec3 right = mvec::cross(mvec::normalize(tmp),f);
//    Vec3 up_norm = up;
//    mvec::normalize(up);

    

//    // The up vector must not be parallel to the line of sight from the
//    //           eye point to the reference point.

//     return {
//               s.x,  s.y,  s.z, 0,
//               u.x,  u.y,  y.z, 0,
//              -f.x, -f.y, -f.z, 0,
//                 0,    0,    0, 1
//            };
// }

// Mat4 mat::look_at(const Vec3& eye, const Vec3& center, const Vec& up)
// {
//     mat::view(eye, center, up);
// }



///----- modeled after glm::rotate (modeled after gl's rotate matrix.)
///----- lhs will be rotated by the constructed rotation matrix.
///----- If unsure, pass identity to this function.
mgl::mat4 mgl::rotate(mgl::mat4& lhs, const float angle, const mgl::vec3& axis)
{

    float cos_angle = cos(a);
    float sin_angle= sin(a);

    axis = mgl::normalize(axis);

    mgl::vec3 temp =  (1.0f - cos_angle) * axis;

    mgl::mat4 rotation_matrix = mgl::mat4_identity();

    rotation_matrix[0][0] = cos_angle + (temp.x * axis.x);
    rotation_matrix[1][0] = (temp.x * axis.y) + (sin_angle * axis.z);
    rotation_matrix[2][0] = (temp.x * axis.z) - (sin_angle * axis.y);

    rotation_matrix[0][1] = (temp.y * axis.x) - (sin_angle * axis.z);
    rotation_matrix[1][1] = cos_angle + (temp.y * axis.y);
    rotation_matrix[2][1] = (temp.y * axis.z) + (sin_angle * axis.z);

    rotation_matrix[0][2] = (temp.z * axis.x) + (sin_angle * axis.y);
    rotation_matrix[1][2] = (temp.z * axis.y) - (sin_angle * axis.x);
    rotation_matrix[2][2] = cos_angle + (temp.z * axis.z);



    // Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
    // Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
    // Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
    // Result[3] = m[3];

    // result 

    return rotation_matrix;

}

// column major!
    // rotation_matrix[0][0] = cos_angle + temp[0] * axis[0];
    // rotation_matrix[0][1] = temp[0] * axis[1] + s * axis[2];
    // rotation_matrix[0][2] = temp[0] * axis[2] - s * axis[1];

    // rotation_matrix[1][0] = temp[1] * axis[0] - s * axis[2];
    // rotation_matrix[1][1] = cos_angle + temp[1] * axis[1];
    // rotation_matrix[1][2] = temp[1] * axis[2] + s * axis[0];

    // rotation_matrix[2][0] = temp[2] * axis[0] + s * axis[1];
    // rotation_matrix[2][1] = temp[2] * axis[1] - s * axis[0];
    // rotation_matrix[2][2] = cos_angle + temp[2] * axis[2];

    // mat<4, 4, T, Q> Result;
    // Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
    // Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
    // Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
    // Result[3] = m[3];
    // return Result;




// mgl::rotate(mgl::mat4& matrix, )
// {

// }



// Mat4 mat::scale(const Mat4& matrix, const float scale_factor)
// {
//     mat4 matrix{};
//     matrix[0][0] *= scale_factor;
//     matrix[1][1] *= scale_factor;
//     matrix[2][2] *= scale_factor;
// }

// Mat4 mat::mat4_identity()
// {
//     return { 1, 0, 0, 0,
//              0, 1, 0, 0,
//              0, 0, 1, 0,
//              0, 0, 0, 1};
// }
