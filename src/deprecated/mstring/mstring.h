#ifndef INCLUDED_MSTRING_
#define INCLUDED_MSTRING_
#include <string> 

//@TODO: sensible defaults?

class MString
{
	char *m_data;
	size_t m_capacity;
	size_t m_size; //undefined? what do we do with this?

	public:
		MString() = delete; // while not necessary if other constructor is 
		// provided, this is a declaration of intention (i.e. do NOT use this)
		// that the compiler understands.
		MString(const char* string_literal); // constructs from string literal;
		MString(const std::string& string);
		~MString() = default; // no need for me to do anything about it.

		// need copy constructor? or assignment operator?
		// operator=(const string& string) = delete;
		MString& operator=(const MString& string) = delete;
		char* data(); // c_str? or data?
		size_t capacity() const;

};



inline char* MString::data()
{
	return &m_data;
}

inline size_t MString::capacity() const
{
	return m_capacity;
}

inline size_t Mstring::size() const
{
	return m_size;
}


// #ifndef _XSTRING_H
// #define _XSTRING_H

// #include "stl_allocator.h"
// #include <string>

// typedef std::basic_string<char, std::char_traits<char>, stl_allocator<char> > xstring;
// typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, stl_allocator<wchar_t> > xwstring;

// #endif

#endif