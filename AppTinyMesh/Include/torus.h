#pragma once

#pragma once

#include "mathematics.h"
#include "disc.h"

class Torus : public Disc
{
private:
	double s;

public:
	inline Torus() { }
	inline Torus(const Vector& c, const Vector& n, double r, double s) : Disc(c, n, r), s(s) { }
	inline double InnerRadius() const { return s; }
};

