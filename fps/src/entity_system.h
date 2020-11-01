#ifndef INCLUDED_ENTITY_SYSTEM_
#define INCLUDED_ENTITY_SYSTEM_
#include <cstdint>
#include <glm/vec3.hpp>
#include <atomic>

using entity_id = uint32_t; 

enum Entity_Type : uint8_t
{
	TYPE_PLAYER = 0,
	TYPE_CUBE = 1,
	TYPE_COUNT = 2
};

struct Entity
{
    entity_id ID;
    Entity_Type type;
    glm::vec3 position;
};


struct Entity_Manager
{
	static std::atomic<uint32_t> next_entity_id; 
};

// void register_entity(Entity_Manager& manager, const Entity_Type type);
// void create_lights(Entity_Manager& manager);


#endif