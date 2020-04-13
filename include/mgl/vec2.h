#ifndef INCLUDED_vec2_
#define INCLUDED_vec2_
#include <stdint.h>

namespace mgl
{
    union vec2
    {
        struct
        {
            float x;
            float y;
        };
        struct
        {
            float u;
            float v;
        };
        struct
        {
            float width;
            float height;
        };
        float data[2];
    };

    union ivec2
    {
        struct
        {
            int32_t x;
            int32_t y;
        };
        struct
        {
            int32_t u;
            int32_t v;
        };
        struct
        {
            int32_t width;
            int32_t height;
        };
        int32_t data[2];

        float operator[](size_t idx)
        {
            return data[idx];
        }
        float const operator[](size_t idx) const
        {
            return data[idx];
        }
    };

    struct uivec2
    {
        uint32_t x;
        uint32_t y;
    };


};



#endif