#ifndef TOMS_LANE_GEOMETRY_H
#define TOMS_LANE_GEOMETRY_H

#include "./math.hpp"
#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	struct Rect
	{
		Vec2<T> position = {0};
		Vec2<T> halfSize = {0};
		Vec2<T> velocity = {0};

		T x_min() const
		{
			return position.x - halfSize.x;
		}

		T x_max() const
		{
			return position.x + halfSize.x;
		}

		T y_min() const
		{
			return position.y - halfSize.y;
		}

		T y_max() const
		{
			return position.y + halfSize.y;
		}
	};

	template<typename T>
	struct Triangle4d
	{
		Vec4<T> p[3];
		unsigned int color;
	};

	template<typename T>
	struct Plane
	{
		Vec3<T> position;
		Vec3<T> normal;
	};

	template<typename T>
	Matrix4x4<T> MakeIdentityMatrix();



	/**
	* Structure of the PointAt Matrix:
	* | Ax | Ay | Az | 0 |
	* | Bx | By | Bz | 0 |
	* | Cx | Cy | Cz | 0 |
	* | Tx | Ty | Tz | 1 |
	*/
	template<typename T>
	Matrix4x4<T> PointAt
	(
		const Vec4<T> &position,
		const Vec4<T> &target,
		const Vec4<T> &up
	);


	/**
	* Structure of the LookAt Matrix:
	* |  Ax  |  Bx  |  Cx  | 0 |
	* |  Ay  |  By  |  Cy  | 0 |
	* |  Az  |  Bz  |  Cz  | 0 |
	* | -T.A | -T.B | -T.C | 1 |
	*/
	template<typename T>
	Matrix4x4<T> LookAt(Matrix4x4<T> const &pointAt);

	void SetZAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeZAxisRotationMatrix(float theta);

	void SetYAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeYAxisRotationMatrix(float theta);

	void SetXAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeXAxisRotationMatrix(float theta);
}

#endif
