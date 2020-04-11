#ifndef SHORT_ALLOC_H
#define SHORT_ALLOC_H

// The MIT License (MIT)
// 
// Copyright (c) 2015 Howard Hinnant
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
#include <cstddef>
#include <cassert>

template <std::size_t size, std::size_t alignment = alignof(std::max_align_t)>
class Arena
{
    alignas(alignment) char buffer[size];
    char* ptr_;

    public:
        ~Arena()
        {
            ptr_ = nullptr;
        }
        Arena() noexcept
        : 
            ptr_(buf_)
        {}

        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;


        template <std::size_t ReqAlign> char* allocate(std::size_t n);
        void deallocate(char* p, std::size_t n) noexcept;

        static constexpr std::size_t size() noexcept
        {
            return size;
        }

        std::size_t used() const noexcept
        {
            return static_cast<std::size_t>(ptr_ - buf_);
        }

        void reset() noexcept
        {
            ptr_ = buf_;
        }

    private:
        
        static std::size_t align_up(std::size_t n) noexcept
        {
            return (n + (alignment-1)) & ~(alignment-1);
        }

        bool pointer_in_buffer(char* p) noexcept
        {
            return buf_ <= p && p <= buf_ + size;
        }
};

template <std::size_t size, std::size_t alignment>
template <std::size_t ReqAlign>
char* Arena<size, alignment>::allocate(std::size_t n)
{
    static_assert(ReqAlign <= alignment, "alignment is too small for this Arena");
    assert(pointer_in_buffer(ptr_) && "short_alloc has outlived Arena");

    auto const aligned_n = align_up(n);

    if (static_cast<decltype(aligned_n)>(buf_ + size - ptr_) >= aligned_n)
    {
        char* r = ptr_;
        ptr_ += aligned_n;
        return r;
    }

    static_assert(alignment <= alignof(std::max_align_t), "you've chosen an "
                  "alignment that is larger than alignof(std::max_align_t), and "
                  "cannot be guaranteed by normal operator new");

    return static_cast<char*>(::operator new(n));
}

template <std::size_t size, std::size_t alignment>
void
Arena<size, alignment>::deallocate(char* p, std::size_t n) noexcept
{
    assert(pointer_in_buffer(ptr_) && "short_alloc has outlived Arena");
    if (pointer_in_buffer(p))
    {
        n = align_up(n);
        if (p + n == ptr_)
            ptr_ = p;
    }
    else
        ::operator delete(p);
}

template <class T, std::size_t size, std::size_t Align = alignof(std::max_align_t)>
class short_alloc
{
public:
    using value_type = T;
    static auto constexpr alignment = Align;
    static auto constexpr size = size;
    using Arena_type = Arena<size, alignment>;

private:
    Arena_type& a_;

public:
    short_alloc(const short_alloc&) = default;
    short_alloc& operator=(const short_alloc&) = delete;

    short_alloc(Arena_type& a) noexcept : a_(a)
    {
        static_assert(size % alignment == 0,
                      "size size needs to be a multiple of alignment Align");
    }
    template <class U>
        short_alloc(const short_alloc<U, size, alignment>& a) noexcept
            : a_(a.a_) {}

    template <class _Up> struct rebind {using other = short_alloc<_Up, size, alignment>;};

    T* allocate(std::size_t n)
    {
        return reinterpret_cast<T*>(a_.template allocate<alignof(T)>(n*sizeof(T)));
    }
    void deallocate(T* p, std::size_t n) noexcept
    {
        a_.deallocate(reinterpret_cast<char*>(p), n*sizeof(T));
    }

    template <class T1, std::size_t size1, std::size_t A1, 
              class U, std::size_t M, std::size_t A2>
    friend
    bool
    operator==(const short_alloc<T1, size1, A1>& x, const short_alloc<U, M, A2>& y) noexcept;

    template <class U, std::size_t M, std::size_t A> friend class short_alloc;
};

template <class T, std::size_t size, std::size_t A1, class U, std::size_t M, std::size_t A2>
inline bool operator==(const short_alloc<T, size, A1>& x, const short_alloc<U, M, A2>& y) noexcept
{
    return size == M && A1 == A2 && &x.a_ == &y.a_;
}

template <class T, std::size_t size, std::size_t A1, class U, std::size_t M, std::size_t A2>
inline bool operator!=(const short_alloc<T, size, A1>& x, const short_alloc<U, M, A2>& y) noexcept
{
    return !(x == y);
}

#endif  // SHORT_ALLOC_H
