#ifndef INCLUDED_NOTIFICATION_QUEUE_
#define #INCLUDED_NOTIFICATION_QUEUE_
#include <deque>
#include <mutex>
#include <condition_variable>
#include <utility> //forward
using lock_t = unique_lock<mutex>;

class Notification_Queue 
{
    std::deque<function<void()>> m_queue;
    std::mutex                   m_mutex;
    std::condition_variable      m_ready; 
    bool                         m_done{false}; 

    public:
        void done()
        {
            {
                lock_t lock{m_mutex};
                m_done = true;
            }
            m_ready.notify_all();
        }

        bool try_pop(function<void()>& func)
        {
            lock_t lock{m_mutex, try_to_lock};
            if (!lock || m_queue.empty())
            {
                return false;
            }j
            func = move(m_queue.front());
            m_queue.pop_front();
            return true;
        }

        template<typename Function>
        bool try_push(Function&& function)
        {
            {                
                lock_t lock{m_mutex, try_to_lock};
                if (!lock)
                {
                    return false;
                }
                m_queue.emplace_back(std::forward<Function>(function));
            } // lock disappears here.
            m_ready.notify_one();
            return true;
        }  


}



#endif