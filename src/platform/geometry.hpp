#ifndef TOMS_LANE_GEOMETRY_H
#define TOMS_LANE_GEOMETRY_H

#include "./math.hpp"
#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	struct Rect
	{
		Vec2<T> position;
		Vec2<T> halfSize;
		Vec2<T> velocity;
	};

	Rect<float> CopyRect(const Rect<float>& original);

	template<typename T>
	struct Triangle2d
	{
		Vec2<T> p[3];
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
	struct Camera
	{
		Vec4<T> position;
		Vec4<T> direction;
		Vec4<T> up;
	};

	template<typename T>
	Matrix4x4<T> MakeIdentityMatrix();

	template<typename T>
	Matrix4x4<T> MakeTranslationMatrix
	(
		T dispX,
		T dispY,
		T dispZ
	);


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

	template<typename T>
	Vec4<T> IntersectPlane
	(
		const Plane<T> &plane,
		const Vec4<T> &lineStart,
		const Vec4<T> lineEnd
	);

	template<typename T>
	T ShortestDistanceFromPointToPlane
	(
		const Vec4<T> &point,
		const Vec3<T> &planeP,
		const Vec3<T> &unitNormalToPlane
	);

	template<typename T>
	int ClipTriangleAgainstPlane
	(
		const Plane<T> &plane,
		Triangle4d<T> &inputTriangle,
		Triangle4d<T> &outputTriangle1,
		Triangle4d<T> &outputTriangle2
	);

	Matrix4x4<float> MakeProjectionMatrix
	(
		float fieldOfVewDeg,
		float aspectRatio,
		float nearPlane,
		float farPlane
	);

	void SetZAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeZAxisRotationMatrix(float theta);

	void SetYAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeYAxisRotationMatrix(float theta);

	void SetXAxisRotationMatrix(float theta, Matrix4x4<float> &matrix);

	Matrix4x4<float> MakeXAxisRotationMatrix(float theta);
}

#endif
