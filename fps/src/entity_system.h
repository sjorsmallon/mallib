#ifndef INCLUDED_ENTITY_SYSTEM_
#define INCLUDED_ENTITY_SYSTEM_
#include <cstdint>
#include <glm/vec3.hpp>
#include <atomic>

using entity_id = uint32_t; 
using generation = uint32_t;

enum Entity_Type : uint8_t
{
	TYPE_PLAYER = 0,
	TYPE_CUBE = 1,
	TYPE_COUNT = 2
};

struct Entity
{
    // things we thought about
    entity_id ID;
    Entity_Type type;
    glm::vec3 position;

    // things we need to decide on
    generation generation; // this should actually be a counter for this particular entity ID (i.e. what generation is this?) instead of for this particular entity.
    bool scheduled_for_destruction;
    bool visible;
};

struct Player : public Entity
{
	glm::vec3 movement_vector;
};

struct Entity_Manager
{
	static std::atomic<uint32_t> next_entity_id;
	// how are they grouped??. I guess.
	// entity manager can iterate over entities.by_type, or by_component, or by_... whatever.
	// don't make it too complicated.	
};

#endif