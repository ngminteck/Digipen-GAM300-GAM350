/******************************************************************************/
/*!
\file   WaypointManager.cpp
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

#include "pch.h"
#include "WaypointManager.h"


WayPointManager::WayPointManager(HandleManager<TransformComp> &trans) :
  m_transform(trans)
{
  ASC_MSGSYS.RegisterCallBack(mTYPE_SCENE_LOADED, &WayPointManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &WayPointManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_RESUME, &WayPointManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &WayPointManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PAUSE, &WayPointManager::HandleMsg, this);
}

//do this next
BaseHandle WayPointManager::Deserialize(const GameObject &gameObject, Deserializer & deserializer)
{
  //Log("-------------------------------------------------");
  //Handle id;
  WPtHandle id(CreateComponent());
  WayPointNode &wp = *GetComponent(id);
  wp.SetGOHandle(gameObject.GetSelfHandle());
  wp.SetTransHdl(Handle<TransformComp>(gameObject[TRANSFORM]));

  tinyxml2::XMLElement *n = deserializer.m_xmlElem.FirstChildElement("Neighbour");
  while (n)
  {
    Vec3 neighborPos;
    neighborPos.x = n->FloatAttribute("x");
    neighborPos.y = n->FloatAttribute("y");
    neighborPos.z = n->FloatAttribute("z");
    
    //Log("Neighbour : ", neighborPos);
    wp.neighboursPos.push_back(neighborPos);

    n = n->NextSiblingElement();
  }
  return id;
}

void WayPointManager::Serialize(Serializer & serializer, BaseHandle hdl)
{
  if (m_components.m_objects.empty()) return;

  // Get the current transform to serialize
  WayPointNode &wp = *GetComponent(hdl);

  //needs to read in a std::list<WayPointNode*> of items.
  //serializer.Write("NeighboursCount", static_cast<int>(wp.neighbours.size()));

  for (auto &elem : wp.neighbours)
  {
    auto &trans = *ASC_FACTORY.GetComponentPtr<TransformComp>(elem);
    serializer.Write("Neighbour", trans.pos);

    //wp.neighboursPos[i] = trans.pos;
  }

  //for (auto links : wp.neighbours)
  //  serializer.Write("Neighbours", links.Val);
}

void WayPointManager::Reflect(BaseHandle id, const Reflector & rf)
{
  WayPointNode &wp = *GetComponent(id);

  //this one is tricky...
  //can reflect:
  //1. Number of linked nodes.
  //3. Show on panel: all linked node's name

  //rf.Reflect("Size", wp.neighours.size());
  //rf.Reflect("Linked Nodes", wp.neighours);
  static unsigned int SelectedObj = 0;

  bool clickedObj = false;
  GOHandle selectedHdl;
  if (ImGui::TreeNode("Neighbours"))
  {
    //iterate through my list of neighbours and reflect their names.
    for (unsigned int i = 0; i < wp.neighbours.size(); ++i)
    {
      auto &elem = *ASC_GETGO_PTR(wp.neighbours[i]);
      auto& str = elem.name;
      if (ImGui::Selectable(str.c_str(), SelectedObj == i))
      {
        clickedObj = true;
        SelectedObj = i;
        selectedHdl = elem.GetSelfHandle();
      }
    }

    ImGui::TreePop();
  }

  if (clickedObj)
    ASC_MSGSYS.SendMsg(Msg_UINT(selectedHdl.Val(), mTYPE_SELECTED));
}

void WayPointManager::Update(float dt)
{
}

GOHandle WayPointManager::GetNearestWayPoint(const Vec3& pos, bool abs)
{
  if (abs)
  {

    for (auto &elem : m_components.m_objects)
    {
      auto transform = m_transform[elem.GetTransHdl()];
      assert(transform);

      if (transform->pos == pos)
      {
      return elem.GetGOHandle();
      }
    }

  }
  else
  {
    float dist = FLT_MAX;
    GOHandle go;

    for (auto &elem : m_components.m_objects)
    {
      auto transform = m_transform[elem.GetTransHdl()];
      assert(transform);

      float tmpDist = ASC_DISTBETWEEN_2PTSQ(transform->pos, pos);
      //Log(ASC_GETGO_PTR(elem.GetGOHandle())->name, ": ", tmpDist, "(", transform->pos, ")");
      if (tmpDist < dist)
      {
        dist = tmpDist;
        go = elem.GetGOHandle();
      }
    }

    if (dist != FLT_MAX)
    {
      //Log("Chosen: ",ASC_GETGO_PTR(go)->name, ": ", dist);
      return go;
    }
  }

  // Shouldn't happened?
  //assert(false);
  return GOHandle();
}

//Handle CloneComponent(Handle hdl) override;
////
//Handle WayPointManager::CreateComponent()
//{
//  Handle hdl = m_components.GetNewObject();
//  WayPointNode& WP_comp = *m_components[hdl];
//  WP_comp.neighbours.clear();
//  return hdl;
//}

void WayPointManager::InitComponent(const GameObject& go, BaseHandle hdl)
{
  WPtHandle wphdl(hdl);
  m_components[wphdl]->SetGOHandle(go.GetSelfHandle());
  //WayPointNode &waypoint = *m_components[wphdl];
  //waypoint.SetTransHdl(go[TRANSFORM]);
}

void WayPointManager::CopyComponentToComponent(WayPointNode& target, const WayPointNode& source)
{
  target = source;
}


void WayPointManager::HandleMsg(MsgData_Base *msg)
{
  switch (msg->type)
  {
  case mTYPE_SCENE_LOADED:
  {
    for (auto &elem : m_components.m_objects)
    {
      for (auto &pos : elem.neighboursPos)
      {
        auto link = GetNearestWayPoint(pos);
        elem.neighbours.push_back(link);
      }
    }

#ifdef _SUBMISSION
    SetAllToInvisible();
#endif 
  } break;
  case mTYPE_PLAY:
  case mTYPE_RESUME:
  {
  }
  break;
  case mTYPE_STOP:
  case mTYPE_PAUSE:
  {
#ifndef _SUBMISSION
    for (auto &elem : m_components.m_objects)
      elem.GetGOHandle()->GetHdl<GraphicsComp>()->SetVisible(true);
#endif
  }
  break;
  default: break;
  }
}

GOHandle WayPointManager::AddWayPoint(const Vec3 & pos) const
{
  //spawn prefab Waypoint Node
  auto wp = ASC_FACTORY.CreateGOfromPB("Waypoint");
  ASC_FACTORY.GetGameObjectPtr(wp)->name += " " + std::to_string(m_components.m_objects.size());
  ASC_GETCOMP_PTR(TransformComp, wp)->pos = pos;
  return wp;
}

void WayPointManager::DeleteWayPoint(GOHandle hdl)
{
  GameObject* gObj = ASC_FACTORY.GetGameObjectPtr(hdl);
  WayPointNode* self = m_components[gObj->GetHdl<WayPointNode>()];
  std::vector<GOHandle> neighbours = self->neighbours;
  for (auto links : neighbours)
  {
    GameObject* lObj = ASC_FACTORY.GetGameObjectPtr(links);
    WayPointNode* lHdl = m_components[lObj->GetHdl<WayPointNode>()];
    DeleteLink(*lHdl, *self);
  }

  //ASC_FACTORY.DestroyObject(hdl);
}

void WayPointManager::DrawWayPoints()
{
#ifndef _SUBMISSION
  for (auto &elem : m_components.m_objects)
  {
    auto t1 = ASC_GETCOMP_PTR(TransformComp, elem.GetGOHandle());
    for (auto links : elem.neighbours)
    {
      auto t2 = ASC_GETCOMP_PTR(TransformComp, links);
      ASC_LINERENDER.AddLine(t1->pos, t2->pos, { 1.0f, 0, 1.0f, 1.0f });
    }
  }
#endif
}

void WayPointManager::CreateLink(WayPointNode& left, WayPointNode& right)
{
  if (left.GetSelfHandle() == right.GetSelfHandle()) return;

  for (auto &elem : left.neighbours)
    if (elem == right.GetSelfHandle()) return;

  left.neighbours.push_back(right.GetGOHandle());
  right.neighbours.push_back(left.GetGOHandle());
}

void WayPointManager::DeleteLink(WayPointNode& left, WayPointNode& right)
{
  auto& rWP = std::find(left.neighbours.begin(), left.neighbours.end(), right.GetGOHandle());
  auto& lWP = std::find(right.neighbours.begin(), right.neighbours.end(), left.GetGOHandle());

  if (rWP != left.neighbours.end() && lWP != right.neighbours.end()) 
  {
    left.neighbours.erase(rWP);
    right.neighbours.erase(lWP);
  }
}

void WayPointManager::SetAllToInvisible()
{
  for (auto &elem : m_components.m_objects)
    elem.GetGOHandle()->GetHdl<GraphicsComp>()->SetVisible(false);
}
