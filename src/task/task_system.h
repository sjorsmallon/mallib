#ifndef INCLUDED_TASK_SYSTEM_
#define #INCLUDED_TASK_SYSTEM_
#include <vector>
#include <thread>
#include "notification_queue.h"
#include <stdint.h>


class Task_System 
{
    const unsigned      m_count
    {
        std::thread::hardware_concurrency();
    };

    std::vector<thread> m_threads;
    notification_queue  m_queue;

    void run(uint32_t task_id)
    {
        while (true)
        {
            std::function<void> func;
            m_queue.pop(func);
            func();
        }
    }



}



#endif