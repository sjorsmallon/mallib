#include "../../src/entity_container.h"


int main()
{
	Entity_Manager entity_manager;

	for (int i = 0; i < 100; ++i)
	{
		create_entity(entity_manager, Entity_Type::Cube);
	}


}