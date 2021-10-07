/******************************************************************************/
/*!
\file   GameObject.cpp
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definitions required for GameObject_type

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

//this varialbe is form 
const size_t GOTP::reserve_sz = 200;
std::vector<const char*> GOTP::GOtypeList;
std::unordered_map<std::string, GOTYPE> GOTP::strGOTypeMap;
std::unordered_map<GOTYPE, const char *> GOTP::GOTypeStrMap;

//Handle Null;

int GameObject::count = 0;

//Handle::Handle(unsigned int id) :
//  m_id(id), m_iteration(0)
//{
//}

//Handle::operator bool() const
//{
//  return m_id == ASC_INVALID_HANDLE;
//}

//Handle::operator unsigned() const
//{
//  return m_id;
//}
void  GameObject::SetEnable(bool val)
{
  GOHandle();
  isEnable = val;
  //check for parenting
  auto obj = components.find(COMPONENT_TYPE::NEWPHYSICS);
  
  if(obj != components.end())
  {
    NewPhysicsComp* tc = ASC_NEWPHYSICSMANAGER.GetComponent(obj->second);
    tc->obj_phy->BodySetGravityEnable((val) ? tc->prevsBoolForGrav : false);
  }
}
void GameObject::AddComponent(COMPONENT_TYPE type, BaseHandle componentIndex)
{
  components[type] = componentIndex;
}


BaseHandle GameObject::operator[](COMPONENT_TYPE type) const
{
  auto elem = components.find(type);
  return elem != components.end() ? elem->second : BaseHandle();
}

void GameObject::SetType(GOTYPE type)
{
  ASC_FACTORY.SetGOType(GetSelfHandle(), type);
}


void HighlightObj(GOHandle hdl, bool flag)
{
  HighlightObj(*ASC_GETGO_PTR(hdl), flag);
}

void HighlightObj(GameObject& go, bool flag)
{
  BaseHandle gchdl = go[GRAPHICS];
  if (gchdl.IsValid())
  {
    GraphicsComp* gc = ASC_GMAN.GetComponent(gchdl);
    //this is so it doesn't call too many times for parented stuff
    if (gc->isHighlighted == flag) return;
    gc->SetHighlight(flag);
  }
  //check for parenting
  TransformComp* tc = ASC_TRANSMAN.GetComponent(go[TRANSFORM]);
  if (tc->parentinghdl.IsValid())
  {
    ChildIterate ci = ASC_TRANSMAN.GetChildIter(tc->parentinghdl);
    while (ci.Next().IsValid())
    {
      GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());
      HighlightObj(*nextChild, flag);
    }
  }
}
