/******************************************************************************/
/*!
\file   PhysicsManager.cpp
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
extern Shapes shapes;

static bool isEnabled = false;

PhysicsManager::PhysicsManager(HandleManager<TransformComp>& transform_comps) :
  m_transform(transform_comps)
{
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &PhysicsManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &PhysicsManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_PAUSE, &PhysicsManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_RESUME, &PhysicsManager::HandleMsg, this);


}

PhysicsManager::~PhysicsManager()
{
  PhysicsManager::UnLoad();
}



void PhysicsManager::Load()
{
  InitCollision();
#if 0
  auto &obj = ASC_FACTORY.AddObject();
  obj.name = "Default plane";
  ASC_FACTORY.SetGOType(obj.GetSelfHandle(), go_floor);
  ASC_FACTORY.AddComponent(obj, GRAPHICS, PHYSICS);
  PhysicsComp* physics = ASC_FACTORY.GetComponent<PhysicsComp>(obj.GetSelfHandle());
  TransformComp* trans = ASC_FACTORY.GetComponent<TransformComp>(obj.GetSelfHandle());
  GraphicsComp* graphics = ASC_FACTORY.GetComponent<GraphicsComp>(obj.GetSelfHandle());
  trans->pos = Vec3(-200.0f, -0.1f, -200.0f);
  trans->scale = Vec3(400.0f, 0.1f, 400.0f);
  //graphics->tint = Vec4(0.396078431372549f, 0.6117647058823529f, 0.9372549019607843f, 1.0f);
  //graphics->tint = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  AddShape(*physics, ePLANE);
#endif
}

void PhysicsManager::UnLoad()
{
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    //RemoveShape(rb.shape[i]);
  }

  rb.numCount = 0;
  m_components.Clear();
}

void PhysicsManager::Update(float dt)
{
  if (isEnabled)
  {
    start = Clock::now();
    for (size_t i = 0; i < rb.numCount; ++i)
    {
      PhysicsComp &elem = m_components.m_objects[i];
      TransformComp &transform = *m_transform[elem.GetTransHdl()];
      if (elem.Enabled && elem.GetGOHandle()->GetIsEnable())
      {
        rb.position[i] = transform.pos + rb.colliderOffset[i];
        rb.rotation[i] = transform.rotation;
      }
    }

    Step(dt);

    //for (size_t i = 0; i < rb.numCount; ++i)
    //{
    //  PhysicsComp &elem = m_components.m_objects[i];
    //  TransformComp &transform = *m_transform[elem.GetTransHdl()];
    //  transform.pos = rb.position[i] - rb.colliderOffset[i];
    //  transform.rotation = rb.rotation[i];
    //  transform.dirty = true;
    //}

    duration = Clock::now() - start;
  }

#if !_SUBMISSION
  for (size_t i = 0; i < rb.numCount; ++i)
  {
    PhysicsComp &elem = m_components.m_objects[i];
    TransformComp &transform = *m_transform[elem.GetTransHdl()];
    if (elem.Enabled && elem.GetGOHandle()->GetIsEnable())
    {
      if (rb.shape[i]->GetType() == ePLANE)
      {
        auto* plane = static_cast<Plane*>(rb.shape[i]);
        auto mat = QuaternionToMatrix(transform.rotation).Transpose();
        plane->normal = mat * Vec3::YAXIS;
        plane->basis[0] = mat * Vec3::XAXIS;
        plane->basis[1] = mat * Vec3::ZAXIS;
      }
    }
  }
#endif
}

BaseHandle PhysicsManager::Deserialize(const GameObject & gameObject, Deserializer &deserializer)
{
  PhyHandle hdl(CreateComponent());
  PhysicsComp &physicsComp = *GetComponent(hdl);
  physicsComp.SetGOHandle(gameObject.GetSelfHandle());
  physicsComp.SetTransHdl(Handle<TransformComp>(gameObject[TRANSFORM]));

  rb.velocity[physicsComp.id].Set();

  //deserializer.Read("Force", rb.force[physicsComp.id]);
  deserializer.Read("IsEnabled", physicsComp.Enabled);

  deserializer.Read("Mass", rb.m[physicsComp.id]);
  deserializer.Read("InverseMass", rb.im[physicsComp.id]);

  deserializer.Read("StaticFriction", rb.staticFriction[physicsComp.id]);
  deserializer.Read("DynamicFriction", rb.dynamicFriction[physicsComp.id]);
  deserializer.Read("Restitution", rb.restitution[physicsComp.id]);
  deserializer.Read("LinearDamping", rb.linearDamping[physicsComp.id]);
  deserializer.Read("Static", rb.isStatic[physicsComp.id]);
  deserializer.Read("Trigger", rb.trigger[physicsComp.id]);
  deserializer.Read("UseGravity", rb.useGravity[physicsComp.id]);
  deserializer.Read("AllowRayCast", rb.allowRayCast[physicsComp.id]);

  int type = -1;
  deserializer.Read("ColliderType", type);

  if (type == eMAX)
  {
    type = eSPHERE;
    LogWarning("Using Sphere colliders instead.");
  }

  AddShape(physicsComp, static_cast<ShapeType>(type));

  switch (rb.shape[physicsComp.id]->GetType())
  {
  case eSPHERE:
  {
    deserializer.Read("Radius", static_cast<Sphere *>(rb.shape[physicsComp.id])->radius);
  } break;

  case eAABB:
  {
    deserializer.Read("Min", static_cast<AABB *>(rb.shape[physicsComp.id])->min);
    deserializer.Read("Max", static_cast<AABB *>(rb.shape[physicsComp.id])->max);
  } break;

  case ePLANE:
  {
    // No modify
    Vec3 &normal = static_cast<Plane *>(rb.shape[physicsComp.id])->normal;
    Vec3 &x = static_cast<Plane *>(rb.shape[physicsComp.id])->basis[0];
    Vec3 &z = static_cast<Plane *>(rb.shape[physicsComp.id])->basis[1];
    Vec2 &dimension = static_cast<Plane *>(rb.shape[physicsComp.id])->dimension;

    deserializer.Read("Normal", normal);
    deserializer.Read("X", x);
    deserializer.Read("Z", z);
    deserializer.Read("Dimension", dimension);
  } break;

  default: break;
  }

  deserializer.Read("ForceMultiplier", rb.forceMultiplier[physicsComp.id]);
  deserializer.Read("ColliderOffset", rb.colliderOffset[physicsComp.id]);
  deserializer.Read("CollisionFilterList", physicsComp.collision_filter_list);


  //deserializer.Read();
  return hdl;
}

void PhysicsManager::Serialize(Serializer &serializer, BaseHandle handle)
{
  if (m_components.m_objects.empty()) return;

  // Get the current transform to serialize
  PhysicsComp &physicsComp = *GetComponent(handle);

  serializer.Write("IsEnabled",physicsComp.Enabled);

  serializer.Write("Force", rb.force[physicsComp.id]);

  serializer.Write("Mass", rb.m[physicsComp.id]);
  serializer.Write("InverseMass", rb.im[physicsComp.id]);

  serializer.Write("StaticFriction", rb.staticFriction[physicsComp.id]);
  serializer.Write("DynamicFriction", rb.dynamicFriction[physicsComp.id]);
  serializer.Write("Restitution", rb.restitution[physicsComp.id]);
  serializer.Write("LinearDamping", rb.linearDamping[physicsComp.id]);

  serializer.Write("Static", rb.isStatic[physicsComp.id]);
  serializer.Write("Trigger", rb.trigger[physicsComp.id]);
  serializer.Write("UseGravity", rb.useGravity[physicsComp.id]);
  serializer.Write("AllowRayCast", rb.allowRayCast[physicsComp.id]);

  auto type = eMAX;
  if (rb.shape[physicsComp.id])
  {
    type = rb.shape[physicsComp.id]->GetType();
  }
  else
  {
    LogWarning("No collider in physics component, using sphere instead");
  }

  serializer.Write("ColliderType", type);

  switch (type)
  {
  case eSPHERE:
  {
    serializer.Write("Radius", static_cast<Sphere *>(rb.shape[physicsComp.id])->radius);
  } break;

  case eAABB:
  {
    serializer.Write("Min", static_cast<AABB *>(rb.shape[physicsComp.id])->min);
    serializer.Write("Max", static_cast<AABB *>(rb.shape[physicsComp.id])->max);
  } break;

  case ePLANE:
  {
    // No modify
    Vec3 normal = static_cast<Plane *>(rb.shape[physicsComp.id])->normal;
    Vec3 x = static_cast<Plane *>(rb.shape[physicsComp.id])->basis[0];
    Vec3 z = static_cast<Plane *>(rb.shape[physicsComp.id])->basis[1];

    serializer.Write("Normal", normal);
    serializer.Write("X", x);
    serializer.Write("Z", z);
    serializer.Write("Dimension", static_cast<Plane *>(rb.shape[physicsComp.id])->dimension);
  } break;

  default: break;
  }

  serializer.Write("ForceMultiplier", rb.forceMultiplier[physicsComp.id]);
  serializer.Write("ColliderOffset", rb.colliderOffset[physicsComp.id]);
  serializer.Write("CollisionFilterList", physicsComp.collision_filter_list);
 

}

void PhysicsManager::Reflect(BaseHandle hdl, const Reflector &reflector)
{
  PhysicsComp &physicsComponent = *m_components[PhyHandle(hdl)];
  size_t id = physicsComponent.id;

  reflector.Reflect("Gravity", rb.useGravity[id]);
  reflector.Reflect("Trigger", rb.trigger[id]);
  reflector.Reflect("AllowRaycast", rb.allowRayCast[id]);
  reflector.Reflect("Static", rb.isStatic[id]);

  float mass = rb.m[id];
  if (reflector.Reflect("Mass", mass))
  {
    rb.m[id] = mass;
    rb.im[id] = fabs(mass) < FLT_EPSILON ? 0.0f : 1.0f / mass;
  }

  reflector.Reflect("ForceMultiplier", rb.forceMultiplier[id]);

  // Not meant to change
  Vec3 velocity = rb.velocity[id];
  reflector.Reflect("Velocity", velocity);

  reflector.Reflect("Force", rb.force[id]);
  reflector.Reflect("Restitution", rb.restitution[id]);
  ASC_EDITOR.HelpMarker("Bounciness", true);
  reflector.Reflect("StaticFriction", rb.staticFriction[id]);
  reflector.Reflect("DynamicFriction", rb.dynamicFriction[id]);
  reflector.Reflect("LinearDamping", rb.linearDamping[id]);
  ASC_EDITOR.HelpMarker("Linear damping will slow down an object moving in any direction", true);

  static std::vector<std::string> colliderTypes{ "Sphere", "AABB", "Plane", "NoCollider" };
  int type = eMAX;
  Shape *shape = rb.shape[id];
  if (shape)
  {
    type = shape->GetType();
  }

  if (reflector.Reflect("Type", colliderTypes, &type))
  {
    if (!shape || type != shape->GetType())
    {
      AddShape(physicsComponent, static_cast<ShapeType>(type));
    }
  }

  switch (type)
  {
  case eSPHERE:
  {
    reflector.Reflect("Radius", static_cast<Sphere *>(rb.shape[id])->radius);
  } break;

  case eAABB:
  {
    AABB* aabb = static_cast<AABB *>(rb.shape[id]);
    Vec3 &minBox(aabb->min), &maxBox(aabb->max);
    Vec3 size = maxBox - minBox;

    reflector.Reflect("Size", size);

    size *= 0.5f;
    minBox = -size;
    maxBox = size;

  } break;

  case ePLANE:
  {
    static Quat quat;
    Plane *plane = static_cast<Plane *>(rb.shape[id]);
    auto rot = m_transform[physicsComponent.GetTransHdl()]->rotation;
    if (quat != rot)
    {
      quat = rot;
      auto mat = QuaternionToMatrix(quat).Transpose();
      plane->normal = mat * Vec3::YAXIS;
      plane->basis[0] = mat * Vec3::XAXIS;
      plane->basis[1] = mat * Vec3::ZAXIS;
    }

    reflector.Reflect("Dimension", plane->dimension);
    /*reflector.Reflect("Normal", plane->normal);
    reflector.Reflect("B1", plane->basis[0]);
    reflector.Reflect("B2", plane->basis[1]);*/
  } break;

  default: break;
  }

 
  reflector.Reflect("ColliderOffset", rb.colliderOffset[id]);


  ImGui::Columns(2);
  ImGui::TextColored({ 0,10,255, 1 }, "Collision Type");
  ImGui::NextColumn();
  
  int collisiontypeindex = -1;
  std::vector<const char*> collisiontypelist{ "STATIC", "DYNAMIC", "TRIGGER", "PLAYER","OTHER" };

  uint32_t collisiontype_counter = 0;
  for (const auto& str : collisiontypelist)
  {
      std::string tostring = str;
      if (physicsComponent.GetCollisionType().compare(tostring)==0)
      {
          collisiontypeindex = collisiontype_counter;
          break;
      }
      ++collisiontype_counter;
  }

  if (ImGui::Combo("##currcollisiontype", &collisiontypeindex, collisiontypelist.data(), (uint32_t)collisiontypelist.size()))
  {
      physicsComponent.SetCollisionType(collisiontypelist[collisiontypeindex]);
  }

  ImGui::Columns(1);
  ImGui::TextColored({ 0,10,255, 1 }, "Collision Filter Table");

  std::vector<const char*> collisionresponselist{ "IGNORE", "OVERLAPPING", "BLOCK" };

  ImGui::Columns(2);
  for (auto it = physicsComponent.collision_filter_list.begin(); it != physicsComponent.collision_filter_list.end(); ++it)
  {
      ImGui::TextColored({ 0,10,255, 1 }, it->first.c_str());
      ImGui::NextColumn();

      int currresponseindex= -1;
      uint32_t responsecounter = 0;

      for (const auto& str : collisionresponselist)
      {
          std::string tostring = str;
          if (it->second.compare(tostring) == 0)
          {
              currresponseindex = responsecounter;
              break;
          }
          ++responsecounter;
      }

      std::stringstream imguilabel;
      imguilabel.str(std::string());
      imguilabel <<"##collisionrespone" << it->first;

      if (ImGui::Combo(imguilabel.str().c_str(), &currresponseindex, collisionresponselist.data(), (uint32_t)collisionresponselist.size()))
      {
          it->second = collisionresponselist[currresponseindex];
      }

      ImGui::NextColumn();

  }
  ImGui::Columns(1);

}

