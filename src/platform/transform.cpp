#include "./geometry.hpp"
#include "./math.hpp"

namespace tl
{

void transform_interface_create_2d_projection_matrix(
	const Rect<float>& source,
	const Rect<float>& target,
	Matrix2x3<float>& matrix
)
{
	float scale_factor = target.halfSize.x / source.halfSize.x;
	float dX = -source.position.x + target.halfSize.x;
	float dY = -source.position.y + target.halfSize.y;

	matrix.m[0][0] = scale_factor;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = scale_factor * dX;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = scale_factor;
	matrix.m[1][2] = scale_factor * dY;
}

}
