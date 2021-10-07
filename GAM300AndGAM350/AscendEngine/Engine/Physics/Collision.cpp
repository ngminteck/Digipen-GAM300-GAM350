/******************************************************************************/
/*!
\file   Collision.cpp
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
#include "pch.h"

extern RigidBodies rb;

CollisionCallback Dispatch[eMAX][eMAX] =
{
  {
    SphereToSphere, SphereToAABB, SphereToPlane, NoCollision,
  },
  {
    AABBToSphere, AABBToAABB, AABBToPlane, NoCollision,
  },
  {
    PlaneToSphere, PlaneToAABB,  NoCollision /* Plane vs plane */, NoCollision
  },
  {
    NoCollision, NoCollision,  NoCollision, NoCollision
  }
};

/*
*/
static Vec3 ClosestPoint(const Vec3 &p, const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
  /** The code for Triangle-float3 test is from Christer Ericson's Real-Time Collision Detection, pp. 141-142. */

  // Check if P is in vertex region outside A.
  Vec3 ab = b - a;
  Vec3 ac = c - a;
  Vec3 ap = p - a;
  float d1 = ab.Dot(ap);
  float d2 = ac.Dot(ap);
  if (d1 <= 0.0f && d2 <= 0.0f)
    return a; // Barycentric coordinates are (1,0,0).

  // Check if P is in vertex region outside B.
  Vec3 bp = p - b;
  float d3 = ab.Dot(bp);
  float d4 = ac.Dot(bp);
  if (d3 >= 0.0f && d4 <= d3)
    return b; // Barycentric coordinates are (0,1,0).

  // Check if P is in edge region of AB, and if so, return the projection of P onto AB.
  float vc = d1*d4 - d3*d2;
  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
  {
    float v = d1 / (d1 - d3);
    return a + v * ab; // The barycentric coordinates are (1-v, v, 0).
  }

  // Check if P is in vertex region outside C.
  Vec3 cp = p - c;
  float d5 = ab.Dot(cp);
  float d6 = ac.Dot(cp);
  if (d6 >= 0.0f && d5 <= d6)
    return c; // The barycentric coordinates are (0,0,1).

  // Check if P is in edge region of AC, and if so, return the projection of P onto AC.
  float vb = d5*d2 - d1*d6;
  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
  {
    float w = d2 / (d2 - d6);
    return a + w * ac; // The barycentric coordinates are (1-w, 0, w).
  }

  // Check if P is in edge region of BC, and if so, return the projection of P onto BC.
  float va = d3*d6 - d5*d4;
  if (va <= 0.f && d4 - d3 >= 0.f && d5 - d6 >= 0.f)
  {
    float w = (d4 - d3) / (d4 - d3 + d5 - d6);
    return b + w * (c - b); // The barycentric coordinates are (0, 1-w, w).
  }

  // P must be inside the face region. Compute the closest point through its barycentric coordinates (u,v,w).
  float denom = 1.0f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  return a + ab * v + ac * w;
}

static bool InterectTriangleSphere(const Sphere &sphere, const Vec3 &a, const Vec3 &b, const Vec3 &c, float &dist)
{
  const Vec3 &spherePos = rb.position[sphere.body];
  Vec3 pt = ClosestPoint(spherePos, a, b, c);

  dist = (pt - spherePos).Magnitude();
  return dist <= sphere.radius;
}

static float SquaredDistPointAABB(const Vec3 &p, const Vec3 &aabbCenter, const AABB &aabb)
{
  auto check = [&](
    const float pn,
    const float bmin,
    const float bmax) -> float
  {
    float out = 0.0f;
    float v = pn;

    if (v < bmin)
    {
      float val = (bmin - v);
      out += val * val;
    }

    if (v > bmax)
    {
      float val = (v - bmax);
      out += val * val;
    }

    return out;
  };

  // Squared distance
  float sq = 0.0f;

  Vec3 min = aabbCenter + aabb.min;
  Vec3 max = aabbCenter + aabb.max;

  sq += check(p.x, min.x, max.x);
  sq += check(p.y, min.y, max.y);
  sq += check(p.z, min.z, max.z);

  return sq;
}