BaseHandle PhysicsManager::CreateComponent()
{
  PhyHandle hdl = m_components.GetNewObject();
  PhysicsComp &physicsComponent = *m_components[hdl];

  AddBody(physicsComponent);
  AddShape(physicsComponent, eSPHERE);

  //CHECK();
  return hdl;
}

void PhysicsManager::DestroyComponent(BaseHandle hdl)
{
  PhyHandle phdl(hdl);
  PhysicsComp& pc = *m_components[phdl];
  size_t rbhandle = pc.id;

  //swap and replace
  PhysicsComp& lastComp = *m_components[rb.componentHandles[rb.numCount - 1]];

  //replace 
  SwapReplaceRb(rbhandle, lastComp.id);
  lastComp.id = rbhandle;
  //CHECK();
  m_components.ReleaseObj(phdl);
}

void PhysicsManager::InitComponent(const GameObject & go, BaseHandle hdl)
{
  PhyHandle phdl(hdl);
  m_components[phdl]->SetGOHandle(go.GetSelfHandle());
  PhysicsComp* newcomp = m_components[phdl];
  newcomp->SetTransHdl(Handle<TransformComp>(go[TRANSFORM]));
}

void PhysicsManager::CopyComponentToComponent(PhysicsComp & target, const PhysicsComp & source)
{
  size_t i = rb.numCount++;
  target.id = i;
  CopyRb(i, source.id);
  AddShape(target, static_cast<ShapeType>(rb.shape[source.id]->GetType()));
  switch (rb.shape[source.id]->GetType())
  {
  case eSPHERE:
  {
    auto *sourceSphere = static_cast<Sphere *>(rb.shape[source.id]);
    auto *targetSphere = static_cast<Sphere *>(rb.shape[target.id]);
    targetSphere->radius = sourceSphere->radius;
  } break;

  case eAABB:
  {
    auto *sourceAABB = static_cast<AABB *>(rb.shape[source.id]);
    auto *targetAABB = static_cast<AABB *>(rb.shape[target.id]);
    targetAABB->min = sourceAABB->min;
    targetAABB->max = sourceAABB->max;
  } break;

  case ePLANE:
  {
    auto *sourcePlane = static_cast<Plane *>(rb.shape[source.id]);
    auto *targetPlane = static_cast<Plane *>(rb.shape[target.id]);
    targetPlane->normal = sourcePlane->normal;
    targetPlane->basis[0] = sourcePlane->basis[0];
    targetPlane->basis[1] = sourcePlane->basis[1];
    targetPlane->dimension = sourcePlane->dimension;
  } break;
  case eMAX: break;
  default: break;
  }

  target.collision_type = source.collision_type;

  for (auto it = source.collision_filter_list.begin(); it != source.collision_filter_list.end(); ++it)
  {
      target.collision_filter_list.emplace(it->first, it->second);
  }
  rb.componentHandles[i] = target.GetSelfHandle();
  //CHECK();
}

