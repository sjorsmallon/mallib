#ifndef INCLUDED_PLANE_
#define INCLUDED_PLANE_

#include <vector>
#include <glm/vec3.hpp>
#include <limits> // std::numeric_limits<float>::max()

#include "polygon.h"
#include "logr.h"

const float PLANE_THICKNESS_EPSILON = 0.1f;

struct Plane
{
	glm::vec3 n;	
	float d; // distance from the origin in the direction of the normal.
};

enum Point_To_Plane_Result : uint8_t
{
	POINT_ON_PLANE = 0,
	POINT_BEHIND_PLANE = 1,
	POINT_IN_FRONT_OF_PLANE = 2
};

enum Polygon_To_Plane_Result: uint8_t
{
	POLYGON_STRADDLING_PLANE = 0,
	POLYGON_BEHIND_PLANE = 1,
	POLYGON_IN_FRONT_OF_PLANE = 2,
	POLYGON_COPLANAR_WITH_PLANE = 3
};


Point_To_Plane_Result   classify_point_to_plane(glm::vec3 position, Plane plane);
Polygon_To_Plane_Result classify_polygon_to_plane(Polygon& polygon, Plane plane);

Plane get_plane_from_polygon(Polygon& polygon);
Plane pick_splitting_plane(std::vector<Polygon*>& polygons);



Plane pick_splitting_plane_AABB_geometry(std::vector<Polygon*>& polygons);


namespace fmt
{
	//	Plane
    template <>
    struct formatter<Plane>
    {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const Plane &lhs, FormatContext &ctx)
		{
			return format_to(ctx.out(), "N: ({:.8f}, {:.8f} {:.8f}), D: {:.8f}",
		   		lhs.n.x, lhs.n.y, lhs.n.z, lhs.d);
		}
    };
}



// void split_polygon_by_plane(Polygon& poly, Plane plane) {}


#endif // INCLUDED_PLANE_