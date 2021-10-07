/******************************************************************************/
/*!
\file   Body.cpp
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

void ClearAccumulators()
{
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    rb.force[i].Set();
    //rb.torque[i].Set();
  }
}

void ApplyForce(size_t id, const Vec3& force)
{
  rb.force[id] += force;
}

void ApplyImpulse(size_t i, const Vec3 &impulse, const Vec3 &contactVector)
{
  if (rb.isStatic[i])
    return;

  rb.velocity[i] += rb.im[i] * impulse;
  //assert(!(isnan(rb.velocity[i].x) || isnan(rb.velocity[i].y) || isnan(rb.velocity[i].z)));
  //rb.angularVelocity[i] += rb.iI[i] * contactVector.Cross(impulse);
}