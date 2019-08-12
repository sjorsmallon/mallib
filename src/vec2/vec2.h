#ifndef INCLUDED_VEC2_
#define INCLUDED_VEC2_

union Vec2
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


#endif