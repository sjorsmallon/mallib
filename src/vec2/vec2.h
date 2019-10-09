#ifndef INCLUDED_VEC2_
#define INCLUDED_VEC2_

// union Vec2
// {
//     struct
//     {
//         float x;
//         float y;
//     };
//     struct
//     {
//         float u;
//         float v;
//     };
//     struct
//     {
//         float width;
//         float height;
//     };
//     float data[2];
// };

struct Vec2
{
    float x;
    float y;
};

struct Vec2i
{
	int32_t x;
	int32_t y;
};

struct Vec2u
{
	uint32_t x;
	uint32_t y;
};


#endif