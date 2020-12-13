#include <vector>
#include <iostream>
#include <stdint.h>
#include <array>
#include <map>
#include <cassert>

const int BUCKET_CAPACITY = 100;


enum Entity_Type
{
	Player = 0
};

struct Entity {
	uint32_t id;
	Entity_Type entity_type;	
};

template <typename Type>
concept Pod = std::is_standard_layout<Type>::value && std::is_trivial<Type>::value && std::is_aggregate<Type>::value;

template <Pod pod, size_t bucket_capacity>
struct Bucket {
	std::array<pod, bucket_capacity> data;
	std::array<bool, bucket_capacity> occupied;
	Bucket* next = nullptr; //@unused
	size_t bucket_idx = 0;
	size_t size = 0; //
	size_t capacity = bucket_capacity;
};


template<Pod pod, size_t bucket_capacity>
struct Bucket_Array
{
	using bucket = Bucket<pod, bucket_capacity>;
	std::vector<bucket*> all_buckets;
	std::vector<bucket*> unfilled_buckets;

	~Bucket_Array()
	{
		for (size_t idx = 0; idx != all_buckets.size(); ++idx)
		{
			std::cerr << "deleted bucket\n";
			bucket* current = all_buckets[idx];
			delete current;
		}
	}


};

//@Note(Sjors): prefer to not use this & let it be done by the array itself.
template<Pod pod, size_t bucket_capacity>
void add_bucket_by_handle(Bucket_Array<pod, bucket_capacity>&bucket_array, Bucket<pod, bucket_capacity>* new_bucket)
{

	new_bucket->bucket_idx = bucket_array.all_buckets.size();
	
	if (!bucket_array.all_buckets.empty())
	{
		Bucket<pod, bucket_capacity>* last_bucket = bucket_array.all_buckets.back();
		last_bucket->next = new_bucket;
	}

	bucket_array.all_buckets.push_back(new_bucket);
	std::cerr << "added bucket by handle!\n";
}

template<Pod pod, size_t bucket_capacity>
void add_bucket(Bucket_Array<pod, bucket_capacity>& bucket_array)
{
	Bucket<pod, bucket_capacity>* new_bucket = new Bucket<pod, bucket_capacity>();
	new_bucket->bucket_idx = bucket_array.all_buckets.size();
	
	if (bucket_array.all_buckets.size())
	{
		Bucket<pod, bucket_capacity>* last_bucket = bucket_array.all_buckets.back();
		last_bucket->next = new_bucket;
	}

	bucket_array.all_buckets.push_back(new_bucket);
	std::cerr << "added bucket!\n";
};


template<Pod pod, size_t bucket_capacity>
struct Occupied_Iterator
{
	    using iterator_category = std::forward_iterator_tag;
	    using value_type = pod;
	    using difference_type = pod;
	    using pointer = pod*;
	    using reference = pod&;

		using bucket = Bucket<pod, bucket_capacity>;

      	pod* current = nullptr;
      	std::vector<bucket*>& all_buckets;
      	size_t current_bucket_idx = 0;
      	size_t current_item_idx = 0;

    	Occupied_Iterator(pod* pod_in, std::vector<bucket*>& all_buckets_in)
    	:
    		current{pod_in},
    		all_buckets(all_buckets_in)
    	{};

            bool operator== (const Occupied_Iterator& other) {
                return current == other.current;
            }
 
			reference operator*() {return *current;}

            Occupied_Iterator& operator++()
            {
            	current_item_idx += 1;
				current++;
            	// continue skipping unoccupied slots
            	while (all_buckets[current_bucket_idx]->occupied[current_item_idx] == false)
            	{
	            	if (current_item_idx == bucket_capacity)
	            	{
	            		current_bucket_idx += 1;
	            		current_item_idx = 0;

	            		if (current_bucket_idx == all_buckets.size())
	            		{
	            			// set current to END (one past the last bucket.)
	            			current = &all_buckets[all_buckets.size() - 1]->data[bucket_capacity]; 
	            			return *this;
	            		}
	            		current = &all_buckets[current_bucket_idx]->data[current_item_idx];
	            	}
	            	else
	            	{
	            		current++;
	            		current_item_idx += 1;
	            	}

            	}
            	return *this;
            }
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


struct Entity_Manager
{
	using bucket = Bucket<Entity, BUCKET_CAPACITY>;
	using Entity_Array = Bucket_Array<Entity, BUCKET_CAPACITY>;
	using entity_iterator = Occupied_Iterator<Entity, BUCKET_CAPACITY>;

	// owning
	Entity_Array entities;
	// non-owning
	std::map<Entity_Type, std::vector<bucket*>> buckets_by_type;

};

//FIXME(Sjors): what if it's empty?
Entity_Range by_type(Entity_Manager& entity_manager, Entity_Type type)
{
	assert(!entity_manager.buckets_by_type.empty());

	return Entity_Range{&entity_manager.buckets_by_type[type][0]->data[0],
					    &entity_manager.buckets_by_type[type][entity_manager.buckets_by_type[type].size() - 1]->data[BUCKET_CAPACITY],
					    entity_manager.buckets_by_type[type]};
}

//@FIXME(Sjors): what if it's empty?
Entity_Range all_entities(Entity_Manager& entity_manager)
{
	assert(!entity_manager.entities.all_buckets.empty());

	return Entity_Range{&entity_manager.entities.all_buckets[0]->data[0],
					    &entity_manager.entities.all_buckets[entity_manager.entities.all_buckets.size() - 1]->data[BUCKET_CAPACITY],
					    entity_manager.entities.all_buckets};	
}

void create_entity(Entity_Manager& entity_manager, uint32_t id, Entity_Type type)
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
			target_bucket->data[idx] = Entity{id, type};
			target_bucket->occupied[idx] = true;
			target_bucket->size += 1;
			break;
		}		
	}

}


int main()
{
	Entity_Manager entity_manager;

	for (uint32_t i : {0,1,2,3,4,5,6,7,8,9})
	{
		create_entity(entity_manager, i,Entity_Type::Player);
	}


	for (auto& entity: all_entities(entity_manager))
	{
		std::cerr << entity.id << '\n';
	}

	// for (auto& bucket :  entity_manager.entities.all_buckets)
	// {
	// 	for (auto& entity: bucket->data)
	// 	{
	// 		std::cerr << entity.id << '\n';
	// 	}
	// }

}