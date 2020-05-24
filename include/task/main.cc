#include <iostream>
#include <thread>

#include "task_system.h"
#include <cstdio>
#include <condition_variable>
#include <mutex>

std::atomic<uint32_t> counter;
std::mutex mutex;

using lock_t = std::unique_lock<std::mutex>;

static void increment_atomic_value()
{
		counter++;
}


int main()
{
	int32_t thread_count = std::thread::hardware_concurrency();
    std::cout << thread_count << " concurrent threads are supported.\n";

    Task_System task_system(thread_count);
    task_system.async([](){increment_atomic_value();});
    task_system.async([](){increment_atomic_value();});
    task_system.async([](){increment_atomic_value();});
    task_system.async([](){increment_atomic_value();});

    std::cout << "counter is : " << counter << '\n';

}