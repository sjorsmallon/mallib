#include <vector>
#include <iostream>
#include <stdint.h>

struct Entity {
	uint32_t id;
};

struct Vector_Of_Int
{
	std::vector<Entity> entities;

	typedef typename Entity* iterator;

	iterator begin() {return &entities[0];}
	iterator end()	 {return &entities[entities.size()];}
	// iterator operator++(){};
};


int main()
{
	Vector_Of_Int vector_of_int;

	for (uint32_t i : {0,1,2,3,4,5,6,7,8,9})
	{
		vector_of_int.entities.push_back(Entity{i});
	}

	for (auto& entity: vector_of_int)
	{
		std::cerr << entity.id << '\n';
	}

}