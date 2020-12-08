#ifndef INCLUDED_WTHIFWE_
#define INCLUDED_WTHIFWE_
#include <array>
#include <deque>
#include <vector>

// template <typename Iterator>
// class searchable_iterator
// {
//     Iterator wrapped, begin, end;
// public:
//     difference_type     Iterator::difference_type;
//     value_type          Iterator::value_type;
//     pointer             Iterator::pointer;
//     reference           Iterator::reference;
//     iterator_category   Iterator::iterator_category;

//     searchable_iterator(Iterator wrapped, Iterator begin, Iterator end)
//       : wrapped(wrapped), begin(begin), end(end) {}

//     // All the members, calling that member of wrapped (see std::reverse_iterator for guidance)

//     searchable_iterator next_row()
//     {
//         return searchable_iterator(std::find_if(begin, end, [this](const T & elem) { return (elem == 1); }), begin, end);
//     }
// }


struct Vector_of_Int
{
	std::vector<int> data;

}


for (auto &elem: vector_of_int)
{
	
}

// struct Entity{
// 	int id;
// };

// struct Bucket
// {
// 	std::array<Entity, 10> data;
// };


// struct Entity_Manager
// {
// 	std::deque<bucket*> data;


// 	Entity* 

// };



#endif