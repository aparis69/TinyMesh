#pragma once

#include "mathematics.h"

class Sphere
{
private:
	Vector c;
	double r;

public:
	inline Sphere() { }
	inline Sphere(const Vector& c, double r) : c(c), r(r) { }

	inline double Radius() const { return r; }
	inline Vector Center() const { return c; }
};
