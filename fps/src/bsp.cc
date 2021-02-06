#include "bsp.h"

#include <vector>


BSP build_leaf_storing_bsp(std::vector<Polygon>& polygons)
{
    std::vector<Polygon*> polygon_ptrs;
    for (auto&& polygon: polygons)
    {
        polygon_ptrs.push_back(&polygon);
    }

    BSP bsp;
    const int starting_depth = 0;
    bsp.root_node = build_leaf_storing_bsp_recursively(polygon_ptrs, starting_depth);

    return bsp;
}


bsp_node* get_root_node(BSP& bsp)
{
    return bsp.root_node;
}

// a leaf node is when both front and back are both nullptr.
bool is_leaf_node(bsp_node* node)
{
    return (node->front == nullptr && node->back == nullptr);
}

// this should be file_local
//@Recursive
void free_bsp_recursively(bsp_node* node)
{
    if (node->front != nullptr)
    {
        free_bsp_recursively(node->front);      
    }
    if (node->back != nullptr)
    {
        free_bsp_recursively(node->back);   
    }
    
    delete node;
}

// this should be file_local
//@recursive
bsp_node* build_leaf_storing_bsp_recursively(std::vector<Polygon*>& polygons, int depth)
{
    const int BSP_MAX_TREE_DEPTH = 16;
    const int BSP_MIN_LEAF_SIZE = 2;

    if (polygons.empty()) return nullptr;

    size_t polygon_count = polygons.size();

    if (depth == BSP_MAX_TREE_DEPTH || polygon_count <= BSP_MIN_LEAF_SIZE)
    {
        // create empty node.
        bsp_node* new_node = new bsp_node();
        new_node->polygons = polygons;
        return new_node;
    }

    logr::report_error("[build_leaf_storing_bsp_recursively] USING pick_splitting_plane_AABB_geometry instead of regular pick_splitting_plane!\n");
    // Plane split_plane = pick_splitting_plane(polygons);
    Plane split_plane = pick_splitting_plane_AABB_geometry(polygons);

    std::vector<Polygon*> front_list{};
    std::vector<Polygon*> back_list{};
    for (size_t polygon_idx = 0; polygon_idx != polygon_count; ++polygon_idx)
    {
        Polygon* polygon_ptr = polygons[polygon_idx];
        auto polygon_plane_result = classify_polygon_to_plane(*polygon_ptr, split_plane);

        switch (polygon_plane_result)
        {
            case POLYGON_COPLANAR_WITH_PLANE:
            {
                // uh, this should actually fall through to in_front_of_plane. I don't know how to properly
                // do that with this switch style, so this mirrors IN_FRONT_OF_PLANE.
                front_list.push_back(polygon_ptr);
                break;
            }
            case POLYGON_IN_FRONT_OF_PLANE:
            {
                front_list.push_back(polygon_ptr);
                break;
            }
            case POLYGON_BEHIND_PLANE:
            {
                back_list.push_back(polygon_ptr);
                break;
            }
            case POLYGON_STRADDLING_PLANE:
            {
                logr::report_warning("[build_leaf_storing_bsp_recursively] POLYGON_STRADDLING_PLANE NOT HANDLED.\n");
                break;
            }
        }

        depth += 1;
        bsp_node* front_tree = build_leaf_storing_bsp_recursively(front_list, depth);
        bsp_node* back_tree =  build_leaf_storing_bsp_recursively(back_list,  depth);

        bsp_node* return_node = new bsp_node();

        return_node->front = front_tree;
        return_node->back = back_tree;
        return_node->plane = split_plane;

        logr::report("split_plane: {}\n", split_plane);
        return return_node;
    }
}

// bsp and positions are in world space?
bool bsp_trace(glm::vec3 position, glm::vec3 direction, const float dt, BSP& bsp)
{
    // glm::vec3 target_position = position + (direction * dt);

    // bsp_node* node = nullptr;
    // node = get_root_node(bsp);
    
    // // walk the bsp (broad phase)
    // while (true)
    // {
    //  if (is_leaf_node(node))
    //  {
    //      Polygon = &node->polygons;
    //  }

    //  // determine what side of the plane we are on.
    //  Plane& plane = bsp->plane;
 //         if (classify_point_to_plane(position, plane) == POINT_IN_FRONT_OF_PLANE)
 //         {
 //             node = bsp->front;
 //         }
 //         else
 //         {
 //             node = bsp->back;
 //         }
    // }

    // std::vector<Polygon>* polygons = nullptr;
    // polygons = node->polygons;

    // trace collision with the subset (narrow phase)
    // for (auto&& Polygon : polygons)
    // {

    //  if (intersection())
    //  {
    //      direction = clip(direction);
    //  }
    // }

    return true;
}
