#ifndef INCLUDED_ENTITY_MANAGER_
#define INCLUDED_ENTITY_MANAGER_
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <atomic>
#include <vector>
#include <map>
#include <deque>

#include "bucket_array.h"

#include <string>
#include "logr.h"

using entity_id = uint32_t; 
using generation = uint32_t;
using Mesh_id = size_t;

enum Entity_Type : uint32_t
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
    // std::string mesh_name;
    generation generation; // this should actually be a counter for this particular entity ID (i.e. what generation is this?) instead of for this particular entity.
    bool scheduled_for_destruction = false;
    bool visible;
    Mesh_id mesh_id;
    Xform_State xform_state;
};

struct Entity_Manager
{
	uint32_t next_entity_id;
	std::deque<uint32_t> free_ids;
	std::vector<std::vector<Entity>> entities      =  std::vector<std::vector<Entity>>(Entity_Type::COUNT);
	std::vector<std::vector<Entity*>> active_entities = std::vector<std::vector<Entity*>>(Entity_Type::COUNT);

	//@TODO(Sjors): this should be a single vector.
	std::vector<std::deque<entity_id>> free_indices = std::vector<std::deque<entity_id>>(Entity_Type::COUNT);

	//@TODO(Sjors): this should actually not be a pointer, but type and id
	std::vector<Entity*> scheduled_for_destruction;
};

void make_entities_active(Entity_Manager& entity_manager);


void create_entity(Entity_Manager& entity_manager, Entity_Type type);
std::vector<Entity>& by_type(Entity_Manager& entity_manager, Entity_Type type);

std::vector<Entity*>& by_type_ptr(Entity_Manager& entity_manager, Entity_Type type);

inline void schedule_for_destruction(Entity_Manager& entity_manager, Entity* entity)
{
	entity->scheduled_for_destruction = true;
	entity_manager.scheduled_for_destruction.push_back(entity);
}

inline void destroy_scheduled_entities(Entity_Manager& entity_manager)
{
	logr::report_warning_once("[entity_manager] destroy_scheduled_entites: implementation unfinished!\n");
	auto&& cubes = entity_manager.active_entities[Entity_Type::Cube];
	cubes.erase(std::remove_if(cubes.begin(), cubes.end(),
		[&](Entity* entity) -> bool
		{
			for (auto* marked_entity : entity_manager.scheduled_for_destruction)
			{
				if (entity->id == marked_entity->id) return true;
			}
			return false;
		}), cubes.end());

	for (auto* marked_entity : entity_manager.scheduled_for_destruction) 
			entity_manager.free_indices[marked_entity->type].push_front(marked_entity->id);

	entity_manager.scheduled_for_destruction.clear();
}

// std::vector<Entity*> by_type(Entity_Manager& manager, Entity_Type type);  


#endif