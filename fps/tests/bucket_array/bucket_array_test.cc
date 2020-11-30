#include "../../src/bucket_array.h" 

struct AABB
{
	float A;
	float a;
	float B;
	float b;		
};

int main()
{

	const size_t size = 100;
	Bucket_Array<AABB, size> bucket_array  = Bucket_Array<AABB, size>();

	add_bucket(bucket_array);

}