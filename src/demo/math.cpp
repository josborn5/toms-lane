#include "./math.hpp"

float ShortestDistanceFromPointToPlane(
	const tl::Vec4<float>& point,
	const tl::Vec3<float>& plane_point,
	const tl::Vec3<float>& unit_normal_to_plane
)
{
	tl::Vec3<float> temp_point;
	temp_point.x = point.x;
	temp_point.y = point.y;
	temp_point.z = point.z;

	tl::Vec3<float> plane_to_point = tl::SubtractVectors<float>(temp_point, plane_point);

	float distance = tl::DotProduct(unit_normal_to_plane, plane_to_point);
	return distance;
}


