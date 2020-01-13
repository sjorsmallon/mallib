#ifndef INCLUDED_CONTAINERS_
#define INCLUDED_CONTAINERS_
#include "memory.h"
#include <vector>
#include <array>
#include <cstddef>

template <class Type>
using Auto_Array = std::vector<Type, mem::Mallocator<Type>>;

using String = std::basic_string<char, std::char_traits<char>, mem::Mallocator<char>>;

// template <class Type_2, size_t N>
// using Array = std::array<Type_2, N,mem::Mallocator<Type_2>>;



#endif