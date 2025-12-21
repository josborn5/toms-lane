#include "./math.hpp"

float ShortestDistanceFromPointToPlane(
	const tl::Vec3<float>& point,
	const tl::Vec3<float>& plane_point,
	const tl::Vec3<float>& unit_normal_to_plane
)
{
	tl::Vec3<float> plane_to_point = tl::SubtractVectors<float>(point, plane_point);

	float distance = tl::DotProduct(
		tl::UnitVector(unit_normal_to_plane),
		plane_to_point
	);
	return distance;
}


