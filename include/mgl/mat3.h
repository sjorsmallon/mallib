#ifndef INCLUDED_mat3_
#define INCLUDED_mat3_

// ROW MAJOR
namespace mgl
{
	struct mat3
	{
	  float data[3][3];

	  float* operator[](size_t idx)
	  {
	    return data[idx];
	  }
	  float const *operator[](size_t idx) const 
	  {
	  	return data[idx];
	  }
	};
}


#endif