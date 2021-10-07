/******************************************************************************/
/*!
\file   WaypointManager.h
\author Cai wangyang
\par    email: wangyang.cai\@digipen.edu
\par    Digipen login: wangyang.cai
\date   02/02/2020
\brief
Contains definitions required for WayPointManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

typedef ComponentManagerBase<WayPointNode>::COMPHANDLETYPE WPtHandle;

#define ASC_WP WayPointManager::Inst()
class WayPointManager : public ComponentManagerBase<WayPointNode>, public Singleton<WayPointManager>
{
public:

  explicit WayPointManager(HandleManager<TransformComp> &trans);
  //~WayPointManager();

  //Virtual functions
  //void Load() override;
  //void UnLoad() override;
  void Update(float dt) override;
  GOHandle GetNearestWayPoint(const Vec3& pos, bool abs = 1);
  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;
  //Handle CloneComponent(Handle hdl) override;

  //Handle CreateComponent() override;

  void InitComponent(const GameObject& go, BaseHandle hdl) override;
  void CopyComponentToComponent(WayPointNode& target, const WayPointNode& source) override;
  void HandleMsg(MsgData_Base*) override;

  GOHandle AddWayPoint(const Vec3&) const;
  void DeleteWayPoint(GOHandle);

  void DrawWayPoints();
  void CreateLink(WayPointNode& left, WayPointNode& right);
  void DeleteLink(WayPointNode& left, WayPointNode& right);

  //for editor to call
  void SetAllToInvisible();
private:
  HandleManager<TransformComp> &m_transform;
  //std::list<WayPointNode> m_nodes;
};