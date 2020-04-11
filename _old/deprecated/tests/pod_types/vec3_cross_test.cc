#include "../mvec/mvec.h"
#include <array>
#include <chrono>
#include <iostream>

using namespace std::chrono;

int main()
{
	const size_t size = 300000;
	std::array<Vec3, size> test1;
	std::array<Vec3, size> test2;
	auto start = high_resolution_clock::now();

	for (size_t idx = 0; idx != size; ++ idx)
	{
		mvec::cross_by_value(test1[idx], test2[idx]);
		//mvec::cross_by_ref(test1[idx], test2[idx]);
	}
	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>(stop - start);

	std::cout << duration.count() << '\n';
}