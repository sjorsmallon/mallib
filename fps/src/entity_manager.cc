#include "entity_manager.h"


//@FIXME(Sjors): what if it's empty?
Entity_Range all_entities(Entity_Manager& entity_manager)
{
	assert(!entity_manager.entities.all_buckets.empty());

	return Entity_Range{entity_manager.entities.all_buckets};	
}

//FIXME(Sjors): what if it's empty?
Entity_Range by_type(Entity_Manager& entity_manager, Entity_Type type)
{
	assert(!entity_manager.buckets_by_type[type].empty());
	
	return Entity_Range{entity_manager.buckets_by_type[type]};
}

size_t count_by_type(Entity_Manager& entity_manager, Entity_Type entity_type)
{
	size_t type_bucket_count = entity_manager.buckets_by_type[entity_type].size();

	if (!type_bucket_count) return size_t{0};

	size_t entity_count = 0;
	for (auto&& bucket_ptr: entity_manager.buckets_by_type[entity_type])	entity_count += bucket_ptr->size;

	return entity_count;
}

Entity& create_entity(Entity_manager& entity_manager, Entity_Type type)
{
	
}

void create_entity(Entity_Manager& entity_manager, Entity_Type type)
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

void schedule_for_destruction(Entity_Manager& entity_manager, Entity* entity)
{
	//@Memory(Sjors): heap allocation
	entity_manager.scheduled_for_destruction.push_back(entity);
}


// destroy scheduled entities, check all buckets whether they are now empty and reconstruct unfull_buckets list.
void destroy_scheduled_entities(Entity_Manager&entity_manager)
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
