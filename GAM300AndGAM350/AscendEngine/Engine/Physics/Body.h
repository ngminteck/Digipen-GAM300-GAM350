/******************************************************************************/
/*!
\file   Body.h
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
static const size_t NUM_PHYSICS = DB_MEMSZ;

template<typename T>
using Array = std::array<T, NUM_PHYSICS>;

struct RigidBodies
{
  Array<Vec3> position;
  Array<Vec3> colliderOffset;
  Array<Quat> rotation;
  Array<Vec3> velocity;
  Array<Vec3> force;

  // Set by shape
  Array<float> m;  // mass
  Array<float> im; // inverse mass
  Array<float> forceMultiplier;

  Array<float> staticFriction;
  Array<float> dynamicFriction;
  Array<float> restitution;
  Array<float> linearDamping;

  Array<bool> isStatic;
  Array<bool> useGravity;
  Array<bool> trigger;
  Array<bool> allowRayCast;
  Array<Shape *>shape;

  // Hold onCollide Enter/Exit/Stay flags
  std::array<Array<size_t>, 2> onCollide;
  Array<size_t> collideWith;

  Array<Handle<PhysicsComp>> componentHandles;
  size_t numCount;
};

struct Shapes
{
  Array<AABB> aabb;
  size_t aabbCount;

  Array<Sphere> sphere;
  size_t sphereCount;

  Array<Plane> plane;
  size_t planeCount;
};

void ApplyImpulse(size_t i, const Vec3 &impulse, const Vec3 &contactVector);
void ApplyForce(size_t id, const Vec3& force);
void ClearAccumulators();