#include "entity_system.h"
#include "components.h"



namespace
{

}

void create_player(Entity_Manager& manager)
{
    auto player = manager.registry.create();
    manager.registry.emplace<position>(player, 0.0f,0.0f, 3.0f);
    manager.registry.emplace<velocity>(player, 0.0f,0.0f, 0.0f);
    manager.registry.emplace<player_controlled>(player); // empty type

}
