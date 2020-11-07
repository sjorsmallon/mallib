#ifndef INCLUDED_TASK_SYSTEM_
#define INCLUDED_TASK_SYSTEM_

#include <thread>
#include <iostream>
#include <atomic_queue/atomic_queue.h>
#include <functional>
#include <vector>
#include <cstdint>
#include <mutex>

#include <fstream>

#include <iostream>
#include <sstream>
#include <mutex>
#include <future>

// @Help: I think this can still "deadlock" some tasks.
// if thread a is doing task 0, and the task system delegates a task to that queue (task 1)
// AND thread b is busy waiting for its own queue to be filled, task 1 will not be finished
// until task 0 is finished, while thread b is doing nothing. 

// The only way I can think of to prevent this is to verify whether any of the other threads 
// are currently busy waiting before inserting (i.e. notify_one or something like that).

using Task = std::function<void()>;
using Queue = atomic_queue::AtomicQueue2<Task, 16>;

struct Task_System
{
	int32_t num_queues;
	std::vector<Queue> queues;
	std::vector<std::thread> workers;
	std::atomic<uint32_t> target_queue_idx;

	Task_System(const uint32_t thread_count)
	:
		queues(thread_count),
		workers(thread_count)
	{
		for (int thread_idx = 0; thread_idx != thread_count; ++thread_idx)
		{
			workers[thread_idx] = std::thread([=](){
				run(queues, thread_idx);
			});
		}
	}

	~Task_System()
	{
		for (auto& worker: workers)
		{
			worker.join();
		}
	}

	private:
		void run(std::vector<Queue>& task_queues, unsigned int queue_idx);
};


void add_task(Task_System& task_system, Task&& task);




#define terr ThreadStream(std::cerr)
#define tout ThreadStream(std::cout)

/**
 * Thread-safe std::ostream class.
 *
 * Usage:
 *    tout << "Hello world!" << std::endl;
 *    terr << "Hello world!" << std::endl;
 */
class ThreadStream : public std::ostringstream
{
    public:
        ThreadStream(std::ostream& os) : os_(os)
        {
            // copyfmt causes odd problems with lost output
            // probably some specific flag
//            copyfmt(os);
            // copy whatever properties are relevant
            imbue(os.getloc());
            precision(os.precision());
            width(os.width());
            setf(std::ios::fixed, std::ios::floatfield);
        }

        ~ThreadStream()
        {
            std::lock_guard<std::mutex> guard(_mutex_threadstream);
            os_ << this->str();
        }

    private:
        static std::mutex _mutex_threadstream;
        std::ostream& os_;
};



#endif