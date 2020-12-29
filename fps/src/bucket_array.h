#ifndef INCLUDED_BUCKET_ARRAY_
#define INCLUDED_BUCKET_ARRAY_
#include <array>
#include <vector>

template <typename Type>
concept Pod = std::is_standard_layout<Type>::value && std::is_trivial<Type>::value && std::is_aggregate<Type>::value;

template <Pod pod, size_t bucket_capacity>
struct Bucket {
	std::array<pod, bucket_capacity> data;
	std::array<bool, bucket_capacity> occupied;
	// Bucket* next;// = nullptr; //@unused
	size_t bucket_id = 0; // For now, equal to its idx.
	size_t size = 0; // is used to return the "occupied" count without iterating over thte entire bucket.
	size_t capacity{bucket_capacity};
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
			bucket* current = all_buckets[idx];
			delete current;
		}
	}

};

//@Note(Sjors): prefer to not use this & let it be done by the array itself.
template<Pod pod, size_t bucket_capacity>
void add_bucket_by_handle(Bucket_Array<pod, bucket_capacity>&bucket_array, Bucket<pod, bucket_capacity>* new_bucket)
{

	new_bucket->bucket_id = bucket_array.all_buckets.size();
	
	if (!bucket_array.all_buckets.empty())
	{
		// Bucket<pod, bucket_capacity>* last_bucket = bucket_array.all_buckets.back();
		// last_bucket->next = new_bucket;
	}

	bucket_array.all_buckets.push_back(new_bucket);
}

template<Pod pod, size_t bucket_capacity>
void add_bucket(Bucket_Array<pod, bucket_capacity>& bucket_array)
{
	Bucket<pod, bucket_capacity>* new_bucket = new Bucket<pod, bucket_capacity>();
	new_bucket->bucket_id = bucket_array.all_buckets.size();
	
	if (bucket_array.all_buckets.size())
	{
		// Bucket<pod, bucket_capacity>* last_bucket = bucket_array.all_buckets.back();
		// last_bucket->next = new_bucket;
	}

	bucket_array.all_buckets.push_back(new_bucket);
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

    	Occupied_Iterator(bool true_if_end, std::vector<bucket*>& all_buckets_in)
    	:
    		all_buckets{all_buckets_in}
    	{
			current = &all_buckets[all_buckets.size() - 1]->data[bucket_capacity - 1];
			
			if (true_if_end) return;
    		else
    		{
	    		bool found = false;
	    		for (size_t bucket_idx = 0; bucket_idx != all_buckets.size(); ++bucket_idx)
	    		{
					bucket* target_bucket = all_buckets[bucket_idx];

	    			if (found) break;

	    			for (size_t item_idx = 0; item_idx != bucket_capacity; ++item_idx)
	    			{
	    				if (target_bucket->occupied[item_idx])
	    				{
	    					current = &target_bucket->data[item_idx];
							current_bucket_idx = bucket_idx;
							current_item_idx = item_idx;
	    					found = true;
	    					break;
	    				}
	    			}
	    		}
    		}
    		

    	};

            bool operator== (const Occupied_Iterator& other) {
                return current == other.current;
            }
 
			reference operator*() {return *current;}

            Occupied_Iterator& operator++()
            {
				while (true)
				{
					if (current_item_idx < bucket_capacity - 1)
					{
						current_item_idx += 1;
						current++;
						if (all_buckets[current_bucket_idx]->occupied[current_item_idx]) return *this;
					}
					else
					{
						if (current_bucket_idx == all_buckets.size() - 1)
						{
							// set current to END (one past the last bucket.)
							current = &all_buckets[all_buckets.size() - 1]->data[bucket_capacity - 1];
							return *this;
						}
						else
						{
							current_bucket_idx += 1;
							current_item_idx = 0;
							current = &all_buckets[current_bucket_idx]->data[current_item_idx];
							return *this;
						}
					}
				}
				


					
    //        	current_item_idx += 1;
				//current++;
    //        	// continue skipping unoccupied slots
    //        	while (all_buckets[current_bucket_idx]->occupied[current_item_idx] == false)
    //        	{
	   //         	if (current_item_idx == bucket_capacity)
	   //         	{
	   //         		current_bucket_idx += 1;
	   //         		current_item_idx = 0;

	   //         		if (current_bucket_idx == all_buckets.size())
	   //         		{
	   //         			// set current to END (one past the last bucket.)
	   //         			current = &all_buckets[all_buckets.size() - 1]->data[bucket_capacity]; 
	   //         			return *this;
	   //         		}
	   //         		current = &all_buckets[current_bucket_idx]->data[current_item_idx];
	   //         	}
	   //         	else
	   //         	{
	   //         		current++;
	   //         		current_item_idx += 1;
	   //         	}

    //        	}
    //        	return *this;
            }
};



#endif