#pragma once

#include "mathematics.h"

class Matrix3
{
private:
	double r[9];

public:
	inline Matrix3() { }
	inline Matrix3(double r00, double r01, double r02, double r10, double r11, double r12, double r20, double r21, double r22);

	// Functions to access Matrix class components  
	constexpr double& operator[] (int);
	constexpr double operator[] (int) const;

	friend Matrix3 operator+ (const Matrix3&, const Matrix3&);
	friend Matrix3 operator- (const Matrix3&, const Matrix3&);
	friend Matrix3 operator* (const Matrix3&, const Matrix3&);
	Vector operator*(const Vector& v) const;
	Matrix3 Transpose() const;

public:
	static Matrix3 RotationX(double theta);
	static Matrix3 RotationY(double theta);
	static Matrix3 RotationZ(double theta);
	static Matrix3 Rotation(const Vector& angle);

	static Matrix3 Scaling(const Vector& scaleFactor);

public:
	static Matrix3 Identity;
};

inline constexpr double& Matrix3::operator[] (int i)
{
	return r[i];
}

inline constexpr double Matrix3::operator[] (int i) const
{
	return r[i];
}