static Vec3 ClosestPoint(const AABB &aabb, const Vec3 &pt)
{
  Vec3 result;
  Vec3 boxPos = rb.position[aabb.body];
  Vec3 boxMin = boxPos + aabb.min;
  Vec3 boxMax = boxPos + aabb.max;
  result.x = max(boxMin.x, min(boxMax.x, pt.x));
  result.y = max(boxMin.y, min(boxMax.y, pt.y));
  result.z = max(boxMin.z, min(boxMax.z, pt.z));

  return result;
}

//static bool AABBSphereContact(Manifold *m, size_t a, size_t b, AABB *box, float radius)
//{
//  float squaredDistance = SquaredDistPointAABB(rb.position[b], rb.position[a], *box);
//
//  // Intersection if the distance from center is larger than the radius
//  // of the sphere.
//  float r2 = radius * radius;
//  return squaredDistance <= r2;
//}

static Vec3 BaryCoordFromPoint(const Vec3 &pt, const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
  Vec3 v0;
  Vec3 v1;
  Vec3 v2;

  v0 = c - a;
  v1 = b - a;
  v2 = pt - a;

  float dot00 = v0.Dot(v0);
  float dot01 = v0.Dot(v1);
  float dot02 = v0.Dot(v2);
  float dot11 = v1.Dot(v1);
  float dot12 = v1.Dot(v2);

  float denom = (dot00 * dot11 - dot01 * dot01);

  // collinear or singular triangle
  if (fabs(denom) < FLT_EPSILON)
  {
    // arbitrary location outside of triangle?
    // not sure if this is the best idea, maybe should be returning undefined
    return Vec3(-2.0f, -1.0f, -1.0f);
  }

  float invDenom = 1.0f / denom;
  float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // barycentric coordinates must always sum to 1
  return Vec3(1.0f - u - v, v, u);
}

static bool ContainsPointsInTriangle(const Vec3 &pt, const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
  Vec3 result = BaryCoordFromPoint(pt, a, b, c);
  return (result.x >= 0.0f) && (result.y >= 0.0f) && ((result.x + result.y) <= 1.0f);
}

static void staticSphereToSphere(Manifold *m, size_t a, size_t b)
{
  Sphere *A = reinterpret_cast<Sphere *>(rb.shape[a]);
  Sphere *B = reinterpret_cast<Sphere *>(rb.shape[b]);

  // Calculate translational vector, which is normal
  Vec3 normal = rb.position[b] - rb.position[a];

  float dist_sqr = normal.MagnitudeSq();
  float radius = A->radius + B->radius;

  // Not in contact
  if (dist_sqr >= radius * radius)
  {
    m->contact_count = 0;
    return;
  }

  float distance = sqrt(dist_sqr);

  m->contact_count = 1;

  if (fabs(distance) < FLT_EPSILON)
  {
    m->penetration = A->radius;
    m->normal = Vec3(1.0f, 0.0f, 0.0f);
    m->contacts[0] = rb.position[a];
    return;
  }

  m->penetration = radius - distance;
  m->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
  m->contacts[0] = m->normal * A->radius + rb.position[a];
}

