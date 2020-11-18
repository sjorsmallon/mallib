#ifndef INCLUDED_VERTEX_
#define INCLUDED_VERTEX_

//@Note(Sjors): DON'T use these for uniform buffers. they are not properly aligned.
struct Vertex_XNCUU
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv0; // texture atlas
	glm::vec2 uv1; // local 
};

struct Vertex_XNC
{

};

struct Vertex_XC
{

};


#endif