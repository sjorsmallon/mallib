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
	Rocket = 3,
	COUNT = 4
};

struct Xform_State
{
	glm::vec3 position;
	glm::vec4 orientation;
	float scale;
};

struct Entity
{
    // things we thought about (set by spawn_entity).
    entity_id id;
    Entity_Type type;
    glm::vec3 position;
	glm::vec3 movement_vector;

	// dmg
	bool can_take_damage;
	int32_t health;
	int32_t damage;

	// relation
	entity_id parent_entity;


    // things we need to decide on
    int splash_damage;
    int splash_radius;
    entity_id target_entity;


    char mesh_name[64];
    bool visible;
    generation generation; // this should actually be a counter for this particular entity ID (i.e. what generation is this?) instead of for this particular entity.
    size_t bucket_id;
    bool scheduled_for_destruction; // = false;
    // std::string mesh_name; // hmm, for POD we cannot use something like std::string. maybe const char?
    Mesh_id mesh_id;
    Xform_State xform_state;
};

struct Entity_Manager
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	using Entity_Array = Bucket_Array<Entity, BUCKET_CAPACITY>;

	// owning
	Entity_Array entities{};
	// non-owning
	std::map<Entity_Type, std::vector<bucket*>> buckets_by_type;
	std::vector<Entity*> scheduled_for_destruction;

	uint32_t next_entity_id{0};

	// To be figured out.
	uint32_t player_entity_id{1};
	Entity* player_entity_ptr;
};


struct Entity_Range
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	using entity_iterator = Occupied_Iterator<Entity, BUCKET_CAPACITY>;

	std::vector<bucket*>& buckets;

	Entity_Range(std::vector<bucket*>& buckets_in) : buckets(buckets_in) 
	{}

	// false is for the begin,
	// end is for the end.
	entity_iterator begin() {return {false, buckets};}
	entity_iterator end() {return {true, buckets};}

};


inline uint32_t get_next_entity_id(Entity_Manager& entity_manager)
{
	uint32_t id = entity_manager.next_entity_id;
	entity_manager.next_entity_id += 1;

	return id;
}


inline Entity& get_main_player_entity(Entity_Manager& entity_manager)
{
	assert(entity_manager.player_entity_id != 0);
	assert(entity_manager.player_entity_ptr);

	return *(entity_manager.player_entity_ptr);
}

inline void set_main_player_entity(Entity_Manager& entity_manager, Entity& entity)
{
	entity_manager.player_entity_id = entity.id;
	entity_manager.player_entity_ptr = &entity;
}


//@FIXME(Sjors): unite spawn_entities and create_default_entities.
// Only things currently listed in _sure_ are set.
inline Entity& spawn_entity(Entity_Manager& entity_manager, Entity_Type type)
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
			uint32_t id = get_next_entity_id(entity_manager);
			
			Entity entity{};
			entity.id = id;
			entity.type = type;
			entity.position = glm::vec3(0.0f, id, 0.0f);


			// this is actually an implementational detail. Not very nice.
			entity.bucket_id = target_bucket->bucket_id;


			target_bucket->data[idx] = entity;
			target_bucket->occupied[idx] = true;
			target_bucket->size += 1;
			
			return target_bucket->data[idx];
		}		
	}
}

inline Entity& create_player_entity(Entity_Manager& entity_manager)
{
	auto& player_entity = spawn_entity(entity_manager, Entity_Type::Player);

	player_entity.can_take_damage = true;
	player_entity.health = 100;
	player_entity.damage = 0;
	player_entity.parent_entity = 0; // world and/or server

	player_entity.splash_damage = 0;
	player_entity.splash_radius = 0;
	player_entity.target_entity = 0;

	//@Memory(Sjors):strcpy...
	strcpy(player_entity.mesh_name, "none");
	player_entity.visible = false;
	player_entity.scheduled_for_destruction = false;

	return player_entity;
}



//@FIXME(Sjors): what if it's empty?
inline Entity_Range all_entities(Entity_Manager& entity_manager)
{
	assert(!entity_manager.entities.all_buckets.empty());

	return Entity_Range{entity_manager.entities.all_buckets};	
}

//FIXME(Sjors): what if it's empty?
inline Entity_Range by_type(Entity_Manager& entity_manager, Entity_Type type)
{
	assert(!entity_manager.buckets_by_type[type].empty());
	
	return Entity_Range{entity_manager.buckets_by_type[type]};
}

inline size_t count_by_type(Entity_Manager& entity_manager, Entity_Type entity_type)
{
	size_t type_bucket_count = entity_manager.buckets_by_type[entity_type].size();

	if (!type_bucket_count) return size_t{0};

	size_t entity_count = 0;
	for (auto&& bucket_ptr: entity_manager.buckets_by_type[entity_type])	entity_count += bucket_ptr->size;

	return entity_count;
}



inline void create_default_entity(Entity_Manager& entity_manager, Entity_Type type)
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
			uint32_t id = get_next_entity_id(entity_manager);
			Entity entity{};
			entity.id = id;
			entity.type = type;
			entity.position = glm::vec3(0.0f, id, 0.0f);

			// unsure things go here.
			entity.can_take_damage = true;
			entity.health = 100;
			// this is actually an implementational detail. Not very nice.
			entity.bucket_id = target_bucket->bucket_id;


			target_bucket->data[idx] = entity;
			target_bucket->occupied[idx] = true;
			target_bucket->size += 1;
			break;
		}		
	}
}

inline void schedule_for_destruction(Entity_Manager& entity_manager, Entity* entity)
{
	//@Memory(Sjors): heap allocation
	entity_manager.scheduled_for_destruction.push_back(entity);
}


// destroy scheduled entities, check all buckets whether they are now empty and reconstruct unfull_buckets list.
inline void destroy_scheduled_entities(Entity_Manager&entity_manager)
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;

	for (auto* entity_ptr: entity_manager.scheduled_for_destruction)
	{
		bucket* host_bucket = entity_manager.entities.all_buckets[entity_ptr->bucket_id];
		// size_t count = 0;
		// for (size_t idx =0; idx != host_bucket->capacity; ++idx)
		// {
		// 	if (host_bucket->occupied[idx])
		// 	{
		// 		// logr::report("occupied at idx {}\n", idx);
		// 		count++;
		// 	}
		// }
		// logr::report("occupied slots: {}\n", count);
			
		for (size_t entity_idx = 0; entity_idx != host_bucket->capacity; ++entity_idx)
		{
			Entity* current_entity_ptr = &host_bucket->data[entity_idx];
			if (current_entity_ptr == entity_ptr)
			{
				host_bucket->size -= 1;
				// logr::report("host_bucket->size: {}\n", host_bucket->size);
				// logr::report("count_by_type: {}\n", count_by_type(entity_manager, Entity_Type::Cube));
				// logr::report("bucket_count: {}\n", entity_manager.entities.all_buckets.size());
				
				*entity_ptr = {0};
				host_bucket->occupied[entity_idx] = false;
				entity_ptr->bucket_id = host_bucket->bucket_id;
			}
		}

	}

	// it could be the case that with the entities deleted, some buckets are freed up.
	entity_manager.entities.unfilled_buckets.clear();
	for (auto* bucket_ptr: entity_manager.entities.all_buckets)
	{
		if (bucket_ptr->size != bucket_ptr->capacity) entity_manager.entities.unfilled_buckets.push_back(bucket_ptr);
	}

	//@Memory(Sjors): heap deallocation
	entity_manager.scheduled_for_destruction.clear();
}


#endif