#include "Vector.ih"


Vector::~Vector()
{
	delete[] m_data;
}

template<typename Type>
Vector::Vector()
:
	capacity(0),
	m_data(new Type[capacity])
{
}

