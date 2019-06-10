#ifndef INCLUDED_VEC4_
#define INCLUDED_VEC4_

struct Vec4
{
    public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

        Vec4() = default;
        Vec4(const float x);
        Vec4(const float x,
             const float y,
             const float z,
             const float w);
        Vec4(const Vec3& rhs);
};

inline Vec4::Vec4(const float x)
:
    this->x(x),
    this->y(x),
    this->z(x),
    this->w(x)
{}

inline Vec4::Vec4(const float x,
                  const float y,
                  const float z,
                  const float w)
:
    this->x(x),
    this->y(y),
    this->z(z),
    this->w(w)
{}

#endif