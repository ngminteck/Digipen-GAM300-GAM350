#pragma once

#include "Curve.h"

class Bezier : public Curve
{
public:
	Bezier();
	virtual ~Bezier();

protected:
	virtual void _on_way_point_added();

protected:
	Vec3 interpolate(float u, const Vec3& P0, const Vec3& P1, const Vec3& P2, const Vec3& P3);
};
