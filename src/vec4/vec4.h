#ifndef INCLUDED_VEC4_
#define INCLUDED_VEC4_

//@Note: is this a problem with matrices?
union Vec4
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    }
    struct 
    {
        float r;
        float g;
        float b;
        float a;
    }
    float data[4];
};


#endif