void PhysicsManager::HandleMsg(MsgData_Base *mb)
{
  switch (mb->type)
  {
  case mTYPE_STOP:
  case mTYPE_GAME_PAUSE:
  {
    isEnabled = false;
  } break;

  case mTYPE_PLAY:
  case mTYPE_GAME_RESUME:
  {
    isEnabled = true;
  } break;

  default: break;
  }
}

GOHandle PhysicsManager::GetGameObjectHandleFromBody(size_t i)
{
  PhyHandle handle = rb.componentHandles[i];
  return handle.IsValid() ? GetComponent(handle)->GetGOHandle() : GOHandle();
}

bool PhysicsManager::RayCast(const Vec3 &pos, const Vec3 &dir, const GOHandle go, float &t)
{
  float oldTime = FLT_MAX;
  t = FLT_MAX;

  Vec3 nDir = dir.Normalize();

  auto gameObject = ASC_FACTORY.GetGameObjectPtr(go);
  PhyHandle hdl = gameObject->GetHdl<PhysicsComp>();

  if (hdl.IsValid())
  {
    PhysicsComp* pc = ASC_PHYSICSMANAGER.GetComponent(hdl);
    assert(pc);

    size_t id = pc->id;
    if (rb.allowRayCast[id] && rb.shape[id]->Ray(pos, nDir, oldTime))
    {
      t = min(oldTime, t);
      return true;
    }
  }
  return false;
}

