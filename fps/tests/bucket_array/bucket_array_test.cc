#include "../../src/bucket_array.h" 
#include <vector>
#include <iostream>
struct AABB
{
	float A;
	float a;
	float B;
	float b;		
};

struct Pointer_Struct
{
	char* test;
};


struct Member_Should_Fail
{
	std::vector<int> member;
};

struct Constructor_Should_Fail
{
	int x;
	Constructor_Should_Fail(int _x)
	: 
		x(_x)
	{}
};

struct Initialized_Struct_Should_Fail
{
	char* test = nullptr;	
};


int main()
{
	// Bucket_Array<Member_Should_Fail, size> member_should_fail = Bucket_Array<		Member_Should_Fail, size>();
	// Bucket_Array<Constructor_Should_Fail, size>  constructor_should_fail = Bucket_Array<Constructor_Should_Fail, size>();
	// Bucket_Array<Initialized_Struct, size> initialized_struct = Bucket_Array<Initialized_Struct, size>();
	const size_t size = 100;
	Bucket_Array<AABB, size> bucket_array  = Bucket_Array<AABB, size>();
	Bucket_Array<Pointer_Struct, size> other_bucket_array = Bucket_Array<Pointer_Struct, size>();

	for (int idx = 0; idx != 22; ++idx)
	{
		float a = static_cast<float>(idx);
		array_add(bucket_array, AABB{a,a,a,a});
	}

	for (auto& empty_bucket: bucket_array.unfilled_buckets)
		std::cerr << "empty bucket.! filled to: " << empty_bucket->size << '\n';	

	int idx = 0;
	for (auto& bucket : bucket_array.buckets)
	{
		std::cerr << "things in bucket"  << idx << " : \n";
		for (auto& value : bucket->data)
		{
			std:: cerr << "value.A: " << value.A << '\n'; 
		} 	
		idx ++;
	}

}