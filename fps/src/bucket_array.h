#ifndef INCLUDED_BUCKET_ARRAY_
#define INCLUDED_BUCKET_ARRAY_
#include <array>
#include <deque>
#include <iostream>

template<typename Type, size_t count>
struct Bucket
{
	std::array<Type, count> data;
};

// Adding/removing objects at the end of a std::vector<T> keeps pointers and iterators stable unless the std::vector<T> needs to reallocate its internal structure. That is, pointers and iterators get only invalidated when an object is added and v.size() == v.capacity(). You can use v.reserve(n) to reserve space.
// Adding/removing objects at either end of a std::deque<T> keeps pointers stable but does invalidate iterators.
// Adding/removing objects anywhere in a std::list<T> keeps both pointers and iterators stable.
// cannot use vector,
// CAN use deque (since we do not care about iterators in this case)

template <typename Type, size_t count_per_bucket>
struct Bucket_Array
{
	size_t count = count_per_bucket;
	std::deque<Bucket<Type, count_per_bucket>*> buckets;
	std::deque<Bucket<Type, count_per_bucket>*> unfilled_buckets;

	// template<typename Type, size_t count_per_bucket>
	Bucket_Array()
	:
		buckets({}),
		unfilled_buckets({})
	{
		std::cerr << "constructed bucket!" << '\n';
	}


	~Bucket_Array()
	{
		for (auto* bucket: buckets)
		{
			delete bucket;
			std::cerr << "deleted bucket!" << '\n';
		} 
	}
};

template<typename Type, size_t count_per_bucket>
inline void add_bucket(Bucket_Array<Type, count_per_bucket>& bucket_array)
{
	auto&& bucket = new Bucket<Type, count_per_bucket>();
	bucket_array.buckets.push_back(bucket);
	std::cerr << "bucket created!" << '\n';
}

template<typename Type, size_t count_per_bucket>
inline void array_add(Bucket_Array<Type, count_per_bucket>& bucket_array, Type& type)
{

}



#endif