GOHandle PhysicsManager::RayCast(const Vec3 &pos, const Vec3 &dir, float &t)
{
  float oldTime = FLT_MAX;
  t = FLT_MAX;

  const Vec3 nDir = dir.Normalize();
  int hitobj = -1;
  for (int i = 0; i < rb.numCount; ++i)
  {
    if (rb.allowRayCast[i] &&
        rb.shape[i]->Ray(pos, nDir, oldTime))
    {
      if (oldTime > 0.0f &&
          oldTime < t)
      {
        t = oldTime;
        hitobj = i;
      }
    }
  }

  GOHandle retObj;
  if ((fabs(FLT_MAX - oldTime) > FLT_EPSILON) && hitobj != -1)
  {
    retObj = ASC_PHYSICSMANAGER.GetComponent(rb.componentHandles[hitobj])->GetGOHandle();
  }
  return retObj;
}

void PhysicsManager::CopyRb(size_t a, size_t b)
{
  rb.position[a] = rb.position[b];
  rb.velocity[a] = rb.velocity[b];
  rb.isStatic[a] = rb.isStatic[b];
  rb.trigger[a] = rb.trigger[b];
  rb.useGravity[a] = rb.useGravity[b];
  rb.allowRayCast[a] = rb.allowRayCast[b];
  rb.force[a] = rb.force[b];
  rb.m[a] = rb.m[b];
  rb.im[a] = rb.im[b];
  rb.staticFriction[a] = rb.staticFriction[b];
  rb.dynamicFriction[a] = rb.dynamicFriction[b];
  rb.restitution[a] = rb.restitution[b];
  rb.linearDamping[a] = rb.linearDamping[b];
  rb.forceMultiplier[a] = rb.forceMultiplier[b];
  rb.colliderOffset[a] = rb.colliderOffset[b];
}

