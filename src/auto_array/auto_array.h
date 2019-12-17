#ifndef INCLUDED_AUTO_ARRAY_
#define INCLUDED_AUTO_ARRAY_

// nothing const at all.

template<typename Type>
class Auto_Array
{
	Type* m_data;
	size_t m_size;
	size_t m_capacity;

	public:
		
	push_back(Type& element);
	operator[](size_t index);

}

