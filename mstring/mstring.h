#ifndef INCLUDED_MSTRING_
#define INCLUDED_MSTRING_

//@refactor: for now, always allocate 256 characters and see how that works.
class MString
{
	char m_data[256];
	size_t m_capacity = 256;
	size_t m_size; // undefined? what do we do with this?
	public:
		MString() = delete; // what does this do?
		MString(const char* string_literal); // constructs from string literal;
		MString(const string& string);
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

#endif