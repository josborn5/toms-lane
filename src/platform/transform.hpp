#ifndef TOMS_LANE_TRANSFORM_HPP
#define TOMS_LANE_TRANSFORM_HPP

namespace tl
{

void transform_interface_create_2d_projection_matrix(
	const Rect<float>& source,
	const Rect<float>& target,
	Matrix2x3<float>& projection
);

void transform_interface_project_rect(
	const Matrix2x3<float>& projection,
	const Rect<float>& source,
	Rect<float>& target
);

}

#endif
