#ifndef INCLUDED_TASK_SYSTEM_
#define #INCLUDED_TASK_SYSTEM_
#include <vector>
#include <thread>
#include "notification_queue.h"
#include <stdint.h>
#include <utility> // forward.

//
//  Task System based on the Better Concurrency talk by Sean Parent
//

class Task_System
{
    const uint64_t m_count{std::thread::hardware_concurrency()};

    std::vector<std::thread> m_threads;
    std::vector<Notification_Queue> m_queues{count};
    std::atomic<uint32_t> m_index{0};
    Notification_Queue  m_queues;

    void run(uint64_t queue_index)
    {
        while (true)
        {
            std::function<void> func;
            for (size_t thread_idx = 0; thread_idx != m_count; ++thread_idx)
            {
                if (m_queue.at((queue_index + thread_idx) % m_count).try_pop(func))
                {
                    break;
                }
            }
            if (!func && !m_queue.at(queue_index).pop(f))
            {
                break;
            }
            func();
        }
    }

    public:
        Task_System() 
        {
            for (uint64_t count = 0; count != m_count; ++count)
            {
                m_threads.emplace_back([&,count]{ run(count)});
            }
        }
        ~Task_System()
        {
            for (auto& queue: m_queues)
            {
                queue.done();
            }
            for (auto& thread : m_threads)
            {
                thread.join();
            }
        }

        template <typename Function>
        void async(Function&& func)
        {
            m_index += 1;
            auto index  = m_index;
            size_t spin_factor = 2;
            size_t max_count = spin_factor * m_count;

            // thread_idx is an unsuitable name.
            for (size_t count = 0; count != max_count; ++count)
            {
                if (m_queue.at((index  + count) % m_count).try_push(std::forward<Function>(function)))
                {
                    return;
                }
            }

            m_queues.at(index % m_count).push(std::forward<Function>(func));
        }


}



#endif