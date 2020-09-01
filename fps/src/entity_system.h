#ifndef INCLUDED_ENTITY_SYSTEM_
#define INCLUDED_ENTITY_SYSTEM_
#include <entt/entt.h>

struct Entity_Manager
{
	entt::registry registry;
};

void create_player(Entity_Manager& manager);
void create_lights(Entity_Manager& manager);


#endif