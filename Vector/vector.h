#ifndef INCLUDED_VECTOR_
#define INCLUDED_VECTOR_
#include <cstddef> // only introduces types & no functions.

// what this is:
// - a replacement for std::vector.  
// - other allocation patterns?
// - a test for templates.
//

// what it is not:
// - reallocating for a larger capacity will invalidate previously held pointers. don't do it.
// - don't store pointers. I think.
//  



template<typename Type>



class Vector 
{
	Type* m_data;
	char *buffer;  // does not have constructor. nice?
	Type* m_front; // first?
	Type* m_back;  // last?

	size_t m_size;

	size_t m_capacity; // @Refactor: defaults?
	size_t m_high_water_mark;
	float  m_water_mark_ratio;
	float  m_growth_factor = 1.5f; // 2.0f?

	public:
		typedef Type* iterator;
		Vector();
		Vector(size_t size); // this is the constructor we always want to use?
		Vector(size_t size, const Type &prototype);
		Vector(std::initializer_list<Type> items); // 
		Vector(const Vector<Type> &rhs);
		~Vector();

		Vector     &operator=(Vector temp);


		//index operators
		Type const &operator[](int index) const;
		Type 	   &operator[](size_t index);
		Type &at(size_t index);

		// insertion / removal.
		void push_back(const Type &element);
		void push_back(Type &&element);
		void pop_back();


		// member accessors
		size_t size()  const;
		Type  *data(); 	// returns handle to start of the data.
		bool  empty()  const;
		size_t high_water_mark() const;

		// memory management.
		void shrink_to_fit();
		void swap(Vector &rhs); // noexcept?
		void reserve(size_t new_capacity); // pattern? * 1.5?

}

template<typename Type>
Vector<Type>::Vector()
:
	Vector
	m_capacity(0),
	m_size(0),
	m_data(new Type[capacity]) // use other new? this forces objects to have a default constructor, even if that is unwanted.
{}

template<typename Type>
Vector<Type>::Vector(size_t size)
:
	m_capacity(size),
	m_size(size),
	m_data(new Type[capacity])
{}

template<typename Type>
Vector<Type>::Vector(size_t size, const Type &prototype)
:
	m_capacity(size),
	m_size(size),
	m_data(new Type[capacity])
{
	//@Missing:for loop over elements?

}

template<typename Type>
Vector<Type>::Vector(std::initializer_list<Type> items)
:
	m_capacity(items.size()),
	m_size(items.size()),
	m_data(new Type[capacity])
{
	//@Missing: place elements? 
}

template<typename Type>
Vector<Type>::Vector(const Vector<Type> &rhs)
:
	m_capacity(rhs.m_size),
	m_size(rhs.m_size),
	m_data(new Type[capacity])
{
	//@Robustness: can we always memcpy all data of another vector?
	memcpy(rhs.data(),data(), m_size * sizeof(Type));
}



template<typename Type>
Vector<Type>::~Vector();
{
	delete[] m_data;
}

template<class Type>
inline size_t Vector<Type>::size() const
{
	return m_size;
}

template<class Type>
inline size_t Vector<Type>::high-water_mark() const
{
	return d_high_water_mark;
}

template<class Type>
void Vector<Type>::shrink_to_fit();
{
	if (d_capacity > d_size)
	{
		// realloc? swap with a smaller one?
	}
}

template<class Type>
Vector<Type>::&operator=(Vector temp)
{
	temp.swap(*this);
	return *this;
}

template<class Type>
Type const &operator[](int index) const
{
	return m_data[index];
}

template<class Type>
Type 	   &operator[](size_t index)
{
	return m_data[index];
}

template<class Type>
Type &at(size_t index)
{
	if (index >= size)
	{
		throw out_of_range("index requested beyond size.");
	}
	return m_data[index];
} 

template<class Type>
Type      *data(); // returns handle to start of the data.

template<class Type>
void swap(Vector &rhs) // noexcept?
{
	using std::swap;
	swap(m_capacity, other.m_capacity);
	swap(m_data, other.m_data);
}

void reserve(size_t new_capacity) // pattern? * 1.5?
{
	m_capacity = new_capacity;
}

//void swap(Vector &rhs);
//
void push_back(const Type &element)
{
	if (m_size == m_capacity)
	{
		reserve(m_capacity * m_growth_factor + 1); // resize? no, resize should keep the rest. I think.				
	}
	m_data[size] = element;
}


void push_back(Type &&element)
{
	if (m_size == m_capacity)
	{
		reserve(m_capacity * m_growth_factor + 1); // resize? no, resize should keep the rest. I think.				
	}
	m_data[size] = std::move(element);
}

template<class Type>
void Vector<Type>::pop_back()
{
    // You way want to do a check for empty() before calling the destructor.
	if (!empty())
	{
	    // Call the destructor.
	    (reinterpret_cast<T*>(buffer)[m_size - 1]).~T();
	    --m_size;
	}
}

// shrink_to_fit
void shrink_to_fit();
{
	if (capacity > size)
	{
		// realloc? swap with a smaller one?
	}
}