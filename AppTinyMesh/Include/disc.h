#pragma once

#include "mathematics.h"

class Disc
{
private:
	Vector c;
	Vector n;
	double r;

public:
	inline Disc() { }
	inline Disc(const Vector& c, const Vector& n, double r) : c(c), n(n), r(r) { }

	inline double Radius() const { return r; }
	inline Vector Center() const { return c; }
	inline Vector Normal() const { return n; }
};
