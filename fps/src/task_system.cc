#include "task_system.h"

constexpr const int SPIN_FACTOR = 2;
std::mutex ThreadStream::_mutex_threadstream{};

void Task_System::run(std::vector<Queue>& task_queues, unsigned int queue_idx)
{
	const auto num_queues = task_queues.size();

	while (true)
	{
		//@IC(Sjors): duh, the task needs to be zero'd after the loop.
		// Instead of declaring it outside of the while loop, it's easier
		// to instantiate a new one.
		std::function<void()> task; 

		// cautiously test without busy waiting to see if there is work to be done.
		for (int32_t queue_offset = 0; queue_offset != num_queues; ++queue_offset)
		{
			uint32_t target = (queue_idx + queue_offset) % num_queues;
			if (task_queues[target].try_pop(task)) break;
		}

		// nothing available: busy wait with pop on "own" queue.
		// pop _ALWAYS_ returns eventually.
		if (!task) 
		{
			task = task_queues[queue_idx].pop();
		}

		// this should _NEVER_ happen.
		if (!task) break;

		// we obtained something valid: execute it.
		task();

	}

}


void add_task(Task_System& task_system, Task&& task)
{
	const auto target_queue_index = task_system.target_queue_idx++;
	const uint32_t thread_count = static_cast<uint32_t>(task_system.workers.size());
	const auto spin_factor = uint32_t{SPIN_FACTOR};
	const uint32_t max_retries = thread_count * spin_factor;

	// try to insert the task in any of the queues, starting with the "next available" thread.
	for (uint32_t thread_offset = 0; thread_offset != spin_factor; ++thread_offset)
	{
		if (task_system.queues[(target_queue_index + thread_offset) % thread_count].try_push(task) )
			return;
	}

	// No success trying to push back into any of the other queues: just block until we succeed.
	const auto queue_index = target_queue_index % thread_count;
	task_system.queues[queue_index].push(task);
}