void SphereToSphere(Manifold *m, size_t a, size_t b)
{
  // MovingSphereToSphere
  Sphere *A = reinterpret_cast<Sphere *>(rb.shape[a]);
  Sphere *B = reinterpret_cast<Sphere *>(rb.shape[b]);

  //First, we create a new sphere based on the static sphere's position but with the radius of both spheres
  Sphere tempSphere(*A);
  tempSphere.radius += B->radius;

  //Now, we get the length of our sphere's velocity. We will use this for the ray to sphere test and to check against collision time
  float vecLength = rb.velocity[b].Magnitude();

  if (vecLength < FLT_EPSILON) 
    return staticSphereToSphere(m, a, b);

  //We create a ray that starts at the moving sphere's center and has a direction of the normalized velocity
  Vec3 rayOrigin = rb.position[b];
  Vec3 rayDir = rb.velocity[b] / vecLength;

  //We call our ray to sphere function,
  float t;
  if (RayToSphere(rayOrigin, rayDir, tempSphere, t))
  {
    //We check if the time is less than or equal to the velocity's length
    if (t <= ASC_SYSTEM.GetDT())
    {
      // Collide at
      Vec3 collideAt = rayOrigin + rayDir * t;

      // Calculate translational vector, which is normal
      Vec3 normal = collideAt - rb.position[a];

      float dist_sqr = normal.MagnitudeSq();
      float distance = sqrt(dist_sqr);

      m->contact_count = 1;
      m->normal = Vec3::YAXIS;
      m->penetration = tempSphere.radius * 2.0f;

      if (distance > FLT_EPSILON)
      {
        m->penetration = t;
        m->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
      }
      m->contacts[0] = (m->normal * A->radius) + rb.position[a];
    }
  }
}

void AABBToAABB(Manifold *m, size_t a, size_t b)
{
  // Setup a couple pointers to each object
  AABB *A = reinterpret_cast<AABB *>(rb.shape[a]);
  AABB *B = reinterpret_cast<AABB *>(rb.shape[b]);

  Vec3 aMax = rb.position[a] + A->max;
  Vec3 aMin = rb.position[a] + A->min;
  Vec3 bMax = rb.position[b] + B->max;
  Vec3 bMin = rb.position[b] + B->min;

  Vec3 aCenter = (aMax + aMin) * 0.5f;
  Vec3 bCenter = (bMax + bMin) * 0.5f;

  Vec3 aExtents = aMax - aCenter;
  Vec3 bExtents = bMax - bCenter;

  const Vec3 dir = rb.position[b] - rb.position[a];

  Vec3 extends = aExtents + bExtents;
  if (fabs(dir.x) <= extends.x && fabs(dir.y) <= extends.y && fabs(dir.z) <= extends.z)
  {
    int axis = 0;
    Vec3 diff = extends - Vec3(fabs(dir.x), fabs(dir.y), fabs(dir.z));
    float penDist = diff.x;

    if (diff.y < penDist)
    {
      penDist = diff.y;
      axis = 1;
    }
    if (diff.z < penDist)
    {
      penDist = diff.z;
      axis = 2;
    }
    if (diff.x < penDist)
    {
      penDist = diff.x;
      axis = 0;
    }

    float normal = dir.m[axis] / fabs(dir.m[axis]);
    m->contact_count = 1;
    m->penetration = penDist;
    m->normal.m[axis] = normal;
    m->normal = m->normal.Normalize();
    m->contacts[0] = rb.position[b] + m->normal * m->penetration;
  }
}

void AABBToSphere(Manifold *m, size_t a, size_t b)
{
  // Setup a couple pointers to each object
  AABB *A = reinterpret_cast<AABB *>(rb.shape[a]);
  Sphere *B = reinterpret_cast<Sphere *>(rb.shape[b]);

  float squaredDistance = SquaredDistPointAABB(rb.position[b], rb.position[a], *A);

  // Intersection if the distance from center is larger than the radius
  // of the sphere.
  float r2 = B->radius * B->radius;

  if (squaredDistance <= r2)
  {
    Vec3 closestPt = ClosestPoint(*A, rb.position[b]);

    m->contact_count = 1;
    m->normal = (rb.position[b] - closestPt).Normalize();
    m->penetration = sqrtf(r2 - squaredDistance);
    m->contacts[0] = rb.position[b] + m->normal * m->penetration;
  }
}

