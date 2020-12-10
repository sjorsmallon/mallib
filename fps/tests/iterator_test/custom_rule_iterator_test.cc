#include <vector>
#include <iostream>
#include <stdint.h>

struct Entity {
	uint32_t id;
	bool active;
};

// struct Active_Entity_Iterator
// {
// 	Entity* current = nullptr;
// 	Entity* begin;
// 	Entity* end;
// 	Active_Entity_Iterator(Entity* begin_in, Entity* end_in): current(begin_in), begin(begin_in), end(end_in) {};
// 	Entity* operator++() {
// 		std::cerr << "calling active_entity_iterator's operator++!\n";
// 		while (! (current->active) &&  current != end)
// 		{
// 			current++;
// 		}
// 		return current;
// 	}
// };


struct Vector_Of_Int
{
	std::vector<Entity> entities;

	struct Active_Entity_Iterator {

	    using iterator_category = std::forward_iterator_tag;
	    using value_type = Entity;
	    using difference_type = Entity;
	    using pointer = Entity*;
	    using reference = Entity&;
      	
      	Entity* current;

    	Active_Entity_Iterator(Entity* c) : current{c} {};

            bool operator== (const Active_Entity_Iterator& other) {
                return current == other.current;
            }
            // Entity& operator*() {
            //     return *(current->data);
            // }
            // ... some other operators. For more details, see:
            // http://en.cppreference.com/w/cpp/iterator
			reference operator*() {return *current;}

            Active_Entity_Iterator& operator++ () {
            	std::cerr << "invoking this operator++\n";
                current++;
                return *this;
            }
    };

	Active_Entity_Iterator begin()   {return {&entities[0]};}
	Active_Entity_Iterator end()	 {return  {&entities[entities.size()]};}
	// iterator operator++(){};
};



int main()
{
	Vector_Of_Int vector_of_int;

	for (uint32_t i : {0,1,2,3,4,5,6,7,8,9})
	{
		static bool active = false;
		active = 1 - active;
		vector_of_int.entities.push_back(Entity{i, active});
	}


	for (auto& value: vector_of_int)
	{
		std::cerr << value.id << '\n';
	}

}