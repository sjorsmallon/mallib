#ifndef _TEST_ENTITY_CONTAINER_
#define _TEST_ENTITY_CONTAINER_
#include "bucket_array.h"
#include <map>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define macro_string_concatenate_helper(a, b) a ## b
#define macro_string_concatenate(a, b) macro_string_concatenate_helper(a, b)
#define macro_unique_name(var) macro_string_concatenate(var, __LINE__)

using entity_id = uint32_t; 
using generation = uint32_t;
using Mesh_id = size_t;
using Bucket_idx = size_t;
using Entity_idx = size_t;

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
    bool scheduled_for_destruction; // = false
    bool visible; // = false;
    Mesh_id mesh_id;
    Xform_State xform_state;
    // should this contain bucket idx etc etc etc?
    Bucket_idx bucket_idx;
    Entity_idx entity_idx;
};


static const int bucket_size = 100;
using bucket = Bucket<Entity, bucket_size>;
using bucket_array = Bucket_Array<Entity, bucket_size>;

struct Entity_Manager
{
	entity_id next_id = 0;
	bucket_array all_buckets;
	std::map<Entity_Type, std::deque<bucket*>> buckets_by_type; // hmm. this should be an iterator?
	
	std::deque<entity_id> free_ids;
};


inline entity_id get_free_entity_id(Entity_Manager& entity_manager)
{
	if (entity_manager.free_ids.size()) 
	{
		entity_id id = entity_manager.free_ids.front();
		entity_manager.free_ids.pop_front();
		return id;
	}
	entity_id id = entity_manager.next_id;
	entity_manager.next_id += 1;

	return id;
}

//@Thread(Sjors): get_last_bucket is not guaranteed to work like this)
inline void create_entity(Entity_Manager& entity_manager, Entity_Type entity_type)
{
	bucket* target_bucket = nullptr;
	// are there any non-full buckets of this particular type?
	for (auto bucket_ptr:  entity_manager.buckets_by_type[entity_type])
	{
		if (bucket_ptr->size != bucket_ptr->capacity)
		{
			target_bucket = bucket_ptr;
			break;
		}
	}

	// if no bucket found:
	if (nullptr == target_bucket)
	{
		add_bucket(entity_manager.all_buckets);
		target_bucket = get_last_bucket(entity_manager.all_buckets);
		entity_manager.buckets_by_type[entity_type].push_back(target_bucket);
	}

	Entity entity{};
	entity.id = get_free_entity_id(entity_manager);
	entity.type = entity_type;
	entity.xform_state = {glm::vec3{0.f, entity.id, 0.f}, glm::vec4(0.f), 1.f};
	entity.position =     glm::vec3{0.f, entity.id, 0.f};
	entity.bucket_idx = target_bucket->bucket_idx;

	array_add(entity_manager.all_buckets, std::move(entity));
}

inline void schedule_for_destruction(Entity_Manager& entity_manager, Entity* entity)
{
	entity->scheduled_for_destruction = true;
}

inline void destroy_scheduled_entities(Entity_Manager& entity_manager)
{
	//@Speed(Sjors): not finished.
	for (bucket* bucket_ptr: entity_manager.all_buckets.buckets)
	{
		for (size_t idx = 0; idx != bucket_ptr->capacity; ++idx)
		{
			Entity& entity = bucket_ptr->data[idx];
			if (entity.scheduled_for_destruction)
			{
				entity = {};
				bucket_ptr->size -= 1;
			}
			bucket_ptr->occupied[idx] = false;
		}
	}
}





//@Note(Sjors): this macro works in >= c++17 
// #define for_entity_by_type_macro(entity_manager, entity_type, it) \
for (auto [bucket_idx, entity_idx, bucket_list, it] = std::tuple(size_t{0}, size_t{0}, entity_manager.buckets_by_type[entity_type], &entity_manager.buckets_by_type[entity_type][0]->data[0]); it != nullptr; it = next_entity(bucket_list, bucket_idx, entity_idx))

//@Note(Sjors): internal

//@Note(Sjors): returns null for invalid.   // haha bucket list
inline Entity* next_entity(std::deque<bucket*>& bucket_list,  size_t& current_bucket_idx, size_t& current_entity_idx)
{
	Entity* result = nullptr;

	for (size_t bucket_idx = current_bucket_idx; bucket_idx != bucket_list.size(); ++bucket_idx)
	{
		bucket* current_bucket = bucket_list[bucket_idx];

		while(current_entity_idx < current_bucket->capacity - 1)
		{
			current_entity_idx = current_entity_idx + 1;
			if (current_bucket->occupied[current_entity_idx]) 
			{
				result = &current_bucket->data[current_entity_idx];
				return result;
			}
		}

		current_entity_idx = 0;
		current_bucket_idx += 1;
	}

	return result;
}





// @Note(Sjors); this macro works I think in >= C++11.
#define for_entity_by_type_macro(entity_manager, entity_type, it) \
size_t macro_unique_name(current_bucket_idx) = 0; \
size_t macro_unique_name(current_entity_idx) = 0; \
std::deque<bucket*>& macro_unique_name(bucket_list) = entity_manager.buckets_by_type[entity_type]; \
bucket* macro_unique_name(current_bucket) = macro_unique_name(bucket_list)[0]; \
for (Entity* it = &macro_unique_name(current_bucket)->data[0]; 	it != nullptr; 	it = next_entity(macro_unique_name(bucket_list), macro_unique_name(current_bucket_idx), macro_unique_name(current_entity_idx))) 

#endif


