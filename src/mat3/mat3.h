#ifndef INCLUDED_MAT3_
#define INCLUDED_MAT3_

// ROW MAJOR
struct Mat3
{
  float data[3][3];

  float* operator[](size_t idx)
  {
    return data[idx];
  }
};



#endif