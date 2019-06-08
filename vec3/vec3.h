#ifndef INCLUDED_VEC3F_
#define INCLUDED_VEC3F_
#include "../math.h"
// #include <string>
// #include <sstream>

// math: invsqrt
struct Vec3
{
    public:
    float x = 0;
    float y = 0;
    float z = 0;

    Vec3() {}
    explicit Vec3(float x, float y, float z);
    explicit Vec3(float x);

    float  operator[](const int index) const;
    float &operator[](const int index);
    //float const &operator[](const int index) const; // help?

    Vec3 operator+(const Vec3&rhs) const;
    Vec3 operator-() const;
    Vec3 operator-(const Vec3& rhs) const;
    bool operator==( const Vec3 &rhs) const;
    bool operator!=( const Vec3 &rhs) const;
    void set(float x, float y, float z);
    float *data();


};

inline bool Vec3::compare( const Vec3 &rhs) const
{
    return ( (x == rhs.x) &&
             (y == rhs.y) &&
             (z == rhs.z)
            );
}

inline float Vec3::operator[](int index) const
{
    return ( &x )[index]; //this is dangerous.
    

}

inline float &Vec3::operator[](int index)
{
    return ( &x )[index];
}

inline Vec3 Vec3::operator-() const
{
    return Vec3(-x,-y,-z);
}

inline Vec3 Vec3::operator-(const Vec3 &rhs) const
{
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}


inline Vec3 Vec3::operator/(const float rhs) const
{
    float inv_rhs = 1.0f / rhs;
    return Vec3(x * inv_rhs, y * inv_rhs, z * inv_rhs);
}

inline bool Vec3::operator==(const Vec3 &rhs) const
{
    return compare(rhs);
}

inline bool Vec3::operator!=(const Vec3 &rhs) const
{
    return !compare(rhs);
}

// 
inline float *Vec3::data()
{
    return &x;
}



#endif


