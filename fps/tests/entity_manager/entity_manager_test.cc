#include "../../src/entity_container.h"
#include <tuple>

int main()
{
	Entity_Manager entity_manager;

	for (int i = 0; i < 300; ++i)
	{
		create_entity(entity_manager, Entity_Type::Cube);
	}

	std::cerr << "num buckets: " << entity_manager.all_buckets.buckets.size() << '\n';
	
	for_entity_by_type_macro(entity_manager, Entity_Type::Cube)
	{
		std::cerr << "it.position: " << it->position.x << it->position.y  << it->position.z  << '\n';
	}



}