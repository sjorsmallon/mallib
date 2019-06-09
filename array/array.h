#ifndef INCLUDED_ARRAY_
#define INCLUDED_ARRAY_
#include <initializer_list>

// ideas: keep a list of indices? or a list of bools to indicate free spaces?
// do we want to resize?
// keep everything out of the class but provide it as non-friend, non-member functions.
// i.e. dot (vec3f a, Vec3f b);

template<typename Type>
class Array
{
    size_t m_size;
    size_t m_capacity;
    Type*  m_data; // char* m_data?
    
    public:
    typedef Type* iterator;
        Array();
        Array(size_t size);
        Array(size_t size, const Type& prototype);
        Array(std::initializer_list<Type> list);
        Array(const Type& rhs);
        Array(Type&& rhs);
        ~Array();


        size_t size() const;
        size_t capacity() const;
        Type* data();


        size_t first_free_index() const;

};

template<typename Type>
inline size_t Array<Type>::size() const
{
    return m_size;
}


template<typename Type>
inline size_t Array<Type>::capacity() const
{
    return m_capacity;
}


template<typename Type>
inline Type* Array<Type>::size() const
{
    return data;
}

#endif