#ifndef INCLUDED_NOTIFICATION_QUEUE_
#define INCLUDED_NOTIFICATION_QUEUE_
#include <deque>
#include <mutex>
#include <condition_variable>
#include <utility> //forward

using lock_t = std::unique_lock<std::mutex>;

class Notification_Queue 
{
    std::deque<std::function<void()>> task_queue;
    std::mutex                   mutex;
    std::condition_variable      ready; 
    bool                         task_done{false}; 

    public:
        Notification_Queue() = default;
        ~Notification_Queue() = default;

        bool try_pop(std::function<void()>& function)
        {
            lock_t lock{mutex, std::try_to_lock};
            if (!lock || task_queue.empty())
            {
                return false;
            }
            function = std::move(task_queue.front());
            task_queue.pop_front();
            return true;
        }

        template<typename Function>
        bool try_push(Function&& function)
        {
            {                
                lock_t lock{mutex, std::try_to_lock};
                if (!lock)
                {
                    return false;
                }

                task_queue.emplace_back(std::forward<Function>(function));
            } // lock disappears here.

            ready.notify_one();
            return true;
        }  

        void done()
        {
            {
                lock_t lock{mutex};
                task_done = true;
            } // lock expires here.

            ready.notify_all();
        }

        bool pop(std::function<void()>& function)
        {
            lock_t lock{mutex};

            while (task_queue.empty() && !task_done)
            {
                ready.wait(lock);
            } 
            if (task_queue.empty())
            {
                return false;
            }

            function = std::move(task_queue.front());
            task_queue.pop_front();

            return true;
        } // lock expires here.


        template<typename Function>
        void push(Function&& function) {
            {
                lock_t lock{mutex};
                task_queue.emplace_back(std::forward<Function>(function));
            }
            ready.notify_one();
        }
};



#endif