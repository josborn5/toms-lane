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
	T DotProduct(const Vec3<T> &v1, const Vec4<T> &v2);

	template<typename T>
	T DotProduct(const Vec3<T> &v1, const Vec3<T> &v2);

	template<typename T>
	Vec4<T> CrossProduct(const Vec4<T> &v1, const Vec4<T> &v2)
	{
		return Vec4<T>{
			(v1.y * v2.z) - (v1.z * v2.y),
			(v1.z * v2.x) - (v1.x * v2.z),
			(v1.x * v2.y) - (v1.y * v2.x)
		};
	}

	template<typename T>
	float Length(const Vec4<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
	}

	template<typename T>
	float Length(const Vec3<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
	}

	template<typename T>
	float Length(const Vec2<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y));
	}

	template<typename T>
	Vec4<T> UnitVector(const Vec4<T> &in)
	{
		float length = Length(in);
		return Vec4<T> { in.x / length, in.y / length, in.z / length } ;
	}

	template<typename T>
	Vec3<T> UnitVector(const Vec3<T> &in)
	{
		float length = Length(in);
		return Vec3<T> { in.x / length, in.y / length, in.z / length } ;
	}

	/**
	 * Indexing is done by row then column. matrix.m[row][col]
	 */
	template<typename T>
	struct Matrix4x4
	{
		T m[4][4] = {0};
	};

	template<typename T>
	void Project3DPointTo2D(const Vec4<T> &in, Vec4<T> &out, const Matrix4x4<T> &matrix)
	{
		MultiplyVectorWithMatrix(in, out, matrix);
		if (out.w != 0.0f)
		{
			out.x /= out.w;
			out.y /= out.w;
			out.z /= out.w;
		}
	}

	template<typename T>
	void MultiplyVectorWithMatrix(const Vec4<T> &in, Vec4<T> &out, const Matrix4x4<T> &matrix)
	{
		out.x = (in.x * matrix.m[0][0]) + (in.y * matrix.m[1][0]) + (in.z * matrix.m[2][0]) + (in.w * matrix.m[3][0]);
		out.y = (in.x * matrix.m[0][1]) + (in.y * matrix.m[1][1]) + (in.z * matrix.m[2][1]) + (in.w * matrix.m[3][1]);
		out.z = (in.x * matrix.m[0][2]) + (in.y * matrix.m[1][2]) + (in.z * matrix.m[2][2]) + (in.w * matrix.m[3][2]);
		out.w = (in.x * matrix.m[0][3]) + (in.y * matrix.m[1][3]) + (in.z * matrix.m[2][3]) + (in.w * matrix.m[3][3]);
	}

	template<typename T>
	Matrix4x4<T> MultiplyMatrixWithMatrix(const Matrix4x4<T> &m1, const Matrix4x4<T> &m2)
	{
		Matrix4x4<T> matrix;
		for (int col = 0; col < 4; col += 1)
		{
			for (int row = 0; row < 4; row += 1)
			{
				matrix.m[row][col] = m1.m[row][0] * m2.m[0][col]
								+ m1.m[row][1] * m2.m[1][col]
								+ m1.m[row][2] * m2.m[2][col]
								+ m1.m[row][3] * m2.m[3][col];
			}
		}
		return matrix;
	}
}

#endif
