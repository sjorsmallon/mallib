#include <thread>
#include <atomic>
#include <stdint.h>

void function_1(Counter& counter)
{
	while(Counter.a != 12000)
	{
		Counter.a += 1;
	}

}

struct Counter
{
	uint32_t a;
	uint32_t b;

}


int main()
{
	auto counter = Counter{};
	Counter counter = {};

	std::atomic<Counter> counter;

	std::thread Thread1 = {};

	std::thread Thread();


	std::thread Thread2 = {};
	std::thread Thread3 = {};
	std::thread Thread4 = {};




}