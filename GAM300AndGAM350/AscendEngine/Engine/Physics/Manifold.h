/******************************************************************************/
/*!
\file   Manifold.h
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

struct Manifold
{
  Manifold(size_t a, size_t b);

  void Solve();                       // Generate contact information
  void Initialize();                  // Precalculations for impulse solving
  void ResolveImpulse() const;        // Solve impulse and apply
  void PositionalCorrection() const;  // Naive correction of positional penetration
  void InfiniteMassCorrection() const;

  size_t A;
  size_t B;

  float penetration;     // Depth of penetration from collision
  Vec3 normal;          // From A to B
  Vec3 contacts[2];     // Points of contact during collision
  size_t contact_count; // Number of contacts that occured during collision
  float e;               // Mixed restitution
  float df;              // Mixed dynamic friction
  float sf;              // Mixed static friction
};