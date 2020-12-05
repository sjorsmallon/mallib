#ifndef INCLUDED_BUCKET_ARRAY_
#define INCLUDED_BUCKET_ARRAY_
#include <array>
#include <deque>
#include <cassert>
#include <iostream>

template <typename Type>
concept Pod = std::is_standard_layout<Type>::value && std::is_trivial<Type>::value && std::is_aggregate<Type>::value;


template<Pod pod, size_t count>
struct Bucket
{
	std::array<bool, count> occupied;
	std::array<pod, count> data;
	size_t size;
	size_t bucket_idx;
	size_t capacity{count};
};

// Adding/removing objects at the end of a std::vector<T> keeps pointers and iterators stable unless the std::vector<T> needs to reallocate its internal structure. That is, pointers and iterators get only invalidated when an object is added and v.size() == v.capacity(). You can use v.reserve(n) to reserve space.
// Adding/removing objects at either end of a std::deque<T> keeps pointers stable but does invalidate iterators.
// Adding/removing objects anywhere in a std::list<T> keeps both pointers and iterators stable.
// cannot use vector,
// CAN use deque (since we do not care about iterators in this case)

template <Pod pod, size_t count_per_bucket>
struct Bucket_Array
{
	size_t count = count_per_bucket;
	std::deque<Bucket<pod, count_per_bucket>*> buckets;
	std::deque<Bucket<pod, count_per_bucket>*> unfilled_buckets;
	// last_bucket;

	Bucket_Array()
	:
		buckets({}),
		unfilled_buckets({})
	{
		// std::cerr << "constructed bucket_array!" << '\n';
	}

	~Bucket_Array()
	{
		for (auto* bucket: buckets)
		{
			delete bucket;
			// std::cerr << "deleted bucket!" << '\n';
		} 
	}
};

template<Pod pod, size_t count_per_bucket>
inline void add_bucket(Bucket_Array<pod, count_per_bucket>& bucket_array)
{
	// std::cerr << "add_bucket" << '\n';
	auto&& bucket = new Bucket<pod, count_per_bucket>();

	bucket_array.buckets.push_back(bucket);
	bucket_array.unfilled_buckets.push_back(bucket);
}


template<Pod pod, size_t count_per_bucket>
inline Bucket<pod, count_per_bucket>& get_unfilled_bucket(Bucket_Array<pod, count_per_bucket>& bucket_array)
{
	// std::cerr << "get_unfilled_bucket" << '\n';
	if (!bucket_array.unfilled_buckets.size()) add_bucket(bucket_array);

	return *bucket_array.unfilled_buckets.front();
}


template<Pod pod, size_t count_per_bucket>
inline void array_add(Bucket_Array<pod, count_per_bucket>& bucket_array, pod&& pod_in)
{
	// std::cerr << "array_add" << '\n';
	// get the FIRST free bucket.
	auto&& bucket = get_unfilled_bucket(bucket_array);

	size_t old_size = bucket.size;

	for (size_t idx = 0; bucket.capacity; ++idx)
	{		
		if (!bucket.occupied[idx])
		{
			bucket.data[idx] = pod_in; 
			bucket.occupied[idx] = true;
			bucket.size +=1;
			break;
		}
	}
		
	std::cerr << "old_size:" << old_size << ", new size: " <<  bucket.size << '\n'; 

	assert(old_size != bucket.size);
	
	if (bucket.size == bucket.capacity)
	{
		// This bucket is full: erase it from the unfilled buckets list.
		for (size_t idx = 0; idx != bucket_array.unfilled_buckets.size(); ++idx)	
		{
			auto&& unfilled_bucket = bucket_array.unfilled_buckets[idx];

			if (unfilled_bucket  == std::addressof(bucket)) 
			{
				bucket_array.unfilled_buckets.erase(bucket_array.unfilled_buckets.begin() + idx);
				break;
			}
		}
	}
}





#endif