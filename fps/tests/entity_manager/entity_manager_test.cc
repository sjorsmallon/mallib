#include "../../src/entity_container.h"


int main()
{
	Entity_Manager<100> entity_manager;

	create_entity(entity_manager, Entity_Type::Cube);
}