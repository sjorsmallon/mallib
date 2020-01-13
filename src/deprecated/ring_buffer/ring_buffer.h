#ifndef INCLUDED_RING_BUFFER_
#define INCLUDED_RING_BUFFER_
#include <vector>

template<typename Type>
struct Ring_Buffer
{
	size_t first_free_spot;
	size_t size;
	std::vector<Type> data;

	public:

		Ring_Buffer(size_t size)
		:
			data(size),
			size(data.size()),
			first_free_spot(0)
		{
		}

	void insert(Type T)
	{
		//@Speed:
		vector.at(first_free_spot) = T; // this overrides... hmm. This will only work for ints or such.
		++first_free_spot;
		first_free_spot %= size;
	}
};


#endif