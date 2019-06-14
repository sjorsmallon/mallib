#ifndef INCLUDED_VEC3F_
#define INCLUDED_VEC3F_

// math: invsqrt

// support aggregate initialization, POD type. cannot specify constructors.
struct Vec3
{
    float x = 0;
    float y = 0;
    float z = 0;

    float  operator[](const int index) const;
    float &operator[](const int index);
    Vec3 operator+(const Vec3&rhs) const;
    Vec3 operator-() const;
    Vec3 operator-(const Vec3& rhs) const;
    bool operator==(const Vec3& rhs) const;
    bool compare(const Vec3& rhs) const;
    bool operator!=( const Vec3& rhs) const;
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
    return ( &x )[index]; //this is less dangerous but still dangerous.
}

inline float &Vec3::operator[](int index)
{
    return ( &x )[index];
}

inline Vec3 Vec3::operator-() const
{
    return {-x,-y,-z};
}

inline Vec3 Vec3::operator-(const Vec3 &rhs) const
{
    return {x - rhs.x, y - rhs.y, z - rhs.z};
}


// inline Vec3 Vec3::operator/(const float rhs) const
// {
//     float inv_rhs = 1.0f / rhs;
//     return Vec3(x * inv_rhs, y * inv_rhs, z * inv_rhs);
// }

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


