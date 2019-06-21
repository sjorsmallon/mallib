#include "../mvec/mvec.h"
#include "../mmat/mmat.h"
#include <iostream>
#include <type_traits>

int main()
{
	// std::cout << std::is_standard_layout<Vec3>::value << '\n';
	// std::cout << std::is_trivial<Vec3>::value << '\n';
	// std::cout << std::is_pod<Vec3>::value << '\n';

	std::cout << std::is_pod<Mat4>::value << '\n';
	std::cout << std::is_standard_layout<Mat4>::value << '\n';
	std::cout << std::is_trivial<Mat4>::value << '\n';

	Vec3 test = {};
	std::cout << test.x << test.y << test.z << '\n';
	Vec3 rhs = {1,2,3};
	test += rhs;
	std::cout << test.x << test.y << test.z;

}