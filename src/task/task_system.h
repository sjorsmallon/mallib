#ifndef INCLUDED_TASK_SYSTEM_
#define #INCLUDED_TASK_SYSTEM_
#include <vector>
#include <thread>
#include "notification_queue.h"
#include <stdint.h>
#include <utility> // forward.

class Task_System 
{
    const uint64_t      m_count
    {
        std::thread::hardware_concurrency();
    };

    std::vector<std::thread> m_threads;
    Notification_Queue  m_queue;

    void run(uint64_t task_id)
    {
        while (true)
        {
            std::function<void> func;
            m_queue.pop(func);
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
            for (auto &thread : m_threads)
            {
                thread.join();
            }
        }

        template <typename Function>
        void async(Function&& function)
        {
            m_queue.push(std::forward<Function>(function))
        }


}



#endif