#include "./math.hpp"
#include <math.h>
#include "./geometry.hpp"

namespace tl
{
	void SetZAxisRotationMatrix(float theta, Matrix4x4<float> &matrix)
	{
		float cos = cosf(theta);
		float sin = sinf(theta);
		matrix.m[0][0] = cos;
		matrix.m[0][1] = -sin;
		matrix.m[1][0] = sin;
		matrix.m[1][1] = cos;
	}

	Matrix4x4<float> MakeZAxisRotationMatrix(float theta)
	{
		Matrix4x4<float> matrix = MakeIdentityMatrix<float>();
		SetZAxisRotationMatrix(theta, matrix);
		return matrix;
	}

	void SetYAxisRotationMatrix(float theta, Matrix4x4<float> &matrix)
	{
		float cos = cosf(theta);
		float sin = sinf(theta);
		matrix.m[0][0] = cos;
		matrix.m[0][2] = sin;
		matrix.m[2][0] = -sin;
		matrix.m[2][2] = cos;
	}

	Matrix4x4<float> MakeYAxisRotationMatrix(float theta)
	{
		Matrix4x4<float> matrix = MakeIdentityMatrix<float>();
		SetYAxisRotationMatrix(theta, matrix);
		return matrix;
	}

	void SetXAxisRotationMatrix(float theta, Matrix4x4<float> &matrix)
	{
		float cos = cosf(theta);
		float sin = sinf(theta);
		matrix.m[1][1] = cos;
		matrix.m[1][2] = -sin;
		matrix.m[2][1] = sin;
		matrix.m[2][2] = cos;
	}

	Matrix4x4<float> MakeXAxisRotationMatrix(float theta)
	{
		Matrix4x4<float> matrix = MakeIdentityMatrix<float>();
		SetXAxisRotationMatrix(theta, matrix);
		return matrix;
	}

	template<typename T>
	Matrix4x4<T> MakeIdentityMatrix()
	{
		Matrix4x4<T> matrix;
		matrix.m[0][0] = 1;
		matrix.m[1][1] = 1;
		matrix.m[2][2] = 1;
		matrix.m[3][3] = 1;
		return matrix;
	}
	template Matrix4x4<float> MakeIdentityMatrix();

	template<typename T>
	Matrix4x4<T> MakeTranslationMatrix(
		T dispX,
		T dispY,
		T dispZ
	)
	{
		Matrix4x4<T> matrix = MakeIdentityMatrix<T>();
		matrix.m[3][0] = dispX;
		matrix.m[3][1] = dispY;
		matrix.m[3][2] = dispZ;
		return matrix;
	}
	template Matrix4x4<float> MakeTranslationMatrix(
		float dispX,
		float dispY,
		float dispZ
	);

	/**
	* Structure of the PointAt Matrix:
	* | Ax | Ay | Az | 0 |
	* | Bx | By | Bz | 0 |
	* | Cx | Cy | Cz | 0 |
	* | Tx | Ty | Tz | 1 |
	*/
	template<typename T>
	Matrix4x4<T> PointAt(
		const Vec4<T> &position,
		const Vec4<T> &target,
		const Vec4<T> &up
	)
	{
		// Vector from the position to the target is the new forward direction
		Vec4<T> forwardUnit = SubtractVectors(target, position);
		forwardUnit = UnitVector(forwardUnit);

		// Calculate the new up direction of the new forward direction
		T newUpScalar = DotProduct(up, forwardUnit);
		Vec4<T> newUpTemp = MultiplyVectorByScalar(forwardUnit, newUpScalar);
		Vec4<T> upUnit = SubtractVectors(up, newUpTemp);
		upUnit = UnitVector(upUnit);

		// Calculate the new right direction for the new up & forward directions
		Vec4<T> rightUnit = CrossProduct(upUnit, forwardUnit);

		// Construct the new transformation matrix
		Matrix4x4<T> pointAt;
		pointAt.m[0][0] = rightUnit.x;		pointAt.m[0][1] = rightUnit.y;		pointAt.m[0][2] = rightUnit.z;		pointAt.m[0][3] = 0;
		pointAt.m[1][0] = upUnit.x;			pointAt.m[1][1] = upUnit.y;			pointAt.m[1][2] = upUnit.z;			pointAt.m[1][3] = 0;
		pointAt.m[2][0] = forwardUnit.x;	pointAt.m[2][1] = forwardUnit.y;	pointAt.m[2][2] = forwardUnit.z;	pointAt.m[2][3] = 0;
		pointAt.m[3][0] = position.x;		pointAt.m[3][1] = position.y;		pointAt.m[3][2] = position.z;		pointAt.m[3][3] = 1;
		return pointAt;
	}
	template Matrix4x4<float> PointAt<float>(
		const Vec4<float> &position,
		const Vec4<float> &target,
		const Vec4<float> &up
	);

	/**
	* Structure of the LookAt Matrix:
	* |  Ax  |  Bx  |  Cx  | 0 |
	* |  Ay  |  By  |  Cy  | 0 |
	* |  Az  |  Bz  |  Cz  | 0 |
	* | -T.A | -T.B | -T.C | 1 |
	*/
	template<typename T>
	Matrix4x4<T> LookAt(Matrix4x4<T> const &pointAt)
	{
		T tDotA = (pointAt.m[3][0] * pointAt.m[0][0]) + (pointAt.m[3][1] * pointAt.m[0][1]) + (pointAt.m[3][2] * pointAt.m[0][2]);
		T tDotB = (pointAt.m[3][0] * pointAt.m[1][0]) + (pointAt.m[3][1] * pointAt.m[1][1]) + (pointAt.m[3][2] * pointAt.m[1][2]);
		T tDotC = (pointAt.m[3][0] * pointAt.m[2][0]) + (pointAt.m[3][1] * pointAt.m[2][1]) + (pointAt.m[3][2] * pointAt.m[2][2]);

		Matrix4x4<T> lookAt;
		lookAt.m[0][0] = pointAt.m[0][0];	lookAt.m[0][1] = pointAt.m[1][0];	lookAt.m[0][2] = pointAt.m[2][0];	lookAt.m[0][3] = 0;
		lookAt.m[1][0] = pointAt.m[0][1];	lookAt.m[1][1] = pointAt.m[1][1];	lookAt.m[1][2] = pointAt.m[2][1];	lookAt.m[1][3] = 0;
		lookAt.m[2][0] = pointAt.m[0][2];	lookAt.m[2][1] = pointAt.m[1][2];	lookAt.m[2][2] = pointAt.m[2][2];	lookAt.m[2][3] = 0;
		lookAt.m[3][0] = -tDotA;			lookAt.m[3][1] = -tDotB;			lookAt.m[3][2] = -tDotC;			lookAt.m[3][3] = 1;
		return lookAt;
	}
	template Matrix4x4<float> LookAt<float>(Matrix4x4<float> const &pointAt);
}

