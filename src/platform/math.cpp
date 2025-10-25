#include "./math.hpp"
#include <math.h>

namespace tl
{
	template<typename T>
	Vec4<T> AddVectors(const Vec4<T> &v1, const Vec4<T> &v2)
	{
		return Vec4<T>{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}
	template Vec4<int> AddVectors(const Vec4<int> &v1, const Vec4<int> &v2);
	template Vec4<float> AddVectors(const Vec4<float> &v1, const Vec4<float> &v2);
	template Vec4<double> AddVectors(const Vec4<double> &v1, const Vec4<double> &v2);

	template<typename T>
	Vec3<T> AddVectors(const Vec3<T> &v1, const Vec3<T> &v2)
	{
		return Vec3<T>{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}
	template Vec3<int> AddVectors(const Vec3<int> &v1, const Vec3<int> &v2);
	template Vec3<float> AddVectors(const Vec3<float> &v1, const Vec3<float> &v2);
	template Vec3<double> AddVectors(const Vec3<double> &v1, const Vec3<double> &v2);

	template<typename T>
	Vec2<T> AddVectors(const Vec2<T> &v1, const Vec2<T> &v2)
	{
		return Vec2<T>{v1.x + v2.x, v1.y + v2.y };
	}
	template Vec2<int> AddVectors(const Vec2<int> &v1, const Vec2<int> &v2);
	template Vec2<float> AddVectors(const Vec2<float> &v1, const Vec2<float> &v2);
	template Vec2<double> AddVectors(const Vec2<double> &v1, const Vec2<double> &v2);

	template<typename T>
	Vec4<T> SubtractVectors(const Vec4<T> &v1, const Vec4<T> &v2)
	{
		return Vec4<T>{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}
	template Vec4<int> SubtractVectors(const Vec4<int> &v1, const Vec4<int> &v2);
	template Vec4<float> SubtractVectors(const Vec4<float> &v1, const Vec4<float> &v2);
	template Vec4<double> SubtractVectors(const Vec4<double> &v1, const Vec4<double> &v2);

	template<typename T>
	Vec3<T> SubtractVectors(const Vec3<T> &v1, const Vec3<T> &v2)
	{
		return Vec3<T>{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}
	template Vec3<int> SubtractVectors(const Vec3<int> &v1, const Vec3<int> &v2);
	template Vec3<float> SubtractVectors(const Vec3<float> &v1, const Vec3<float> &v2);
	template Vec3<double> SubtractVectors(const Vec3<double> &v1, const Vec3<double> &v2);

	template<typename T>
	Vec2<T> SubtractVectors(const Vec2<T> &v1, const Vec2<T> &v2)
	{
		return Vec2<T>{v1.x - v2.x, v1.y - v2.y};
	}
	template Vec2<int> SubtractVectors(const Vec2<int> &v1, const Vec2<int> &v2);
	template Vec2<float> SubtractVectors(const Vec2<float> &v1, const Vec2<float> &v2);
	template Vec2<double> SubtractVectors(const Vec2<double> &v1, const Vec2<double> &v2);

	Vec2<float> CopyVec2(const Vec2<float>& original)
	{
		Vec2<float> copy = { original.x, original.y };
		return copy;
	}

	template<typename T>
	Vec4<T> MultiplyVectorByScalar(const Vec4<T> &vec, T sca)
	{
		return Vec4<T>{ vec.x * sca, vec.y * sca, vec.z * sca };
	}
	template Vec4<int> MultiplyVectorByScalar(const Vec4<int> &vec, int sca);
	template Vec4<float> MultiplyVectorByScalar(const Vec4<float> &vec, float sca);
	template Vec4<double> MultiplyVectorByScalar(const Vec4<double> &vec, double sca);

	template<typename T>
	Vec3<T> MultiplyVectorByScalar(const Vec3<T> &vec, T sca)
	{
		return Vec3<T>{ vec.x * sca, vec.y * sca, vec.z * sca };
	}
	template Vec3<int> MultiplyVectorByScalar(const Vec3<int> &vec, int sca);
	template Vec3<float> MultiplyVectorByScalar(const Vec3<float> &vec, float sca);
	template Vec3<double> MultiplyVectorByScalar(const Vec3<double> &vec, double sca);

	template<typename T>
	Vec2<T> MultiplyVectorByScalar(const Vec2<T> &vec, T sca)
	{
		return Vec2<T>{ vec.x * sca, vec.y * sca };
	}
	template Vec2<int> MultiplyVectorByScalar(const Vec2<int> &vec, int sca);
	template Vec2<float> MultiplyVectorByScalar(const Vec2<float> &vec, float sca);
	template Vec2<double> MultiplyVectorByScalar(const Vec2<double> &vec, double sca);

	template<typename T>
	T DotProduct(const Vec4<T> &v1, const Vec4<T> &v2)
	{
		return
			(v1.x * v2.x) +
			(v1.y * v2.y) +
			(v1.z * v2.z);
	}
	template int DotProduct(const Vec4<int> &v1, const Vec4<int> &v2);
	template float DotProduct(const Vec4<float> &v1, const Vec4<float> &v2);
	template double DotProduct(const Vec4<double> &v1, const Vec4<double> &v2);

	template<typename T>
	T DotProduct(const Vec3<T> &v1, const Vec4<T> &v2)
	{
		return
			(v1.x * v2.x) +
			(v1.y * v2.y) +
			(v1.z * v2.z);
	}
	template int DotProduct(const Vec3<int> &v1, const Vec4<int> &v2);
	template float DotProduct(const Vec3<float> &v1, const Vec4<float> &v2);
	template double DotProduct(const Vec3<double> &v1, const Vec4<double> &v2);

	template<typename T>
	T DotProduct(const Vec3<T> &v1, const Vec3<T> &v2)
	{
		return
			(v1.x * v2.x) +
			(v1.y * v2.y) +
			(v1.z * v2.z);
	}
	template int DotProduct(const Vec3<int> &v1, const Vec3<int> &v2);
	template float DotProduct(const Vec3<float> &v1, const Vec3<float> &v2);
	template double DotProduct(const Vec3<double> &v1, const Vec3<double> &v2);

	template<typename T>
	Vec4<T> CrossProduct(const Vec4<T> &v1, const Vec4<T> &v2)
	{
		return Vec4<T>{
			(v1.y * v2.z) - (v1.z * v2.y),
			(v1.z * v2.x) - (v1.x * v2.z),
			(v1.x * v2.y) - (v1.y * v2.x)
		};
	}
	template Vec4<float> CrossProduct(const Vec4<float> &v1, const Vec4<float> &v2);
	template<typename T>
	Vec3<T> CrossProduct(const Vec3<T> &v1, const Vec3<T> &v2)
	{
		return Vec3<T>{
			(v1.y * v2.z) - (v1.z * v2.y),
			(v1.z * v2.x) - (v1.x * v2.z),
			(v1.x * v2.y) - (v1.y * v2.x)
		};
	}
	template Vec3<float> CrossProduct(const Vec3<float> &v1, const Vec3<float> &v2);

	template<typename T>
	float Length(const Vec4<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
	}
	template float Length(const Vec4<float> &in);

	template<typename T>
	float Length(const Vec3<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
	}
	template float Length(const Vec3<float> &in);

	template<typename T>
	float Length(const Vec2<T> &in)
	{
		return sqrtf((in.x * in.x) + (in.y * in.y));
	}
	template float Length(const Vec2<float> &in);

	template<typename T>
	Vec4<T> UnitVector(const Vec4<T> &in)
	{
		float length = Length(in);
		return Vec4<T> { in.x / length, in.y / length, in.z / length } ;
	}
	template Vec4<float> UnitVector(const Vec4<float> &in);

	template<typename T>
	Vec3<T> UnitVector(const Vec3<T> &in)
	{
		float length = Length(in);
		return Vec3<T> { in.x / length, in.y / length, in.z / length } ;
	}
	template Vec3<float> UnitVector(const Vec3<float> &in);

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
	template void Project3DPointTo2D(const Vec4<float> &in, Vec4<float> &out, const Matrix4x4<float> &matrix);

	template<typename T>
	void MultiplyVectorWithMatrix(const Vec4<T> &in, Vec4<T> &out, const Matrix4x4<T> &matrix)
	{
		out.x = (in.x * matrix.m[0][0]) + (in.y * matrix.m[1][0]) + (in.z * matrix.m[2][0]) + (in.w * matrix.m[3][0]);
		out.y = (in.x * matrix.m[0][1]) + (in.y * matrix.m[1][1]) + (in.z * matrix.m[2][1]) + (in.w * matrix.m[3][1]);
		out.z = (in.x * matrix.m[0][2]) + (in.y * matrix.m[1][2]) + (in.z * matrix.m[2][2]) + (in.w * matrix.m[3][2]);
		out.w = (in.x * matrix.m[0][3]) + (in.y * matrix.m[1][3]) + (in.z * matrix.m[2][3]) + (in.w * matrix.m[3][3]);
	}
	template void MultiplyVectorWithMatrix(const Vec4<float> &in, Vec4<float> &out, const Matrix4x4<float> &matrix);

	Vec2<float> Transform2DVector(const Vec2<float> &in, const Matrix2x3<float> &transformMatrix)
	{
		Vec2<float> out;
		out.x = (in.x * transformMatrix.m[0][0]) + (in.y * transformMatrix.m[0][1]) + (transformMatrix.m[0][2]);
		out.y = (in.x * transformMatrix.m[1][0]) + (in.y * transformMatrix.m[1][1]) + (transformMatrix.m[1][2]);
		return out;
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
	template Matrix4x4<float> MultiplyMatrixWithMatrix(const Matrix4x4<float> &m1, const Matrix4x4<float> &m2);
}
