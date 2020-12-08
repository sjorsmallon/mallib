#include <vector>
#include <iostream>
#include <stdint.h>

struct Bucket {
	std::vector<int> data;
};

struct Vector_Of_Buckets
{
	std::vector<Bucket> buckets;

	typedef typename Bucket* iterator;

	iterator begin() {return &buckets[0];}
	iterator end()	 {return &buckets[buckets.size()];}
	// iterator operator++(){};
};


int main()
{
	Vector_Of_Buckets vector_of_buckets;

	for (uint32_t i : {0,1,2,3,4,5,6,7,8,9})
	{
		Bucket bucket;
		bucket.data.push_back(i);
		vector_of_buckets.buckets.push_back(bucket);
	}

	// for (auto &value : vector_of_buckets)
	// {
	// 	std::cerr << value << '\n';
	// }


	for (auto& bucket: vector_of_buckets)
	{
		std::cerr << "bucket.data[0]: " << bucket.data[0] << '\n';

	}

}