#include "mat4.h"

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
