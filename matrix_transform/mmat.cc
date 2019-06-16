#include "mmat.ih"

 
//@Incomplete: when do we need identity? keep a static const identity matrix
// in the namespace? (see id's implementation)
Mat4 mmat::identity()
{
  return {
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1
         };
}

//@Incomplete: keep confusing myself about column vs row major
Mat4 mmat::from_quaternion(const Quaternion& quaternion)
{
    float x2 = quaternion.x + quaternion.x;
    float y2 = quaternion.y + quaternion.y;
    float z2 = quaternion.z + quaternion.z;
    float xx = quaternion.x * x2;
    float xy = quaternion.x * y2;
    float xz = quaternion.x * z2;
    float yy = quaternion.y * y2;
    float yz = quaternion.y * z2;
    float zz = quaternion.z * z2;
    float wx = quaternion.w * x2;
    float wy = quaternion.w * y2;
    float wz = quaternion.w * z2;


    return { 1.0f - (yy + zz), xy - wz, xz + wy, 0.0f,
             xy + wz, 1.0f - (xx + zz), yz - wx, 0.0f,
             xz - wy, yz + wx, 1.0f - (xx + yy), 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f
           };
}

Mat4 mmat::from_xform_state(const Xform_State& xform_state) // so this is the model matrix?
{
  Mat4 model_matrix    = mmat::mat4_identity();
  Mat4 rotation_matrix = mmat::from_quaternion(xform.orientation);
  Mat4 translation_matrix = mmat::translation(xform.position);

  model_matrix *= xform_state.scale;
  model_matrix *= rotation_matrix;
  model_matrix *= translation_matrix;
}


// Mat4 mmat::translate(const Mat4& matrix, const Vec3& vector)
// {
//     // Mat4 new_matrix{matrix}; // copy construction;

//     matrix[3][0] = x; // should be +=? // doesn't work that way!
//     matrix[3][1] = y; // should be +=?
//     matrix[3][2] = z; // should be +=?
// }

Mat4 mmat::translation(const Vec3& position)
{
    return {
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0, 1
            };
}



//@Incomplete: inline assignment instead of creating and returning?
//@Incomplete: is this inverted because of the column vs row major?
Mat4 mmat::perspective(const float fov_y,
                       const float aspect_ratio,
                       const float near_plane,
                       const float far_plane);
{
  float rad_fov = fov_y * DEG2RAD;
  float tan_half_fov = tanf(rad_fov / 2.0f);
  
  Mat4 matrix{};

  matrix[0][0] = 1.0f / (aspect_ratio * tan_half_fov);
  matrix[1][1] = 1.0f / tan_half_fov;
  matrix[2][2] = -  (far_plane + near_plane) / (far_plane - near_plane);
  matrix[2][3] = - (2.0f * far_plane * near_plane) / (far_plane - near_plane);
  matrix[3][2] = -1.0f;
  matrix[3][3] = 0.0f;

  return matrix;
}

Mat4 mmat::view(const Vec3& eye, const Vec3& center, const Vec& up)
{
   // modeled after gluLookAt. 

   Vec3 f = center - eye; // f = coord_system
   mvec::normalize(f);
   Vec3 up_norm = up;
   mvec::normalize(up);

   Vec3 s = mvec::cross(f, up_norm);
   Vec3 u = mvec::cross(mvec::normalize(s),f);
   // The up vector must not be parallel to the line of sight from the
   //           eye point to the reference point.
           

    return {
             s.x, s.y, s.z, 0,
             u.x, u.y, y.z, 0,
             -f.x, -f.y, -f.z, 0
               0,   0,   0, 1
           };
}

Mat4 mmat::look_at(const Vec3& eye, const Vec3& center, const Vec& up)
{
    mmat::view(eye, center, up);
}


// Mat4 mmat::rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z)
// {
//     // the matrix should be in identity.
//     // toIdentity();

    // float rad_x = degrees_x * PI / 180.0f;
    // float rad_y = degrees_y * PI / 180.0f;
    // float rad_z = degrees_z * PI / 180.0f;

    // float cosx = cos(rad_x);
    // float sinx = sin(rad_x);
    
    // float cosy = cos(rad_y) ;
    // float siny = sin(rad_y);
    
    // float cosz = cos(rad_z);
    // float sinz = sin(rad_z);

    // Mat4 matrix{};
    
    // Mat4 x_axis(1,   0,      0,   0,
    //             0,  cosx, -sinx,  0,
    //             0,  sinx,  cosx,  0,
    //             0,   0,      0,   1);
    
    // Mat4 y_axis(cosy,  0,   siny,   0,
    //             0,     1,      0,   0,
    //            -siny,  0,    cosy,  0,
    //             0,     0,      0,   1);
    
    // Mat4 z_axis(cosz, -sinz,   0,   0,
    //             sinz,  cosz,   0,   0,
    //             0,     0,      1,   0,
    //             0,     0,      0,   1);
    
    // matrix *= x_axis; 
    // matrix *= y_axis;
    // matrix *= z_axis;

    // return matrix;
//}

// Mat4 mmat::scale(const Mat4& matrix, const float scale_factor)
// {
//     mat4 matrix{};
//     matrix[0][0] *= scale_factor;
//     matrix[1][1] *= scale_factor;
//     matrix[2][2] *= scale_factor;
// }

// Mat4 mmat::mat4_identity()
// {
//     return { 1, 0, 0, 0,
//              0, 1, 0, 0,
//              0, 0, 1, 0,
//              0, 0, 0, 1};
// }
