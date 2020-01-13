#include "containers.h"
#include <iostream>

int main()
{
	Auto_Array<int> tesrer(1);
	std::cout << "heap size: " << mem::active_memory() << '\n';
	Auto_Array<int> test(256);
	std::cout << "heap size: " << mem::active_memory() << '\n';
	String trgerger;
	trgerger.reserve(200);
	std::cout << "heap size: " << mem::active_memory() << '\n';
	return 0;
}

