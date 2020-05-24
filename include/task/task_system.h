#ifndef INCLUDED_TASK_SYSTEM_
#define INCLUDED_TASK_SYSTEM_
#include <vector>
#include <thread>
#include "notification_queue.h"
#include <stdint.h>
#include <utility> // forward.
#include <iostream>
#include <atomic>
//
//  Task System based on the Better Concurrency talk by Sean Parent
//

class Task_System
{
    uint64_t thread_count{0};
    std::vector<std::thread> threads;
    std::atomic<uint32_t> index{0};
    std::vector<Notification_Queue> notification_queues;

    void run(const uint32_t task_idx)
    {
        while (true)
        {
            std::function<void()> function;
            uint32_t target_notification_idx = 0;
            for (uint32_t thread_idx = 0; thread_idx != thread_count; ++thread_idx)
            {
                target_notification_idx = (task_idx + thread_idx) % thread_count;
                if (notification_queues[target_notification_idx].try_pop(function))
                {
                    break;
                }
            }
            if (!function && !notification_queues[task_idx].pop(function))
            {
                break;
            }
            function();
        }
    }

    public:
        Task_System() = delete;

        Task_System(uint64_t target_thread_count)
        :
            thread_count(target_thread_count),
            threads(target_thread_count),
            notification_queues(target_thread_count)
        {
            for (int32_t thread_idx = 0; thread_idx != target_thread_count; thread_idx++)
            {
                threads.emplace_back([&, thread_idx]{ run(thread_idx); });
            }
        }

        ~Task_System()
        {
            for (auto& queue: notification_queues)
            {
                queue.done();
            }

            for (auto& thread: threads)
            {
                thread.join();
            }

        }

        template<typename Function>
        void async(Function&& function)
        {
            auto my_index = index++;

            const uint64_t spin_factor = 2;
            const uint64_t max_retries = thread_count * spin_factor;
            uint64_t target_notification_idx = 0;
            for (size_t task_index = 0; task_index != max_retries; ++task_index)
            {
                target_notification_idx = (my_index + task_index) % thread_count;
                if (notification_queues[target_notification_idx].try_push(std::forward<Function>(function)))
                {
                        return;
                }
            }
            notification_queues[my_index % thread_count].push(std::forward<Function>(function));
        }

};

#endif