#ifndef TOMS_LANE_MATH_H
#define TOMS_LANE_MATH_H

namespace tl
{
	template<typename T>
	struct Vec2
	{
		T x;
		T y;
	};

	template<typename T>
	struct Vec3
	{
		T x;
		T y;
		T z;
	};

	template<typename T>
	struct Vec4
	{
		T x;
		T y;
		T z;
		T w;
	};

	template<typename T>
	Vec4<T> AddVectors(const Vec4<T> &v1, const Vec4<T> &v2);
	template<typename T>
	Vec3<T> AddVectors(const Vec3<T> &v1, const Vec3<T> &v2);
	template<typename T>
	Vec2<T> AddVectors(const Vec2<T> &v1, const Vec2<T> &v2);

	template<typename T>
	Vec4<T> SubtractVectors(const Vec4<T> &v1, const Vec4<T> &v2);
	template<typename T>
	Vec3<T> SubtractVectors(const Vec3<T> &v1, const Vec3<T> &v2);
	template<typename T>
	Vec2<T> SubtractVectors(const Vec2<T> &v1, const Vec2<T> &v2);

	template<typename T>
	Vec4<T> MultiplyVectorByScalar(const Vec4<T> &vec, T sca);
	template<typename T>
	Vec3<T> MultiplyVectorByScalar(const Vec3<T> &vec, T sca);
	template<typename T>
	Vec2<T> MultiplyVectorByScalar(const Vec2<T> &vec, T sca);

	template<typename T>
	T DotProduct(const Vec4<T> &v1, const Vec4<T> &v2);
	template<typename T>
	T DotProduct(const Vec3<T> &v1, const Vec3<T> &v2);
	template<typename T>
	T DotProduct(const Vec2<T> &v1, const Vec2<T> &v2);

	template<typename T>
	float Length(const Vec4<T> &in);
	template<typename T>
	float Length(const Vec3<T> &in);
	template<typename T>
	float Length(const Vec2<T> &in);

	template<typename T>
	Vec4<T> CrossProduct(const Vec4<T> &v1, const Vec4<T> &v2);

	template<typename T>
	Vec4<T> UnitVector(const Vec4<T> &in);
	template<typename T>
	Vec3<T> UnitVector(const Vec3<T> &in);

	/**
	 * Indexing is done by row then column. matrix.m[row][col]
	 */
	template<typename T>
	struct Matrix4x4
	{
		T m[4][4] = {0};
	};

	template<typename T>
	void Project3DPointTo2D(const Vec4<T> &in, Vec4<T> &out, const Matrix4x4<T> &matrix);

	template<typename T>
	void MultiplyVectorWithMatrix(const Vec4<T> &in, Vec4<T> &out, const Matrix4x4<T> &matrix);

	template<typename T>
	Matrix4x4<T> MultiplyMatrixWithMatrix(const Matrix4x4<T> &m1, const Matrix4x4<T> &m2);
}

#endif
