#ifndef INCLUDED_MEMORY_POOL
#define INCLUDED_MEMORY_POOL


class Memory_Pool
{
  // list of free memory blocks?
  // list of occupied blocks?
  // linked list? doubly linked list?
  // information contained in the block itself? or here?
  char* data; // or void*data? how to take care of alignment?

}

namespace mmem
{

}


#endif