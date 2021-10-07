/******************************************************************************/
/*!
\file   Shape.cpp
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

Shape::~Shape()
{
}

void Shape::DrawDebug() const
{
  rb.position[body] = ASC_FACTORY.GetComponentPtr<TransformComp>(ASC_PHYSICSMANAGER.GetGameObjectHandleFromBody(body))->pos + rb.colliderOffset[body];
}

Sphere::Sphere(float r) :
  radius(r)
{
}

ShapeType Sphere::GetType() const
{
  return eSPHERE;
}

void Sphere::DrawDebug() const
{
  Shape::DrawDebug();
  Vec4 color = Color::RED;
  if (rb.trigger[body]) color = Color::ORANGE;
  ASC_LINERENDER.AddSphere(rb.position[body], radius, color);
}

bool Sphere::Ray(const Vec3& pos, const Vec3& nDir, float& t)
{
  return RayToSphere(pos, nDir, *this, t);
}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

AABB::AABB(Vec3 min, Vec3 max) :
  min(min), max(max)
{
}

ShapeType AABB::GetType() const
{
  return eAABB;
}

void AABB::DrawDebug() const
{
  Shape::DrawDebug();
  Vec4 color = Color::RED;
  if (rb.trigger[body]) color = Color::ORANGE;
  ASC_LINERENDER.AddCube(rb.position[body], min, max, color);
}

bool AABB::Ray(const Vec3& pos, const Vec3& nDir, float& t)
{
  return RayToAABB(pos, nDir, *this, t);
}

Plane::Plane(Vec3 norm, Vec3 v1, Vec3 v2, Vec2 dim)
  : normal(norm.Normalize()), basis{v1.Normalize(), v2.Normalize()}, dimension(dim)
{
}

ShapeType Plane::GetType() const
{
  return ePLANE;
}

void Plane::DrawDebug() const
{
  Shape::DrawDebug();
  if (dimension.MagnitudeSq() < FLT_EPSILON)
    return;

  Vec3 corners[4];
  GetCorners(corners);

  Vec4 color = Color::RED;
  if (rb.trigger[body]) color = Color::ORANGE;

  for (size_t i = 0; i < 4; ++i)
  {
    ASC_LINERENDER.AddLine(corners[i], corners[(i + 1) % 4], color);
  }

  //adding rendering to line normal
  Vec3 centre = (corners[2] + corners[0]) * 0.5f;
  ASC_LINERENDER.AddLine(centre, centre + normal * 5.0f);
}

void Plane::Init(size_t body_)
{
  Shape::Init(body_);
  Mat4 rot = QuaternionToMatrix(rb.rotation[body_]);
  for (auto &elem : basis)
    elem = (rot * elem).Normalize();
  normal = (rot * normal).Normalize();
}


void Plane::GetCorners(Vec3(&corners)[4]) const
{
  GetCorners(corners, rb.position[body]);
}

void Plane::GetCorners(Vec3(&corners)[4], Vec3 plpos) const
{
  const Vec3 x = dimension.x * basis[0];
  const Vec3 y = dimension.y * basis[1];

  /*
  * 0 1
  * 3 2
  */
  corners[0] = plpos;
  corners[1] = corners[0] + x;
  corners[2] = corners[0] + x + y;
  corners[3] = corners[0] + y;
}

bool Plane::Ray(const Vec3& pos, const Vec3& nDir, float& t)
{
  return RayToPlane(pos, nDir, *this, t);
}

ShapeType Empty::GetType() const
{
  return eNOCOLLIDER;
}

bool Empty::Ray(const Vec3& pos, const Vec3& nDir, float& t)
{
  return false;
}
