#ifndef INCLUDED_ENTITY_MANAGER_
#define INCLUDED_ENTITY_MANAGER_
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <atomic>
#include <vector>
#include <deque>
#include <string>

using entity_id = uint32_t; 
using generation = uint32_t;
using Mesh_id = size_t;

enum Entity_Type : uint8_t
{
	None = 0,
	Player = 1,
	Cube = 2,
	COUNT = 3
};

struct Xform_State
{
	glm::vec3 position;
	glm::vec4 orientation;
	float scale;
};

// don't declare any virtual functions please.
struct Entity
{
    // things we thought about
    entity_id id;
    Entity_Type type;
    glm::vec3 position;
	glm::vec3 movement_vector;

    // things we need to decide on
    std::string mesh_name;
    generation generation; // this should actually be a counter for this particular entity ID (i.e. what generation is this?) instead of for this particular entity.
    bool scheduled_for_destruction;
    bool visible;
    Mesh_id mesh_id;
    Xform_State xform_state;
};

struct Entity_Manager
{
	std::atomic<uint32_t> next_entity_id;
	std::deque<uint32_t> free_ids;
	std::deque<uint32_t> free_indices;
	std::vector<Entity> entities;

};



void create_entity(Entity_Manager& entity_manager, Entity_Type type);


#endif