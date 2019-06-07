#include <string>
#include <vector>

//use _t for size guarantees/

// stretchy_buffer
//allocator_traits?
// vector with allocator?

struct Mesh
{
    vector<Vertex> vertices;
    vector<size_t> indices;
}



class SString //: public stretchy buffer?
{
    size_t length;
    size_t capacity;
    char  *data;

}

struct SString_Memory_Pool
{
    char *memory_pool;
    size_t capacity; // maximum number of characters this thing can fit?
    size_t size; // in kb? mb? gb? pages? what is a page?
    std::vector<char *> data_addresses;
    char *first_free_block;
    size_t largest_free_block;
    size_t high_water_mark; // if this is reached, double? or when it is necessary?
    // free this string.
    // zero everything?
}


// what do you want the behavior to be whenever something is allocated?
// it always seems simple to just push_back. 

//maybe it makes sense to initialize 


void eat_leading_spaces(std::string &string_to_eat)
{
    string_to_eat.erase(string_to_eat.begin(), string_to_eat.find_first_not_of(' '));
}

void eat_all_spaces(std::string &string_to_eat)
{
    string_to_eat.erase(std::remove_if(string_to_eat.begin(), string_to_eat.end(), isspace));
}

// char *concatenate(char *lhs, char *rhs) // already exists!

void eat_leading_spaces(char *string_to_eat)
{
    size_t str_length = 0;
    char *front_ptr = string_to_eat;
    char *end_ptr = nullptr;

    if (str == nullptr)
    {
        // do what? return nullptr;
    }
    str_length = strlen(string_to_eat);
    end_ptr = string_to_eat + length;
    while (isspace((unsigned char) *front_tr))
    {
        ++front_ptr;
    }

    if ( string_to_eat + str_length - 1 != end_ptr)
    {
        *(end_ptr + 1) = '\0';
    }
    else if ( front_ptr != string_to_eat && end_ptr != front_ptr)
    {
        *string_to_eat = '\0';
    }
    end_ptr = string_to_eat;
    if (front_ptr != string_to_eat)
    {
        while (*front_ptr)
        {
            *end_ptr++ = *front_ptr++; //@Refactor: this is too clever.
        }
        *end_ptr = '\0';
    }
    // the original implementation returns here. why?
}

void eat_all_spaces(char *string_to_eat)
{
    char *first  = string_to_eat;
    char *second = string_to_eat;
    // first stays in place ,second advances until the next character is not a space.
    // this does not alter the allocated memory, just places a \0 earlier 
    // in the size. does this leak memory? 
    //  no, there is a difference betwee sizeof() and strlen. sizeof will return the buffer size,
    // while strlen will return the amount of characters in the buffer.
    while(*second != '\0')
    {
        *first = *second;
        ++second; 
        if (*first != ' ')
        {
            ++first;
        }
    }
    *first = '\0';
}