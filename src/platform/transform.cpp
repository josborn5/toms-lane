#include "./geometry.hpp"
#include "./math.hpp"

namespace tl
{

static void Transform2DVector(const Vec2<float>& in, const Matrix2x3<float>& transformMatrix, Vec2<float>& out)
{
	out.x = (in.x * transformMatrix.m[0][0]) + (in.y * transformMatrix.m[0][1]) + (transformMatrix.m[0][2]);
	out.y = (in.x * transformMatrix.m[1][0]) + (in.y * transformMatrix.m[1][1]) + (transformMatrix.m[1][2]);
}

void transform_interface_create_2d_projection_matrix(
	const Rect<float>& source,
	const Rect<float>& target,
	Matrix2x3<float>& projection
)
{
	float scale_factor = target.halfSize.x / source.halfSize.x;
	float dX = target.position.x - (source.position.x * scale_factor);
	float dY = target.position.y - (source.position.y * scale_factor);

	projection.m[0][0] = scale_factor;
	projection.m[0][1] = 0.0f;
	projection.m[0][2] = dX;

	projection.m[1][0] = 0.0f;
	projection.m[1][1] = scale_factor;
	projection.m[1][2] = dY;
}

void transform_interface_project_rect(
	const Matrix2x3<float>& projection,
	const Rect<float>& source,
	Rect<float>& target
)
{
	Transform2DVector(source.position, projection, target.position);
	target.halfSize.x = projection.m[0][0] * source.halfSize.x;
	target.halfSize.y = projection.m[1][1] * source.halfSize.y;
}

}