void PhysicsManager::SwapReplaceRb(size_t a, size_t b)
{
  CopyRb(a, b);
  RemoveShape(rb.shape[a]);
  rb.shape[a] = rb.shape[b];
  if (rb.shape[a])
    rb.shape[a]->body = a;
  rb.shape[b] = nullptr;
  rb.componentHandles[a] = rb.componentHandles[b];
  ClearCurrentCollision(a);
  rb.force[b].Set();
  rb.velocity[b].Set();
  rb.colliderOffset[b].Set();

  //ResetRB(b);
  --rb.numCount;

  //CHECK();
}

void PhysicsComp::SetGravity(bool val) const
{
  rb.useGravity[id] = val;
}

void PhysicsComp::SetMass(float val) const
{
  rb.m[id] = val;
}

float PhysicsComp::GetMass() const
{
  return rb.m[id];
}

void PhysicsComp::AddForce(const Vec3 &force) const
{
  ApplyForce(id, force);
}

void PhysicsComp::AddVelocity(const Vec3 & vel)
{
  rb.velocity[id] += vel;
}

void PhysicsComp::AddShape(int shape)
{
  ::AddShape(*this, static_cast<ShapeType>(shape));
}

float PhysicsComp::GetRadius() const
{
  float radius = 0.0f;
  switch (rb.shape[id]->GetType())
  {
  case eAABB:
  {
    AABB &aabb = static_cast<AABB &>(*rb.shape[id]);
    float x = aabb.max.x - aabb.min.x;
    float y = aabb.max.y - aabb.min.y;
    float z = aabb.max.z - aabb.min.z;

    radius = max(x, max(y, z)) * 0.5f;
  } break;

  case eSPHERE:
  {
    Sphere &sphere = static_cast<Sphere &>(*rb.shape[id]);
    radius = sphere.radius;
  } break;

  default: break;
  }

  return radius;
}

Vec3 PhysicsComp::GetVelocity() const
{
  return rb.velocity[id];
}

float PhysicsComp::GetForceMuliplier() const
{
  return rb.forceMultiplier[id];
}

void PhysicsComp::SetDynFriction(float val) const
{
  rb.dynamicFriction[id] = val;
}

void PhysicsComp::Stop() const
{
  rb.velocity[id] = Vec3();
  rb.force[id] = Vec3();
}

void PhysicsComp::SetStatic(bool flag) const
{
  rb.isStatic[id] = flag;
}

void PhysicsComp::SetVelocity(Vec3 vel)
{
  //use vel instead of force
  rb.velocity[id] = vel;
  rb.force[id] = Vec3();
}

void PhysicsComp::SetCollisionType(std::string new_collision_type)
{
    collision_type = new_collision_type;
}

std::string PhysicsComp::GetCollisionType() const
{
    return collision_type;
}