void SphereToAABB(Manifold *m, size_t a, size_t b)
{
  // Setup a couple pointers to each object
  Sphere *A = reinterpret_cast<Sphere *>(rb.shape[a]);
  AABB *B = reinterpret_cast<AABB *>(rb.shape[b]);

  float squaredDistance = SquaredDistPointAABB(rb.position[b], rb.position[a], *B);

  // Intersection if the distance from center is larger than the radius
  // of the sphere.
  float r2 = A->radius * A->radius;

  if (squaredDistance <= r2)
  {
    Vec3 closestPt = ClosestPoint(*B, rb.position[a]);

    m->contact_count = 1;
    m->normal = -(rb.position[a] - closestPt).Normalize();
    m->penetration = sqrtf(r2 - squaredDistance);
    m->contacts[0] = rb.position[a] + m->normal * m->penetration;
  }
}

void PlaneToSphere(Manifold *m, size_t a, size_t b)
{
  // Setup a couple pointers to each object
  Plane *plane = reinterpret_cast<Plane *>(rb.shape[a]);
  Sphere *sphere = reinterpret_cast<Sphere *>(rb.shape[b]);

  if (plane->dimension.MagnitudeSq() < FLT_EPSILON)
  {
    // Use plane eqn and then dot product to find distance of point from plane
    float distanceFromSphereCenter = fabs(plane->normal.Dot(rb.position[b]));

    if (distanceFromSphereCenter < sphere->radius)
    {
      m->contact_count = 1;
      m->penetration = sphere->radius - distanceFromSphereCenter;
      m->normal = plane->normal.Normalize();
      m->contacts[0] = rb.position[b] - m->normal * m->penetration;
    }
  }
  else
  {
    Vec3 corners[4];
    plane->GetCorners(corners);
    //auto normal = plane->GetNormals();
    // Use plane eqn and then dot product to find distance of point from plane
    //float distanceFromSphereCenter = fabs(plane->normal.Dot(rb.position[b]));

    float dist;
    if (InterectTriangleSphere(*sphere, corners[0], corners[1], corners[2], dist))
    {
      m->contact_count = 1;
      m->penetration = dist - sphere->radius;
      m->normal = plane->normal;
      m->contacts[0] = rb.position[b] - m->normal * m->penetration;
    }
    else if (InterectTriangleSphere(*sphere, corners[0], corners[2], corners[3], dist))
    {
      m->contact_count = 1;
      m->penetration = dist - sphere->radius;
      m->normal = plane->normal;
      m->contacts[0] = rb.position[b] - m->normal * m->penetration;
    }
  }
}

void SphereToPlane(Manifold *m, size_t a, size_t b)
{
  PlaneToSphere(m, b, a);
}

void AABBToPlane(Manifold *m, size_t a, size_t b)
{
  // Setup a couple pointers to each object
  AABB *A = reinterpret_cast<AABB *>(rb.shape[a]);
  Plane *B = reinterpret_cast<Plane *>(rb.shape[b]);

  Vec3 max = rb.position[a] + A->max;
  Vec3 min = rb.position[a] + A->min;

  Vec3 center = (max + min) * 0.5f;
  Vec3 extents = max - center;

  float radius = extents.x * abs(B->normal.x) + extents.y * abs(B->normal.y) + extents.z * abs(B->normal.z);
  float dist = B->normal.Dot(center);

  if (fabs(dist) <= radius)
  {
    m->contact_count = 1;
    m->penetration = radius - fabs(dist);
    m->normal = (-B->normal * radius).Normalize();
    m->contacts[0] = rb.position[a] + m->penetration * m->normal;
  }
}

void PlaneToAABB(Manifold *m, size_t a, size_t b)
{
  AABBToPlane(m, b, a);
  m->normal *= -1.0f;
}

