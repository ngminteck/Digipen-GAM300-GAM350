#include "pch.h"


Bezier::Bezier()
: Curve()
{

}

Bezier::~Bezier()
{
}

void Bezier::_on_way_point_added()
{
	if(static_cast<int>(_way_points.size()) < 4)
	{
		return;
	}

	int new_control_point_index= static_cast<int>(_way_points.size()) - 1;

	if(new_control_point_index == 3)
	{
		for(int i=0; i<=_steps; i++)
		{
			float u= (float)i / (float)_steps;

			add_node(interpolate(u, _way_points[0], _way_points[1], _way_points[2], _way_points[3]));
		}
	}
	else
	{
		if(new_control_point_index % 2 == 0)
		{
			return;
		}
		
		int pt=new_control_point_index - 2;
		for(int i=0; i<=_steps; i++)
		{
			float u=(float)i / (float)_steps;
			Vec3 point4= _way_points[pt]*2 - _way_points[pt-1];

			add_node(interpolate(u, _way_points[pt], point4, _way_points[pt+1], _way_points[pt+2]));
		}
	}
}

Vec3 Bezier::interpolate(float u, const Vec3& P0, const Vec3& P1, const Vec3& P2, const Vec3& P3)
{
	Vec3 point;
	point=(P0 * (-1.f) + P1* 3.f  - P2 * 3.f + P3)*u*u*u;
	point+= (P0 * 3 - P1 * 6 + P2 * 3)*u*u;
	point+=( P0* (-3) +  P1*3)*u;
	point+=P0;

	return point;
}

