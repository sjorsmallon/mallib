#ifndef INCLUDED_BSP_
#define INCLUDED_BSP_

#include "plane.h"
#include "polygon.h"
#include <vector>

// a leaf node is when both front and back are both nullptr.
struct bsp_node 
{
	Plane plane;
	bsp_node* front = nullptr; // idx?
	bsp_node* back  = nullptr;
	std::vector<Polygon*> polygons; // non-owning
};

struct BSP
{
	bsp_node* root_node;
};

BSP build_leaf_storing_bsp(std::vector<Polygon>& polygons);
bsp_node* build_leaf_storing_bsp_recursively(std::vector<Polygon*>& polygons, int depth);
bsp_node* get_root_node(BSP& bsp);

void free_bsp_recursively(bsp_node* node);
bool is_leaf_node(bsp_node* node);

bool bsp_trace(BSP& bsp);




#endif // INCLUDED_BSP