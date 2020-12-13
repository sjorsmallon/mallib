#ifndef INCLUDED_ENTITY_MANAGER_
#define INCLUDED_ENTITY_MANAGER_
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <stdint.h>
#include <array>
#include <map>
#include <cassert>
#include "logr.h"
#include "bucket_array.h"

using entity_id = uint32_t; 
using generation = uint32_t;
using Mesh_id = size_t;

const size_t BUCKET_CAPACITY = 100;

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
    bool scheduled_for_destruction; // = false;
    bool visible;
    Mesh_id mesh_id;
    Xform_State xform_state;
};

struct Entity_Manager
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	using Entity_Array = Bucket_Array<Entity, BUCKET_CAPACITY>;

	// owning
	Entity_Array entities;
	// non-owning
	std::map<Entity_Type, std::vector<bucket*>> buckets_by_type;

	uint32_t next_entity_id = 0;
};

struct Entity_Range
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	using entity_iterator = Occupied_Iterator<Entity, BUCKET_CAPACITY>;

	Entity* begin_ptr = nullptr;
	Entity* end_ptr = nullptr;

	std::vector<bucket*>& buckets;

	Entity_Range(Entity* begin_in, Entity* end_in, std::vector<bucket*>& buckets_in) : begin_ptr(begin_in), end_ptr(end_in), buckets(buckets_in) 
	{}

	entity_iterator begin() {return {begin_ptr, buckets};}
	entity_iterator end() {return {end_ptr, buckets};}

};


//FIXME(Sjors): what if it's empty?
inline Entity_Range by_type(Entity_Manager& entity_manager, Entity_Type type)
{
	assert(!entity_manager.buckets_by_type.empty());

	return Entity_Range{&entity_manager.buckets_by_type[type][0]->data[0],
					    &entity_manager.buckets_by_type[type][entity_manager.buckets_by_type[type].size() - 1]->data[BUCKET_CAPACITY],
					    entity_manager.buckets_by_type[type]};
}

//@FIXME(Sjors): what if it's empty?
inline Entity_Range all_entities(Entity_Manager& entity_manager)
{
	assert(!entity_manager.entities.all_buckets.empty());

	return Entity_Range{&entity_manager.entities.all_buckets[0]->data[0],
					    &entity_manager.entities.all_buckets[entity_manager.entities.all_buckets.size() - 1]->data[BUCKET_CAPACITY],
					    entity_manager.entities.all_buckets};	
}

inline void create_entity(Entity_Manager& entity_manager, Entity_Type type)
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	
	bucket* target_bucket = nullptr;
	
	// are there any non-full buckets of this particular type?
	for (auto bucket_ptr:  entity_manager.buckets_by_type[type])
	{
		if (bucket_ptr->size != bucket_ptr->capacity)
		{
			target_bucket = bucket_ptr;
			break;
		}
	}

	// are there any buckets of this entity type?
	// if no bucket found:
	if (nullptr == target_bucket)
	{
		//@memory
		target_bucket = new bucket();
		add_bucket_by_handle(entity_manager.entities, target_bucket); // handoff
		entity_manager.buckets_by_type[type].push_back(target_bucket);
	}

	for (size_t idx = 0; idx != BUCKET_CAPACITY; ++idx)
	{
		if (!target_bucket->occupied[idx]) 
		{
			uint32_t id = entity_manager.next_entity_id;
			entity_manager.next_entity_id += 1;
			Entity entity{};
			entity.id = id;
			entity.type = type;
			entity.position = glm::vec3(0.0f, id, 0.0f);

			target_bucket->data[idx] = entity;
			target_bucket->occupied[idx] = true;
			target_bucket->size += 1;
			break;
		}		
	}
}

inline void schedule_for_destruction(Entity_Manager& entity_manager, Entity* entity)
{
	logr::report_warning_once("[schedule_for_destruction] not implemented.\n");
}


#endif