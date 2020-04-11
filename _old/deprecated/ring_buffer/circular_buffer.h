#include <cstdio>
#include <memory>
#include <mutex>

// a simple ring buffer.
// gets() pops from the latest insertion.
// push() overrides any previous element at that location.
// full() marks whether or not it is full.
// threadsafe because of the mutex.
// but that places the cost on putting stuff in the buffer.

template <class T>
class circular_buffer
{
    std::mutex m_mutex;
    const size_t m_max_size; // capacity?
    std::unique_ptr<T[]> m_buf;

    size_t m_head = 0;
    size_t m_tail = 0;
    bool m_full   = 0;

    public:
        explicit circular_buffer(size_t size)
        :
            m_buf(std::unique_ptr<T[]>(new T[size])),
            m_max_size(size)
        {}

        void put(T item)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_buf[m_head] = item;

            if(m_full)
            {
            m_tail = (m_tail + 1) % m_max_size;
            }

            m_head = (m_head + 1) % m_max_size;

            m_full = m_head == m_tail;
        }

        T get()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if(empty())
            {
              return T(); // What do we want this to do actually?
            }

            //Read data and advance the tail (we now have a free space)
            auto val = m_buf[m_tail];
            m_full = false;
            m_tail = (m_tail + 1) % m_max_size;

            return val;
          }

        void reset()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_head = m_tail;
            m_full = false;
        }

        bool empty() const
        {
            //if head and tail are equal, we are empty
            return (!m_full && (m_head == m_tail));
        }

        bool full() const
        {
            //If tail is ahead the head by 1, we are full
            return m_full;
        }

        size_t capacity() const
        {
            return m_max_size;
        }

        size_t size() const
        {
            size_t size = m_max_size;

            if(!m_full)
            {
              if(m_head >= m_tail)
              {
                size = m_head - m_tail;
              }
              else
              {
                size = m_max_size + m_head - m_tail;
              }
            }

        return size;
        }


};

// int main(void)
// {
//   circular_buffer<uint32_t> circle(10);
//   printf("\n === CPP Circular buffer check ===\n");
//   printf("Size: %zu, Capacity: %zu\n", circle.size(), circle.capacity());

//   uint32_t x = 1;
//   printf("Put 1, val: %d\n", x);
//   circle.put(x);

//   x = circle.get();
//   printf("Popped: %d\n", x);

//   printf("Empty: %d\n", circle.empty());

//   printf("Adding %zu values\n", circle.capacity() - 1);
//   for(uint32_t i = 0; i < circle.capacity() - 1; i++)
//   {
//     circle.put(i);
//   }

//   circle.reset();

//   printf("Full: %d\n", circle.full());

//   printf("Adding %zu values\n", circle.capacity());
//   for(uint32_t i = 0; i < circle.capacity(); i++)
//   {
//     circle.put(i);
//   }

//   printf("Full: %d\n", circle.full());

//   printf("Reading back values: ");
//   while(!circle.empty())
//   {
//     printf("%u ", circle.get());
//   }
//   printf("\n");

//   printf("Adding 15 values\n");
//   for(uint32_t i = 0; i < circle.size() + 5; i++)
//   {
//     circle.put(i);
//   }

//   printf("Full: %d\n", circle.full());

//   printf("Reading back values: ");
//   while(!circle.empty())
//   {
//     printf("%u ", circle.get());
//   }
//   printf("\n");

//   printf("Empty: %d\n", circle.empty());
//   printf("Full: %d\n", circle.full());

//   return 0;
// }
