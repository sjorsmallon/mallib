#ifndef INCLUDED_VEC3F_
#define INCLUDED_VEC3F_

// math: invsqrt

// supports aggregate initialization, POD type
// cannot specify constructors.
// helper functions are in vector_transform.
// some operator overloading in order to facilitate 
// many-used operations.
struct Vec3
{
    float x; // = 0;
    float y; // = 0;
    float z; // = 0;
    
    float  operator[](const int index) const;
    float &operator[](const int index);
    Vec3 operator+(const Vec3& rhs) const;
    Vec3 operator-() const;
    Vec3 operator-(const Vec3& rhs) const;
    bool compare(const Vec3& rhs)   const;
    bool operator==(const Vec3& rhs) const;   
    bool operator!=( const Vec3& rhs) const;

    float *data();

    Vec3& operator+=(const Vec3& rhs);
   // Vec3 operator*(const Mat4& rhs);

    // free function operator* and operator+ for float
};

inline Vec3& Vec3::operator+=(const Vec3& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}


inline bool Vec3::compare( const Vec3 &rhs) const
{
    return ( (x == rhs.x) &&
             (y == rhs.y) &&
             (z == rhs.z)
            );
}
inline bool Vec3::operator==(const Vec3 &rhs) const
{
    return compare(rhs);
}

inline bool Vec3::operator!=(const Vec3 &rhs) const
{
    return !compare(rhs);
}

inline float Vec3::operator[](const int index) const
{
    return ( &x )[index]; //this is less dangerous but still dangerous.
}

inline float &Vec3::operator[](const int index)
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

inline float *Vec3::data()
{
    return &x;
}

// free functions
inline Vec3 operator*(const float lhs, const Vec3& rhs)
{
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

inline Vec3 operator+(const float lhs, const Vec3& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
}

#endif


