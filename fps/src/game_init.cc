#include "game.h"

namespace
{

}

//@Once
void game_init(Entity_Manager &entity_manager)
{
    for (size_t idx = 0; idx != 10; ++idx)
    {
        create_default_entity(entity_manager, Entity_Type::Cube);
    }

    auto& player_entity = create_player_entity(entity_manager);
    set_main_player_entity(entity_manager, player_entity);

}   	
