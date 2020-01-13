#ifndef INCLUDED_MEMORY_
#define INCLUDED_MEMORY_

namespace mem
{
 	size_t& active_memory();

	template<typename T>
	class Mallocator
	{
		public:
		    using value_type = T;

		    Mallocator() noexcept {}  // not required, unless used
		    template <class U> Mallocator(Mallocator<U> const&) noexcept {}

	  		// Use pointer if pointer is not a value_type*
		    value_type* allocate(std::size_t n)
		    {
		    	mem::active_memory() += (n * sizeof(value_type));
		        return static_cast<value_type*>(::operator new (n*sizeof(value_type)));
		    }

		     // Use pointer if pointer is not a value_type*
		    void deallocate(value_type* p, std::size_t size) noexcept 
		    {
		    	mem::active_memory() -= size * sizeof(value_type);
		        ::operator delete(p);
		    }
		
	};

	template <class T, class U>
	bool
	operator==(Mallocator<T> const&, Mallocator<U> const&) noexcept
	{
	    return true;
	}

	template <class T, class U>
	bool
	operator!=(Mallocator<T> const& x, Mallocator<U> const& y) noexcept
	{
	    return !(x == y);
	}

};

#endif
