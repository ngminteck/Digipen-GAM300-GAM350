/******************************************************************************/
/*!
\file   GameObject.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definitions required for COMPONENT_TYPE, GameObject

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

class GameObject;
typedef Handle<GameObject> GOHandle;

//extern Handle Null;

enum COMPONENT_TYPE
{
  TRANSFORM,
  GRAPHICS,
  PHYSICS,
  LOGIC,
  SOUND,
  LIGHT,
  UITEXT,
  WAYPOINT,
  PARTICLE,
  //OBJMAN,
  TAG,
  NEWPHYSICS,
  ANIMATION,
  MAX
};

static std::unordered_map<std::string, COMPONENT_TYPE> strCompMap = {
  { "TRANSFORM", TRANSFORM },
  { "GRAPHICS", GRAPHICS },
  { "PHYSICS", PHYSICS },
  { "LOGIC", LOGIC },
  { "SOUND", SOUND },
  { "LIGHT", LIGHT },
  { "UITEXT", UITEXT },
  { "WAYPOINT", WAYPOINT },
  { "PARTICLE", PARTICLE },
  { "TAG", TAG},
  { "NEWPHYSICS", NEWPHYSICS },
  { "ANIMATION", ANIMATION },
  //{ OBJMAN, "OBJMAN" },
};

static std::unordered_map<COMPONENT_TYPE, const char *> compStrMap = {
  { TRANSFORM, "TRANSFORM" },
  { GRAPHICS, "GRAPHICS" },
  { PHYSICS, "PHYSICS" },
  { LOGIC, "LOGIC" },
  { SOUND, "SOUND" },
  { LIGHT, "LIGHT" },
  { UITEXT, "UITEXT" },
  { WAYPOINT, "WAYPOINT" },
  { PARTICLE, "PARTICLE" },
  { TAG, "TAG" },
  { NEWPHYSICS, "NEWPHYSICS" },
  { ANIMATION, "ANIMATION" },
  //{ OBJMAN, "OBJMAN" },
};

/*
TODO:
Need to add cloning function
*/
class GameObject
{
  GOTYPE m_type = go_default;
public:
  static int count;
  bool isEnable = true;
  bool GetIsEnable()const { return isEnable; }
  void SetEnable(bool val);
  GameObject() : GameObject("DefaultObject_" + std::to_string(count++)) {};
  GameObject(std::string& n) : name(n) {}

  void AddComponent(COMPONENT_TYPE type, BaseHandle componentIndex);
  BaseHandle operator[](COMPONENT_TYPE type) const;
  template<typename CompType>
  inline Handle<CompType> GetHdl() const { return Handle<CompType>(operator[](CompType::CompEnum)); }

  inline GOTYPE GetType() const { return m_type; }
  //this one will use the factory set type DO NOT CHANGE
  void SetType(GOTYPE type);
  //NOTE: NOT suppose to have
  //inline void SetType(GOTYPE type) { m_type = type; }
  //please use the object factory SetGOTYPE

  template<COMPONENT_TYPE type>
  BaseHandle GetComponent()
  {
    compItr itr = components.find(type);
    return itr->second;
  }



  typedef std::unordered_map<COMPONENT_TYPE, BaseHandle>::iterator compItr;

  //variables.
  std::unordered_map<COMPONENT_TYPE, BaseHandle> components;
  std::string name;
  std::string m_prefab_name = "";

  GameObject* parent = nullptr;
  std::unordered_map<std::string, GameObject* > child_container;

  //make factory a friend
  friend class ObjectFactory;
  ASC_VarGetSet(GOHandle, SelfHandle)
};

//for prefab
struct PBGameObject
{
public:
  std::unordered_map<COMPONENT_TYPE, PBHandle> pbcomponents;
  std::string name;
};

void HighlightObj(GOHandle hdl, bool flag);
void HighlightObj(GameObject& go, bool flag);

