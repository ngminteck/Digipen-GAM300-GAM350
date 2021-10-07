/******************************************************************************/
/*!
\file   PhysicsManager.h
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

typedef ComponentManagerBase<PhysicsComp>::COMPHANDLETYPE PhyHandle;

#define ASC_PHYSICSMANAGER PhysicsManager::Inst()
class PhysicsManager : public ComponentManagerBase<PhysicsComp>, public Singleton<PhysicsManager>
{
public:
  explicit PhysicsManager(HandleManager<TransformComp>& transform_comps);
  ~PhysicsManager();

  void Load() override;
  void UnLoad() override;

  void Update(float dt) override;

  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;

  BaseHandle CreateComponent() override;
  void DestroyComponent(BaseHandle hdl) override;
  void InitComponent(const GameObject& go, BaseHandle hdl) override;

  //function from CompManBase
  void CopyComponentToComponent(PhysicsComp& target, const PhysicsComp& source) override;

  //For Msging
  void HandleMsg(MsgData_Base *) override;
  GOHandle GetGameObjectHandleFromBody(size_t i);

  static bool RayCast(const Vec3 &pos, const Vec3 &dir, GOHandle go, float &time);
  static GOHandle RayCast(const Vec3 &pos, const Vec3 &dir, float &time);

 

private:
  //rb
  //b to a
  static void CopyRb(size_t a, size_t b);
  //a is the index where b data will replace it and b will be destroy
  //thus numCount will be 1 size less
  static void SwapReplaceRb(size_t a, size_t b);
  //static void ResetRB(size_t a);

  

  HandleManager<TransformComp> &m_transform;
};

void CHECK();