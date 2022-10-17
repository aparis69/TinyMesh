#include "matrix.h"
#include <assert.h>

Matrix3 Matrix3::Identity = Matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

Matrix3::Matrix3(double r00, double r01, double r02, double r10, double r11, double r12, double r20, double r21, double r22)
{
	r[0] = r00; r[1] = r01; r[2] = r02;
	r[3] = r10; r[4] = r11; r[5] = r12;
	r[6] = r20; r[7] = r21; r[8] = r22;
}

Matrix3 Matrix3::RotationX(double theta)
{
	theta = Math::DegreeToRadian(theta);
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);
	return Matrix3(
		1, 0, 0,
		0, cosTheta, -sinTheta,
		0, sinTheta, cosTheta
	);
}

Matrix3 Matrix3::RotationY(double theta)
{
	theta = Math::DegreeToRadian(theta);
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);
	return Matrix3(
		cosTheta, 0, sinTheta,
		0, 1, 0,
		-sinTheta, 0, cosTheta
	);
}

Matrix3 Matrix3::RotationZ(double theta)
{
	theta = Math::DegreeToRadian(theta);
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);
	return Matrix3(
		cosTheta, -sinTheta, 0,
		sinTheta, cosTheta, 0,
		0, 0, 1
	);
}

Matrix3 Matrix3::Rotation(const Vector& v)
{
	return Matrix3::RotationX(v[0]) * Matrix3::RotationY(v[1]) * Matrix3::RotationZ(v[2]);
}

Matrix3 Matrix3::Scaling(const Vector& v)
{
	return Matrix3(v[0], 0.0, 0.0, 0.0, v[1], 0.0, 0.0, 0.0, v[2]);
}

Matrix3 Matrix3::Transpose() const
{
	return Matrix3(
		r[0], r[3], r[6],
		r[1], r[4], r[7],
		r[2], r[5], r[8]
	);
}

/*!
\brief This function assumes the matrix is diagonal.
*/
Matrix3 Matrix3::Inverse() const
{
	for (int i = 0; i < 9; i++)
	{
		if (i == 0 || i == 4 || i == 8)
			continue;
		assert(r[i] == 0.0);
	}
	
	Matrix3 ret = *this;
	ret[0] = 1.0 / ret[0];
	ret[4] = 1.0 / ret[4];
	ret[8] = 1.0 / ret[8];
	return ret;
}

Matrix3 operator+ (const Matrix3& u, const Matrix3& v)
{
	return Matrix3(u[0] + v[0], u[1] + v[1], u[2] + v[2], u[3] + v[3], u[4] + v[4], u[5] + v[5], u[6] + v[6], u[7] + v[7], u[8] + v[8]);
}

Matrix3 operator-(const Matrix3& u, const Matrix3& v)
{
	return Matrix3(u[0] - v[0], u[1] - v[1], u[2] - v[2], u[3] - v[3], u[4] - v[4], u[5] - v[5], u[6] - v[6], u[7] - v[7], u[8] - v[8]);
}

Matrix3 operator* (const Matrix3& u, const Matrix3& v)
{
	return Matrix3(u[0] * v[0] + u[3] * v[1] + u[6] * v[2], 
		u[1] * v[0] + u[4] * v[1] + u[7] * v[2], 
		u[2] * v[0] + u[5] * v[1] + u[8] * v[2],
		u[0] * v[3] + u[3] * v[4] + u[6] * v[5], 
		u[1] * v[3] + u[4] * v[4] + u[7] * v[5], 
		u[2] * v[3] + u[5] * v[4] + u[8] * v[5],
		u[0] * v[6] + u[3] * v[7] + u[6] * v[8], 
		u[1] * v[6] + u[4] * v[7] + u[7] * v[8], 
		u[2] * v[6] + u[5] * v[7] + u[8] * v[8]);
}

Vector Matrix3::operator*(const Vector& v) const
{
	return Vector(v[0] * r[0] + v[1] * r[3] + v[2] * r[6], v[0] * r[1] + v[1] * r[4] + v[2] * r[7], v[0] * r[2] + v[1] * r[5] + v[2] * r[8]);
}
