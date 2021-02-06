#include "plane.h"
#include "polygon.h"
#include "logr.h"

Point_To_Plane_Result classify_point_to_plane(glm::vec3 position, Plane plane)
{
	float distance_to_plane = glm::dot(plane.n, position) - plane.d;

	// classify point based on the signed input
	if (distance_to_plane > PLANE_THICKNESS_EPSILON) return POINT_IN_FRONT_OF_PLANE;

	if (distance_to_plane < -PLANE_THICKNESS_EPSILON) return POINT_BEHIND_PLANE;

	return POINT_ON_PLANE;
}

Polygon_To_Plane_Result classify_polygon_to_plane(Polygon& polygon, Plane plane)
{
	int32_t in_front_count = 0;
	int32_t behind_count = 0;

	for (auto&& point: polygon.vertices)
	{
		auto point_to_plane_result = classify_point_to_plane(point, plane);
		switch (point_to_plane_result)
		{
			case POINT_IN_FRONT_OF_PLANE:
			{
				in_front_count += 1;
				break;
			}
			case POINT_BEHIND_PLANE: 
			{
				behind_count += 1;
				break;
			}
		}
	}

	// vertices on both sides of the plane, polygon is straddling.
	if ((behind_count != 0 && in_front_count != 0)) return POLYGON_STRADDLING_PLANE;

	// one or more vertices lie in front of the plane, no vertices behind the plane: polygon lies in front of the plane.
	if (in_front_count != 0) return POLYGON_IN_FRONT_OF_PLANE;

	// ditto: polygon lies behind the frame if the behind count is greater than zero.
	if (behind_count != 0) return POLYGON_BEHIND_PLANE;

	// all vertices lie "on" the plane so the polygon is coplanar with the plane.
	return POLYGON_COPLANAR_WITH_PLANE;
}

enum Axis_Planes {
	PLANE_POS_X = 0,
	PLANE_POS_Y,
	PLANE_POS_Z,
	PLANE_NEG_X,
	PLANE_NEG_Y,
	PLANE_NEG_Z
};

Plane pick_splitting_plane_AABB_geometry(std::vector<Polygon*>& polygons)
{
	// optimizing for balance & splits.
	const float blend_factor = 0.8f;

	Plane best_plane{};

	Plane axis_aligned_planes[6] = {
		{glm::vec3{1.0f, 0.0f, 0.0f}, 0.0f},
		{glm::vec3{0.0f, 1.0f, 0.0f}, 0.0f},
		{glm::vec3{0.0f, 0.0f, 1.0f}, 0.0f},
		{glm::vec3{-1.0f, 0.0f, 0.0f}, 0.0f},
		{glm::vec3{0.0f, -1.0f, 0.0f}, 0.0f},
		{glm::vec3{0.0f, 0.0f, -1.0f}, 0.0f}
	};
	
	// score close to 0 is best.
	float best_score = std::numeric_limits<float>::max();

	for (auto&& starting_polygon: polygons)
	{
		logr::report("new starting polygon.\n");
		
		axis_aligned_planes[PLANE_POS_X].d = starting_polygon->max.x; 
		axis_aligned_planes[PLANE_POS_Y].d = starting_polygon->max.y;
		axis_aligned_planes[PLANE_POS_Z].d = starting_polygon->max.z;
		axis_aligned_planes[PLANE_NEG_X].d = -starting_polygon->min.x;
		axis_aligned_planes[PLANE_NEG_Y].d = -starting_polygon->min.y;
		axis_aligned_planes[PLANE_NEG_Z].d = -starting_polygon->min.z;

		for (size_t face_count = 0; face_count != 6; ++face_count)
		{
			int32_t in_front_count = 0; 
			int32_t behind_count = 1; //behind count is 1 since the plane is with the normal of the face ( meaning "this" polygon is "behind" the plane.)
			int32_t straddling_count = 0; 
			Plane plane = axis_aligned_planes[face_count];
			logr::report("plane: {}\n", plane);
			for (auto&& other_polygon: polygons)
			{
				if (&starting_polygon == &other_polygon)
				{
					logr::report("skipping equal polygon\n");
					continue;	
				}

				auto other_polygon_plane_result = classify_polygon_to_plane(*other_polygon, plane);
				switch (other_polygon_plane_result)
				{
					case POLYGON_IN_FRONT_OF_PLANE:
					{
						logr::report("POLYGON_IN_FRONT_OF_PLANE\n");
						in_front_count += 1;
						break;
					}
					case POLYGON_BEHIND_PLANE:
					{
						logr::report("POLYGON_BEHIND_PLANE\n");
						behind_count += 1;
						break;
					}
					case POLYGON_STRADDLING_PLANE:
					{
						logr::report("POLYGON_STRADDLING_PLANE\n");
						straddling_count += 1;
						break;
					}
					// default cannot be reached.
				}
			}

			// if we have no in_front_count, we didn't exactly make a split.
			if (!in_front_count) continue;

			float score = blend_factor * straddling_count + (1.f - blend_factor) * std::abs(in_front_count - behind_count);
			if (score < best_score)
			{
				best_score = score;
				best_plane = plane;
			}
		}
	}
	logr::report("best_score: {}\n", best_score);
	return best_plane;
}

