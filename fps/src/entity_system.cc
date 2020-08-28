#include "entity_system.h"
#include "components.h"

namespace
{

}

//@dependencies: registry
void create_player(entt::registry& registry)
{
    auto player = registry.create();
    registry.emplace<position>(player, 0.0f,0.0f, 10.0f);
    registry.emplace<velocity>(player, 0.0f,0.0f, 0.0f);
    registry.emplace<player_controlled>(player); // empty type

}
//@dependencies: registry
void create_lights(entt::registry& registry)
{
    for (size_t idx = 0; idx != 32; ++idx)
    {
        auto light = registry.create();
        registry.emplace<position>( light, 0.0f, 0.0f, 0.0f);
        registry.emplace<color_rgb>(light, 0.0f, 0.0f, 0.0f);
    }
}
