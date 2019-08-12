#ifndef INCLUDED_VEC4_
#define INCLUDED_VEC4_

struct Vec4
{
	float x;
    float y;
    float z;
    float w;
    public:
    	float& operator[](const int index);

    	float* data();
};

inline float& Vec4::operator[](const int index)
{
	return ( &x )[index];
}
inline float* Vec4::data()
{
	return &x;
}

#endif