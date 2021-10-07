/******************************************************************************/
/*!
\file   Collision.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

struct Manifold;
using CollisionCallback = void(*)(Manifold *m, size_t a, size_t b);
extern CollisionCallback Dispatch[eMAX][eMAX];

void SphereToSphere(Manifold *m, size_t a, size_t b);
void AABBToAABB(Manifold *m, size_t a, size_t b);

void AABBToSphere(Manifold *m, size_t a, size_t b);
void SphereToAABB(Manifold *m, size_t a, size_t b);

void SphereToPlane(Manifold *m, size_t a, size_t b);
void PlaneToSphere(Manifold *m, size_t a, size_t b);

void AABBToPlane(Manifold *m, size_t a, size_t b);
void PlaneToAABB(Manifold *m, size_t a, size_t b);

bool RayToSphere(const Vec3 &rayOrigin, const Vec3 &rayDir, const Sphere &sphere, float &t);
bool RayToAABB(const Vec3 &rayOrigin, const Vec3 &rayDir, const AABB &box, float &t);
//user have to normalise the rayDir since t is depend on that
bool RayToPlane(const Vec3 &rayOrigin, const Vec3 &rayDir, const Plane &plane, float &t);
bool RayToPlane(const Vec3 & planePos, const Vec3 &rayOrigin, const Vec3 &rayDir, const Plane &plane, float &t);

bool MovingSphereToSphere(const Sphere &movingSphere, const Vec3 &sphereVelocity, const Sphere &tStaticSphere, float& t);

void NoCollision(Manifold *m, size_t a, size_t b);

/* can be vec3 or vec2 */

template<typename Vector>
bool SphereToLineSegmentStatic(Vector const & centre, float radius, Vector const & start, Vector const & end)
{
  //check if the sphere is within line seg perpendicular to start and end
  float t;
  Vector ptonline;
  ClosestPtPointSegment(centre, start, end, t, ptonline);

  return (centre - ptonline).MagnitudeSq() < radius * radius;
}

//algorithm from the orange book
//c is the pt
//a is the start of line seg
//b is the end of line seg
//on is the pt on line seg
//t is P(t) = A + t(B - A) or in baracentric terms P(t) = tB + (1 - t)A
template<typename Vector>
void ClosestPtPointSegment(Vector const& ptc, Vector const& pta, Vector const& ptb, float &t, Vector& pton)
{
  Vector ab = ptb - pta;
  // proj c onto ab, but defferring by Dot(ab, ab) and the division will normalize the t
  t = ab.Dot(ptc - pta);
  if (t <= 0.0f)
  {
    // c proj outside the [a, b] interval, ont the a side; clamp to a
    t = 0.0f;
    pton = pta;
  }
  else
  {
    float denom = ab.Dot(ab); // always positive
    if (t >= denom)
    {
      //outside of b clampe to b
      t = 1.0f;
      pton = ptb;
    }
    else
    {
      //within ab
      t /= denom;
      pton = pta + t * ab;
    }
  }
}