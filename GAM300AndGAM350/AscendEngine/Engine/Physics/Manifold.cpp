/******************************************************************************/
/*!
\file   Manifold.cpp
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

static const float gravityScale = 1.0f;
static const Vec3 gravity(0.0f, -10.0f * gravityScale, 0.0f);

extern RigidBodies rb;

Manifold::Manifold(size_t a, size_t b) :
  A(a), B(b), penetration(0), contact_count(0), e(0), df(0), sf(0)
{
}

void Manifold::Solve()
{
  Dispatch[rb.shape[A]->GetType()][rb.shape[B]->GetType()](this, A, B);
}

void Manifold::Initialize()
{
  // Calculate average restitution
  e = min(rb.restitution[A], rb.restitution[B]);

  // Calculate static and dynamic friction
  sf = sqrt(rb.staticFriction[A] * rb.staticFriction[B]);
  df = sqrt(rb.dynamicFriction[A] * rb.dynamicFriction[B]);

  for (size_t i = 0; i < contact_count; ++i)
  {
    // Calculate radii from COM to contact
    Vec3 ra_ = contacts[i] - rb.position[A];
    Vec3 rb_ = contacts[i] - rb.position[B];

    Vec3 rv = rb.velocity[B] - rb.velocity[A] /*+ rb.angularVelocity[B].Cross(rb_) - rb.angularVelocity[A].Cross(ra_)*/;

    // Determine if we should perform a resting collision or not
    // The idea is if the only thing moving this object is gravity,
    // then the collision should be performed without any restitution
    static const float gravityConstant = (0.01f * gravity).MagnitudeSq();
    if (rv.MagnitudeSq() < gravityConstant + ASC_EPSILON)
      e = 0.0f;
  }
}

void Manifold::ResolveImpulse() const
{
  // Early out and positional correct if both objects have infinite mass
  if (fabs(rb.im[A] + rb.im[B]) < FLT_EPSILON)
  {
    InfiniteMassCorrection();
    return;
  }

  for (size_t i = 0; i < contact_count; ++i)
  {
    //assert(!(isnan(rb.velocity[A].x) || isnan(rb.velocity[A].y) || isnan(rb.velocity[A].z)));
    //assert(!(isnan(rb.velocity[B].x) || isnan(rb.velocity[B].y) || isnan(rb.velocity[B].z)));

    // Calculate radii from COM to contact
    Vec3 radA = contacts[i] - rb.position[A];
    Vec3 radB = contacts[i] - rb.position[B];

    // Relative velocity
    Vec3 rv = rb.velocity[B] - rb.velocity[A];

    // Relative velocity along the normal
    //TODO: Check why normal is NAN
    //assert(!(isnan(normal.x) || isnan(normal.y) || isnan(normal.z)));

    float contactVel = rv.Dot(normal);

    // Do not resolve if velocities are separating
    if (contactVel > 0.0f)
      return;

    //Vec3 raCrossN = ra_.Cross(normal);
    //Vec3 rbCrossN = rb_.Cross(normal);
    float invMassSum = rb.im[A] + rb.im[B];

    // Calculate impulse scalar
    float j = -(1.0f + e) * contactVel;
    j /= invMassSum;
    j /= static_cast<float>(contact_count);

    // Apply impulse
    Vec3 impulse = normal * j;
    //assert(!(isnan(rb.velocity[A].x) && isnan(rb.velocity[A].y) && isnan(rb.velocity[A].z)));
    ApplyImpulse(A, -impulse, radA);
    ApplyImpulse(B,  impulse, radB);

    // Friction impulse
    rv = rb.velocity[B] /*+ rb.angularVelocity[B].Cross(rb_)*/ - rb.velocity[A] /*- rb.angularVelocity[A].Cross(ra_)*/;

    Vec3 t = rv - (normal * rv.Dot(normal));
    t = t.Normalize();

    // j tangent magnitude
    float jt = -rv.Dot(t);
    //assert(!isinf(jt));
    jt /= invMassSum;
    jt /= static_cast<float>(contact_count);

    //assert(!isinf(jt));

    // Don't apply tiny friction impulses
    if (fabs(jt) < FLT_EPSILON)
      return;

    // Coulumb's law
    Vec3 tangentImpulse;
    if (fabs(jt) < j * sf)
      tangentImpulse = t * jt;
    else
      tangentImpulse = t * -j * df;

    //assert(!(isnan(tangentImpulse.x) && isnan(tangentImpulse.y) && isnan(tangentImpulse.z)));
    //Apply friction impulse
    //tangentImpulse.y = 0.0f;
    //tangentImpulse.x = fabs(tangentImpulse.x) < FLT_EPSILON ? 0.0f : tangentImpulse.x;
    //tangentImpulse.y = fabs(tangentImpulse.y) < FLT_EPSILON ? 0.0f : tangentImpulse.y;
    //tangentImpulse.z = fabs(tangentImpulse.z) < FLT_EPSILON ? 0.0f : tangentImpulse.z;
    tangentImpulse.y = 0.0f;

    ApplyImpulse(A, -tangentImpulse, radA);
    ApplyImpulse(B, tangentImpulse, radB);
  }
}

void Manifold::PositionalCorrection() const
{
  const float k_slop = 0.1f; // Penetration allowance
  const float percent = 0.01f; // Penetration percentage to correct
  Vec3 correction = max(penetration - k_slop, 0.0f) / (rb.im[A] + rb.im[B]) * normal * percent;
  rb.position[A] -= correction * rb.im[A];
  rb.position[B] += correction * rb.im[B];
}

void Manifold::InfiniteMassCorrection() const
{
  rb.velocity[A].Set();
  rb.velocity[B].Set();
}