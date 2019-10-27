#include "../mat4/mat4.h"
#include "../mat3/mat3.h"
#include "../vec3/vec3.h"
#include "../vec2/vec2.h"
#include <iostream>
#include <type_traits>

int main()
{
    std::cout << std::is_trivial<Mat4>::value
              << std::is_standard_layout<Mat4>::value
              << std::is_trivially_copyable<Mat4>::value
              <<  '\n';

    std::cout << std::is_trivial<Vec3>::value
              << std::is_standard_layout<Vec3>::value
              << std::is_trivially_copyable<Vec3>::value
              << '\n';

    std::cout << std::is_trivial<Vec2>::value
              << std::is_standard_layout<Vec2>::value
              << std::is_trivially_copyable<Vec2>::value
              << '\n';

    std::cout << std::is_trivial<Mat3>::value
              << std::is_standard_layout<Mat3>::value
              << std::is_trivially_copyable<Mat3>::value
              << '\n';

}