// #include <mat.ih"

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


// Mat4 mat::rotate(const Mat4& matrix, const int degrees_x, const int degrees_y, const int degrees_z)
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
