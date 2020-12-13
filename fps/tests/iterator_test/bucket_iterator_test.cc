#include <vector>
#include <iostream>
#include <stdint.h>
#include <array>

const int bucket_capacity = 100;

enum Entity_Type
{
	Player = 0
};


struct Entity {
	uint32_t id;
};

struct Bucket {
	std::array<Entity, bucket_capacity> data;
	std::array<bool, bucket_capacity> occupied;
	Bucket* next = nullptr;
	size_t bucket_idx = 0;
};


struct Active_Entity_Iterator
{
	    using iterator_category = std::forward_iterator_tag;
	    using value_type = Entity;
	    using difference_type = Entity;
	    using pointer = Entity*;
	    using reference = Entity&;

      	Entity* current = nullptr;
      	std::vector<Bucket*>& all_buckets;
      	size_t current_bucket_idx = 0;
      	size_t current_entity_idx = 0;

    	Active_Entity_Iterator(Entity* entity, std::vector<Bucket*>& all_buckets_in) :   current{entity}, all_buckets(all_buckets_in) {};

            bool operator== (const Active_Entity_Iterator& other) {
                return current == other.current;
            }
 
			reference operator*() {return *current;}

            Active_Entity_Iterator& operator++ () {

            	current_entity_idx += 1;

            	if (current_entity_idx == bucket_capacity)
            	{
            		current_bucket_idx += 1;
            		current_entity_idx = 0;

            		if (current_bucket_idx == all_buckets.size())
            		{
            			// set current to END (one past the last bucket.)
            			current = &all_buckets[all_buckets.size() - 1]->data[bucket_capacity]; 
            			return *this;
            		}

            		current = &all_buckets[current_bucket_idx]->data[current_entity_idx];
            	}
            	else
            	{
            		current++;
            	}
  			

            	return *this;
            }
};


struct Bucket_Array
{
	std::vector<Bucket*> all_buckets;

	Active_Entity_Iterator begin()   {return  {&(all_buckets[0]->data[0]), all_buckets};}
	Active_Entity_Iterator end()	 {return  {&all_buckets[all_buckets.size() - 1]->data[bucket_capacity], all_buckets};}
};


void add_bucket(Bucket_Array& bucket_array)
{
	Bucket* bucket = new Bucket();
	bucket->bucket_idx = bucket_array.all_buckets.size();
	
	if (bucket_array.all_buckets.size())
	{
		Bucket* last_bucket = bucket_array.all_buckets.back();
		last_bucket->next = bucket;
	}

	bucket_array.all_buckets.push_back(bucket);
	std::cerr << "added bucket!\n";
};

void create_entity(Bucket_Array& bucket_array, uint32_t id)
{
	auto& bucket = bucket_array.all_buckets[0];

	for (size_t idx = 0; idx != bucket_capacity; ++idx)
	{
		if (!bucket->occupied[idx]) 
		{
			bucket->data[idx] = Entity{id};
			bucket->occupied[idx] = true;
			break;
		}		
	}

}


int main()
{
	Bucket_Array bucket_array;
	add_bucket(bucket_array);


	for (uint32_t i : {0,1,2,3,4,5,6,7,8,9})
	{
		create_entity(bucket_array, i);
	}


	for (auto& value: bucket_array)
	{
		std::cerr << value.id << '\n';
	}

}