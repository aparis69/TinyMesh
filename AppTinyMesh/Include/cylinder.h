#pragma once
#include "mathematics.h"

class Cylinder
{
private:
	Vector a, b;
	double r;

public:
	inline Cylinder() { }
	inline Cylinder(const Vector& a, const Vector& b, double r) : a(a), b(b), r(r) { }

	inline Vector Vertex(int i) const { if (i == 0) return a; else return b; }
	inline double Radius() const { return r; }
};
