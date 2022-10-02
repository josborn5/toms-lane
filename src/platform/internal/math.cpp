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
}