bool RayToSphere(const Vec3 &rayOrigin, const Vec3 &rayDir, const Sphere &sphere, float &t)
{
  //Create a vector from the sphere to the ray's start point
  Vec3 vecV1 = rayOrigin - rb.position[sphere.body];

  //Get the dot product of this vector with the ray's direction
  //This will become the 'b' part of our quadratic equation
  float fB = vecV1.Dot(rayDir);

  //Get the square distance from the start of the ray to the sphere's surface
  //This will become the 'c' part of our quadratic equation
  float fC = vecV1.Dot(vecV1) - (sphere.radius * sphere.radius);

  //If the ray starts outside the sphere and points away from it, we return false
  if (fC > 0.0f && fB > 0.0f)
    return false;

  //Else, we get the discriminant for our equation
  float fDisc = fB*fB - fC;

  //If this is less than zero, we return false
  if (fDisc < 0.0f)
    return false;

  //If we didn't want the time of collision, we could just return true here.
  //But in this case we do want it, so we'll continue

  //We solve our equation and get the time of collision
  //We use -sqrt(fDisc) to get the smallest root, ie. the first point in which the ray touches the sphere
  t = -fB - sqrt(fDisc);

  //If the time is less than zero, it means the ray started inside the sphere
  //If so, we just make it zero
  if (t < 0.0f)
    t = 0.0f;

  //Our collision point is going to be:
  //tRay.m_vecStart + tRay.m_vecDir * fT
  return true;
}

bool RayToAABB(const Vec3 &rayOrigin, const Vec3 &rayDir, const AABB &box, float &t)
{
  Vec3 ray = rayDir.Normalize();

  // ray.direction is unit direction vector of ray
  float dirFracX = 1.0f / ray.x;
  float dirFracY = 1.0f / ray.y;
  float dirFracZ = 1.0f / ray.z;

  Vec3 boxMin = rb.position[box.body] + box.min;
  Vec3 boxMax = rb.position[box.body] + box.max;

  // box.min is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
  float t1 = (boxMin.x - rayOrigin.x) * dirFracX;
  float t2 = (boxMax.x - rayOrigin.x) * dirFracX;
  float t3 = (boxMin.y - rayOrigin.y) * dirFracY;
  float t4 = (boxMax.y - rayOrigin.y) * dirFracY;
  float t5 = (boxMin.z - rayOrigin.z) * dirFracZ;
  float t6 = (boxMax.z - rayOrigin.z) * dirFracZ;

  // var tmin = Math.max(Math.max(Math.min(t1, t2), Math.min(t3, t4)));
  // var tmax = Math.min(Math.min(Math.max(t1, t2), Math.max(t3, t4)));
  float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
  float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

  // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
  // if tmin > tmax, ray doesn't intersect AABB
  if (tmax < 0.0f || tmin > tmax)
  {
    t = tmax;
    return false;
  }

  t = tmin;
  return true;
}

bool RayToPlane(const Vec3 &rayOrigin, const Vec3 &rayDir, const Plane &plane, float &t)
{
  return RayToPlane(rb.position[plane.body], rayOrigin, rayDir, plane, t);
}

bool RayToPlane(const Vec3 & planePos, const Vec3 & rayOrigin, const Vec3 & rayDir, const Plane & plane, float & t)
{
  // assuming vectors are all normalized
  Vec3 raydirn = rayDir;// rayDir.Normalize();
  float denom = plane.normal.Dot(raydirn);
  if (fabs(denom) < FLT_EPSILON)
    return false;

  Vec3 p0l0 = planePos - rayOrigin;
  t = p0l0.Dot(plane.normal) / denom;

  if (plane.dimension.MagnitudeSq() < FLT_EPSILON)
    return t >= 0.0f;

  Vec3 hitPt = rayOrigin + raydirn * t;
  Vec3 corners[4];
  plane.GetCorners(corners, planePos);

  return ContainsPointsInTriangle(hitPt, corners[0], corners[1], corners[2]) ||
    ContainsPointsInTriangle(hitPt, corners[0], corners[2], corners[3]);
}

void NoCollision(Manifold *, size_t, size_t)
{
  // Do nothing like plane vs plane
}