//@FIXME(SJORS): this is special-cased for AABB polygons, since their plane-picking will be wrong.
Plane pick_splitting_plane(std::vector<Polygon*>& polygons)
{
	// optimizing for balance & splits.
	const float blend_factor = 0.8f;

	Plane best_plane{};
	
	// score close to 0 is best.
	float best_score = std::numeric_limits<float>::max();

	for (auto&& starting_polygon: polygons)
	{
		int32_t in_front_count = 0; 
		int32_t behind_count = 0;
		int32_t straddling_count = 0; 


		Plane plane = get_plane_from_polygon(*starting_polygon);

		for (auto&& other_polygon: polygons)
		{
			if (&starting_polygon == &other_polygon) continue;



			auto other_polygon_plane_result = classify_polygon_to_plane(*other_polygon, plane);
			switch (other_polygon_plane_result)
			{
				case POLYGON_IN_FRONT_OF_PLANE:
				{
					in_front_count += 1;
					break;
				}
				case POLYGON_BEHIND_PLANE:
				{
					behind_count += 1;
					break;
				}
				case POLYGON_STRADDLING_PLANE:
				{
					straddling_count += 1;
					break;
				}
				// default cannot be reached.
			}
		}

		float score = blend_factor * straddling_count + (1.f - blend_factor) * std::abs(in_front_count - behind_count);
		if (score < best_score)
		{
			best_score = score;
			best_plane = plane;
		}
	}

	return best_plane;
}

// http://cs.haifa.ac.il/~gordon/plane.pdf
//Computing the Plane Equation of a Polygon
Plane get_plane_from_polygon(Polygon& polygon)
{
	// using the naming from the above paper.
	size_t rhs_idx = 0;
	float A{};
	float B{};
	float C{};
	float D{};
	// P is center of gravity ("average point").
	glm::vec3 P{};
	
	for (size_t lhs_idx = 0; lhs_idx != polygon.vertices.size(); ++lhs_idx)
	{
		rhs_idx = (lhs_idx + 1) % polygon.vertices.size();
		auto& lhs = polygon.vertices[lhs_idx];
		auto& rhs = polygon.vertices[rhs_idx];
		
		A += (lhs.y - rhs.y) * (lhs.z + rhs.z);
		B += (lhs.z - rhs.z) * (lhs.x + rhs.x);
		C += (lhs.x - rhs.x) * (lhs.y + lhs.y);

		P += lhs;
	}	
	P = P / static_cast<float>(polygon.vertices.size());
	glm::vec3 N = glm::normalize(glm::vec3(A,B,C));
	D = - glm::dot(N, P);
	logr::report("center of mass P: {}\n", P);	
	return Plane{N, D};	
}

