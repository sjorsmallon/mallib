#include "mmat.ih"

Mat4 mmat::translate(const Mat4& matrix, const Vec3& vector)
{
    Mat4 new_matrix{matrix}; // copy construction;

    matrix[3][0] = x; // should be +=? // doesn't work that way!
    matrix[3][1] = y; // should be +=?
    matrix[3][2] = z; // should be +=?

}
Mat4 mmat::scale(const Mat4& matrix, const float scale_factor)
{
    mat4 matrix{};
    matrix[0][0] *= scale_factor;
    matrix[1][1] *= scale_factor;
    matrix[2][2] *= scale_factor;
}

Mat4 mmat::rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z)
{
//     // the matrix should be in identity.
//     // toIdentity();

    float rad_x = degrees_x * PI / 180.0f;
    float rad_y = degrees_y * PI / 180.0f;
    float rad_z = degrees_z * PI / 180.0f;

    float cosx = cos(rad_x);
    float sinx = sin(rad_x);
    
    float cosy = cos(rad_y) ;
    float siny = sin(rad_y);
    
    float cosz = cos(rad_z);
    float sinz = sin(rad_z);

    Mat4 matrix{};
    
    Mat4 x_axis(1,   0,      0,   0,
                0,  cosx, -sinx,  0,
                0,  sinx,  cosx,  0,
                0,   0,      0,   1);
    
    Mat4 y_axis(cosy,  0,   siny,   0,
                0,     1,      0,   0,
               -siny,  0,    cosy,  0,
                0,     0,      0,   1);
    
    Mat4 z_axis(cosz, -sinz,   0,   0,
                sinz,  cosz,   0,   0,
                0,     0,      1,   0,
                0,     0,      0,   1);
    
    matrix *= x_axis; 
    matrix *= y_axis;
    matrix *= z_axis;

    return matrix;
}

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



