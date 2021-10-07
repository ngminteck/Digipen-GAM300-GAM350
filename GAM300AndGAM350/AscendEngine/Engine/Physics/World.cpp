/******************************************************************************/
/*!
\file   World.cpp
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

RigidBodies rb;
Shapes shapes;

static std::vector<Manifold> contacts;
static ObjectPool<AABB> aabbPool(NUM_PHYSICS);
static ObjectPool<Sphere> spherePool(NUM_PHYSICS);
static ObjectPool<Plane> planePool(NUM_PHYSICS);
static ObjectPool<Empty> emptyPool(NUM_PHYSICS);

static const float gravityScale = 9.8f;
static const Vec3 gravity(0.0f, -10.0f * gravityScale, 0.0f);

static bool currentState = false;

static void ClearCollision()
{
  // Clear all contacts
  contacts.clear();
  currentState = !currentState;

  // Clear current collide state
  ZeroMem(rb.onCollide[currentState]);
}

static void SetCollision(Manifold &m)
{
  if (m.contact_count)
  {
    int typeA = 1 << rb.shape[m.A]->GetType();
    int typeB = 1 << rb.shape[m.B]->GetType();

    rb.onCollide[currentState][m.A] |= typeB;
    rb.onCollide[currentState][m.B] |= typeA;

    rb.collideWith[m.A] = m.B;
    rb.collideWith[m.B] = m.A;
  }
}

static void ProcessCollision()
{
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    // Draw debug info
    size_t prevFlag = rb.onCollide[!currentState][i];
    size_t currFlag = rb.onCollide[currentState][i];

    GOHandle gameObject = ASC_PHYSICSMANAGER.GetGameObjectHandleFromBody(i);
    GOHandle other = ASC_PHYSICSMANAGER.GetGameObjectHandleFromBody(rb.collideWith[i]);

    //if (gameObject.IsValid() && other.IsValid())
    {
      // OnCollideEnter
      if (currFlag & ~prevFlag)
      {
        ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_ENTER, gameObject, other));
      }
      // OnCollide
      else if (currFlag)
      {
        ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_STAY, gameObject, other));
      }

      // OnCollideExit
      if (~currFlag & prevFlag)
      {
        ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_EXIT, gameObject, other));
      }
    }
  }
}

static void IntegrateForces(float dt)
{
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    if (fabs(rb.im[i]) < FLT_EPSILON ||
       rb.isStatic[i])
      continue;

    static Vec3 zero = Vec3(0.0f, 0.0f, 0.0f);
    const Vec3 &g = rb.useGravity[i] ? gravity : zero;

    rb.velocity[i] += (rb.force[i] * rb.forceMultiplier[i] * rb.im[i] + g) * dt;
    //assert(!(isnan(rb.velocity[i].x) || isnan(rb.velocity[i].y) || isnan(rb.velocity[i].z)));
  }
}

static void IntegrateVelocity(float dt)
{
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    if (fabs(rb.im[i]) < FLT_EPSILON ||
        rb.isStatic[i])
      continue;

    // Impose drag.
    rb.velocity[i] *= pow(rb.linearDamping[i], dt);
    //assert(!(isnan(rb.velocity[i].x) || isnan(rb.velocity[i].y) || isnan(rb.velocity[i].z)));
    rb.position[i] += rb.velocity[i] * dt;
  }

  IntegrateForces(dt);
}

void InitCollision()
{
  ZeroMem(rb.onCollide[currentState]);
  ZeroMem(rb.onCollide[!currentState]);
  ZeroMem(rb.collideWith);
}

void Step(float dt)
{
  //static const float fixedDT = 1.0f / 60.0f;

  ClearCollision();

  // Generate new collision info
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    for (size_t j = i + 1; j < rb.numCount; ++j)
    {
      if ((fabs(rb.im[i]) < FLT_EPSILON ||
          rb.isStatic[i]) &&
          (fabs(rb.im[j]) < FLT_EPSILON ||
          rb.isStatic[j]))
        continue;

      Manifold m(i, j);
      m.Solve();
      SetCollision(m);

      if (rb.trigger[i] ||
          rb.trigger[j])
        continue;

      if (m.contact_count)
        contacts.emplace_back(m);
    }
  }

  ProcessCollision();

  // Integrate forces
  IntegrateForces(dt);

  // Initialize collision
  for (size_t i = 0; i < contacts.size(); ++i)
    contacts[i].Initialize();

  // Solve collisions
  for (size_t j = 0; j < 10; ++j)
    for (auto &contact : contacts)
      contact.ResolveImpulse();

  // Integrate velocities
  IntegrateVelocity(dt);

  // Correct positions
  for (auto &contact : contacts)
    contact.PositionalCorrection();

  // Clear all forces
  ClearAccumulators();
}

void AddBody(PhysicsComp &physicsComponent)
{
  size_t i = rb.numCount;
  ++rb.numCount;

  rb.position[i].Set();
  rb.velocity[i].Set();
  rb.force[i].Set();

  rb.useGravity[i] = true;
  rb.trigger[i] = false;
  rb.allowRayCast[i] = true;

  rb.forceMultiplier[i] = 1.0f;
  rb.staticFriction[i] = 0.4f;
  rb.dynamicFriction[i] = 0.2f;
  rb.restitution[i] = 1.0f;
  rb.linearDamping[i] = 0.005f;
  rb.m[i] = 8.0f;
  rb.im[i] = 1.0f / 8.0f;
  rb.componentHandles[i] = physicsComponent.GetSelfHandle();
  physicsComponent.id = i;
}

void AddShape(PhysicsComp& physicsComponent, ShapeType type)
{
  size_t id = physicsComponent.id;
  Shape *newShape = nullptr;
  RemoveShape(rb.shape[id]);

  switch (type)
  {
  case eSPHERE:
  {
    newShape = &spherePool.acquireObject();
    //newShape = &shapes.sphere[shapes.sphereCount];
    //newShape = new Sphere;
    //++shapes.sphereCount;
    newShape->Init(id);
    //assert(shapes.sphereCount < NUM_PHYSICS);
  } break;

  case eAABB:
  {
    newShape = &aabbPool.acquireObject();
    //newShape = &shapes.aabb[shapes.aabbCount];
    //newShape = new AABB;
    //++shapes.aabbCount;
    newShape->Init(id);
    //assert(shapes.aabbCount < NUM_PHYSICS);
  } break;

  case ePLANE:
  {
    rb.m[id] = 0.0f;
    rb.im[id] = 0.0f;
    newShape = &planePool.acquireObject();
    //newShape = &shapes.plane[shapes.planeCount];
    //newShape = new Plane;
    //++shapes.planeCount;
    newShape->Init(id);
    //assert(shapes.planeCount < NUM_PHYSICS);
  } break;

  case eNOCOLLIDER:
  {
    newShape = &emptyPool.acquireObject();
    newShape->Init(id);
  } break;

  case eMAX: break;
  default: break;
  }

  //assert(newShape != nullptr);
  rb.shape[id] = newShape;
  //CHECK();
}

#if 0
void RemoveShape(Shape **shape)
{
  if (*shape && (*shape)->GetType() != eNOCOLLIDER)
    delete *shape;
  *shape = nullptr;
#else
void RemoveShape(Shape *shape)
{
  if (!shape) return;

  switch (shape->GetType())
  {
  case eSPHERE:
  {
    Sphere &sphere = *static_cast<Sphere *>(shape);
    spherePool.releaseObject(sphere);
  } break;

  case eAABB:
  {
    AABB &aabb = *static_cast<AABB *>(shape);
    aabbPool.releaseObject(aabb);
  } break;

  case ePLANE:
  {
    Plane &plane = *static_cast<Plane *>(shape);
    planePool.releaseObject(plane);
  } break;

  case eNOCOLLIDER:
  {
    Empty &empty = *static_cast<Empty *>(shape);
    emptyPool.releaseObject(empty);
  } break;

  case eMAX: break;
  default: break;
  }

  //void *arr = nullptr;
  //auto fn = [shape](auto begin, auto end)
  //{
  //  while (begin++ != end)
  //  {
  //    if (begin._Ptr == shape)
  //    {
  //      std::iter_swap(begin, end);
  //      break;
  //    }
  //  }
  //};

  ////shape->body = -1;

  //switch (shape->GetType())
  //{
  //case eSPHERE:
  //{
  //  arr = &shapes.sphere;
  //  auto begin = reinterpret_cast<Array<Sphere> *>(arr)->begin();
  //  auto end = reinterpret_cast<Array<Sphere> *>(arr)->begin() + shapes.sphereCount;
  //  fn(begin, end);

  //  --shapes.sphereCount;
  //} break;

  //case eAABB:
  //{
  //  arr = &shapes.aabb;
  //  auto begin = reinterpret_cast<Array<AABB> *>(arr)->begin();
  //  auto end = reinterpret_cast<Array<AABB> *>(arr)->begin() + shapes.aabbCount;
  //  fn(begin, end);

  //  --shapes.aabbCount;
  //} break;

  //case ePLANE:
  //{
  //  arr = &shapes.plane;
  //  auto begin = reinterpret_cast<Array<Plane> *>(arr)->begin();
  //  auto end = reinterpret_cast<Array<Plane> *>(arr)->begin() + shapes.planeCount;
  //  fn(begin, end);

  //  --shapes.planeCount;
  //} break;

  //default:
  //{
  //} break;
  //}

  //CHECK();
  //assert(arr != nullptr);
#endif
}

void ClearCurrentCollision(size_t body)
{
  rb.onCollide[currentState][body] = 0;
  rb.collideWith[body] = 0;
}
