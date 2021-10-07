/******************************************************************************/
/*!
\file   ScriptHelper.h
\author Zelin ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for ScriptHelper

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef SHELPER_H
#define SHELPER_H

#define SICREATE(x) SI_##x,
enum ScriptIndex {
  SI_ObjectiveManager,

#include "../Scripts/scriptnames.h"
  //Anything,

  //add SICREATE in ScriptCreator.h 

  SI_MAX_SCRIPT
};
#undef SICREATE

enum TowerType
{
  Tower_Basic,
  Tower_Laser,

  Tower_Frost,
  Tower_FrostAOE,

  Tower_Debuff,
  Tower_DebuffUpgrade,
  //Also add to :
  //1)TOWER_CREATE in ScriptCreator.h
  //2)towerInfo_map below
  Tower_Max,
};

enum InteractType
{
    InteractBase,
    Interact_Door,
    Interact_Item,
    Interact_Pickup,
    Interact_Bark,
    Interact_Move,
    Interact_Activate,
    Interact_Swivel,
    Interact_Puppy,
    Interact_PushPull,

};
enum TextType
{
  BASE_TEXT,
  Dialog_Text,
};
enum TriggerType
{
  BASE_TRIGGER,
  EVENT_TRIGGER,
};
struct TextInfo
{
  TextInfo() = delete;
  TextInfo(ScriptIndex scriptType_,
    TextType _interact)
    : scriptType(scriptType_), interact(_interact)
  {}
  const ScriptIndex scriptType;
  const TextType interact;

private:
};

struct TriggerInfo
{
  TriggerInfo() = delete;
  TriggerInfo(ScriptIndex scriptType_,
    TriggerType _interact)
    : scriptType(scriptType_), trigger(_interact)
  {
  }


  const ScriptIndex scriptType;
  const TriggerType trigger;

private:


};

struct InteractInfo
{
    InteractInfo() = delete;
    InteractInfo(ScriptIndex scriptType_,
    InteractType _interact)
    : scriptType(scriptType_),interact(_interact)
{
}


const ScriptIndex scriptType;
const InteractType interact;

private:


};
struct TowerInfo
{
  TowerInfo() = delete;
  TowerInfo(ScriptIndex scriptType_, int cost, float range, 
    TowerType baseTowerUpgrade, TowerType lastTowerUpgrade, std::string hologram_prefabName)
    : scriptType(scriptType_), res_cost(cost), hardcoded_range(range), 
    baseUpgrade(baseTowerUpgrade), lastUpgrade(lastTowerUpgrade),
    hologram_prefab(hologram_prefabName)
  {
  }

  GOHandle GetHologram()
  {
    hologram = hologram.IsValid() ?
      hologram : ASC_FACTORY.CreateGOfromPB(hologram_prefab.data());

    return hologram;
  }

  const ScriptIndex scriptType;
  const int res_cost = INT_MAX;
  const float hardcoded_range = FLT_MAX;
  const TowerType baseUpgrade = Tower_Max;
  const TowerType lastUpgrade = Tower_Max;
  const std::string hologram_prefab;
private:
  GOHandle hologram;
};

static const std::map<InteractType, InteractInfo> interactInfo_map
{
                             //        ScriptType, type of interact
{ InteractBase ,         InteractInfo(SI_Interact,InteractBase) },
{ Interact_Door ,         InteractInfo(SI_DoorInteract,Interact_Door) },
{ Interact_Item ,         InteractInfo(SI_ItemInteract,Interact_Item) },
{ Interact_Pickup ,         InteractInfo(SI_PickupInteract,Interact_Pickup) },
{ Interact_Bark ,         InteractInfo(SI_BarkInteract,Interact_Bark) },
{ Interact_Move ,         InteractInfo(SI_MoveInteract,Interact_Move) },
{ Interact_Activate ,     InteractInfo(SI_ActivateInteract,Interact_Activate) },
{ Interact_Puppy ,         InteractInfo(SI_PuppyInteract,Interact_Puppy) },
{ Interact_PushPull ,         InteractInfo(SI_PushPullInteract,Interact_PushPull) },
{ Interact_Swivel ,         InteractInfo(SI_SwivelInteract,Interact_Swivel) },

//{ Tower_Max , INT_MAX},
};
static const std::map<TextType, TextInfo> textInfo_map
{
  //        ScriptType, type of interact
//{ BASE_TEXT ,          TextInfo(SI_BaseTrigger,BASE_TEXT) }
//{ Tower_Max , INT_MAX},
};
//te
static const std::map<TriggerType, TriggerInfo> triggerInfo_map
{
  //        ScriptType, type of interact
{ BASE_TRIGGER ,         TriggerInfo(SI_BaseTrigger,BASE_TRIGGER) },
{ EVENT_TRIGGER ,         TriggerInfo(SI_EventTrigger,EVENT_TRIGGER) },


//{ Tower_Max , INT_MAX},
};
//Helper function for towerInfo_map
const TowerInfo& GetTowerInfo(TowerType type);

enum EnemyType
{
  Enemy_Ground,
  Enemy_Air,
  Enemy_Meteorite,
  Enemy_Speedy,
  //Also add to :
  //1) 
  Enemy_Max,
};

enum Effects
{
  Effect_Slow,
  Effect_GenerateDmgText,
  Effect_ChangeColor,
  Effect_DamageBuff,
  Effect_ApplyDamage,
  Effect_MAX
};


struct TargetInfo
{
  TargetInfo(){}

  TargetInfo(
    GOHandle& target, GOHandle& me, GOTYPE type_, const Vec3& target_pos_, const Vec3& dir_,
    float dist_sq_, float UI_disSq_ = 0, const std::string& name_ = 0, float raycastTime_ = 0
  )
    : targetHdl(target), meHdl(me), type(type_), target_pos(target_pos_), dir(dir_), dist_sq(dist_sq_),
    UI_disSq(UI_disSq_), name(name_), raycastTime(raycastTime_)
  {}

  TargetInfo(
    GOHandle target, GOHandle me, bool raycast = false,
    float UI_disSq_ = 0
  )
    : targetHdl(target), meHdl(me), UI_disSq(UI_disSq_)
  {
    //type(type_), pos(pos_), dir(dir_), dist_sq(dist_sq_), const std::string& name_ = 0

    auto target_t = target->GetHdl<TransformComp>();
    auto me_t = me->GetHdl<TransformComp>();
    target_pos = target_t->pos;
    type = target->GetType();
    name = target->name;
    Vec3 tmp_dir = target_pos - me_t->pos;
    dir = tmp_dir.Normalize();
    dist_sq = tmp_dir.MagnitudeSq();

    if (raycast) PhysicsManager::RayCast(dir, dir, raycastTime); 
    else raycastTime = FLT_MAX;
  }

  void UpdateInfo(const Vec3& ownPos, float angle_ = 0)
  {
    assert(targetHdl.IsValid());

    //using the same handle
    GameObject* gObj = ASC_GETGO_PTR(targetHdl);
    TransformComp* target_t = ASC_GETCOMP_PTR(TransformComp, targetHdl);
    target_pos = target_t->pos;
    Vec3 tmp_dir = target_pos - ownPos;
    dir = tmp_dir.Normalize();
    dist_sq = tmp_dir.MagnitudeSq();

#if !SUBMISSION
    name = gObj->name;
#endif
  }

  void Reset()
  {
    targetHdl = GOHandle();
    type = go_default;
    dir = target_pos = Vec3();
    UI_disSq = dist_sq = raycastTime = FLT_MAX;
    name = "invalid";

    path_index = 0;
    path.clear();
    endNode = GOHandle();
    startNode = GOHandle();
  }

  GOHandle targetHdl;// = ASC_INVALID_HANDLE;
  GOHandle meHdl;// = ASC_INVALID_HANDLE;
  GOTYPE type = go_default;
  Vec3 dir, target_pos;
  float dist_sq = FLT_MAX;

  float raycastTime = FLT_MAX;
  float UI_disSq = FLT_MAX;
  std::string name = "invalid";

  unsigned path_index = 0;
  std::vector<GOHandle> path;
  GOHandle endNode;// = ASC_INVALID_HANDLE;
  GOHandle startNode;// = ASC_INVALID_HANDLE;
};

struct EffectInfo
{
  EffectInfo() {}
  EffectInfo(Effects type_, float duration_, float var_ = 0.0f, Color newcol_ = Color::HOT_PINK, float delay_ = 0.0f)
    : type(type_), var(var_), duration(duration_), new_col(newcol_), delay(delay_)
  {}

  Effects type;
  Color orig_col = Color::BLACK;
  Color new_col = Color::BLACK;
  float var = 0.0f;
  float duration = 0.0f;
  float delay = 0.0f;

  float curr_dt = 0.0f;
  float curr_time = 0.0f;
};

//#define ASC_CREATEBULLET(HANDLE_REFERENCE) \
//HANDLE_REFERENCE = ASC_FACTORY.CreateGOfromPB("Bullet"); 
#define ASC_GETSCRIPT_INFOR_PTR(GO_HANDLE)\
dynamic_cast<SCRIPTING::ObjInfor*>(GetScript_Infor(GO_HANDLE));

#define ASC_GETSCRIPT_PTR(SCRIPT_CLASS_NAME, GO_HANDLE)\
static_cast<SCRIPT_CLASS_NAME*>(GetScript_fromGO(GO_HANDLE, ScriptIndex::SI_##SCRIPT_CLASS_NAME));

#define ASC_GETSCRIPT_TOWER(GO_HANDLE)\
static_cast<Tower*>(GetScriptTower_fromGO(GO_HANDLE));

#define ASC_GETSCRIPT_INTERACT(GO_HANDLE)\
static_cast<Interact*>(GetScriptInteract(GO_HANDLE));


#define ASC_GETSCRIPT_BASETRIGGER(GO_HANDLE)\
static_cast<BaseTrigger*>(GetScriptBaseTrigger(GO_HANDLE));


#define ASC_GETCOMP_PTR_BYTYPE(COMPTYPE_ENUM, GOhandle) GetComp(COMPTYPE_ENUM, GOhandle)

//#define TowerTPCREATE(TowerType_name) \
//StrTypeMap.insert(std::pair<std::string, TowerType>(#TowerType_name, TowerType_name)); \
//TypeStrMap.insert(std::pair<TowerType, const char *>(TowerType_name , #TowerType_name)); \
//TypeList.push_back(#TowerType_name);


SCRIPTING::ScriptInterface* GetScript_fromGO(GOHandle handle, ScriptIndex idx);
SCRIPTING::ScriptInterface* GetScriptTower_fromGO(GOHandle handle);
SCRIPTING::ScriptInterface* GetScriptInteract(GOHandle handle);
SCRIPTING::ScriptInterface* GetScriptBaseTrigger(GOHandle handle);
SCRIPTING::ScriptInterface* GetScript_Infor(GOHandle handle);


float Calculate_ValuePerFrame(float dmgPerSec, float dt);
BaseComponent* GetComp(COMPONENT_TYPE type, GOHandle h);

float CalculateDmgRegisterDelay(float distToTarget_sq, float bullet_speed, float mul_modifier_ToResult);
GOHandle ParticleEffect_HitTarget_Basic(const std::string & prefab, const TargetInfo & info/*, Camera & cam, float raycastTime*/, float delay,bool emit);
GOHandle ParticleEffect_HitFloor_Basic(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, float raycastTime, bool emit);
GOHandle ParticleEffect_TargetBehindCover_Basic(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, float raycastTime, float delay, bool emit);

GOHandle ParticleEffect_HitTarget_Frost(const std::string & prefab, const TargetInfo & info/*, Camera & cam, float raycastTime*/, float delay, bool emit);
GOHandle ParticleEffect_HitFloor_Frost(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, float raycastTime, bool emit);
GOHandle ParticleEffect_TargetBehindCover_Frost(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, float raycastTime, float delay, bool emit);

#define EMPTY_CODE ;
//GOHANDLE_CHILD is the identifier/handle to the child, PARENT_CODE only execute once, CHILD_CODE execute for all child
#define APPLYTO_ALLCHILD(WANRING_ON, INPUT_GOHANDLE_PARENT, GOHANDLE_CHILD, PARENT_CODE, CHILD_CODE) \
{\
PARENT_CODE;\
auto p_handle = INPUT_GOHANDLE_PARENT->GetHdl<TransformComp>()->parentinghdl;\
if (p_handle.IsValid())\
{\
  ChildIterate ci = ASC_TRANSMAN.GetChildIter(p_handle);\
  while (ci.Next().IsValid())\
  {\
    GOHandle GOHANDLE_CHILD \
      = ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle();\
    assert(GOHANDLE_CHILD.IsValid());\
    if(GOHANDLE_CHILD.IsValid())\
    { CHILD_CODE; }\
    else\
    {\
      if (WANRING_ON)\
        LogWarning("SCIRPT: APPLYTO_CHILD, GOHANDLE_CHILD invalid"); \
    }\
  }\
}\
else\
{\
  if(WANRING_ON)\
    LogWarning("SCIRPT: APPLYTO_ALLCHILD, p_handle invalid");\
}\
}

#define APPLYTO_CHILD(WANRING_ON, INPUT_GOHANDLE_PARENT, GOHANDLE_CHILD, CHILD_NAME_STR ,CODE) \
{\
  auto p_handle = INPUT_GOHANDLE_PARENT->GetHdl<TransformComp>()->parentinghdl;\
  if (p_handle.IsValid())\
  {\
    auto child_trans_handle = INPUT_GOHANDLE_PARENT->GetHdl<TransformComp>()->FindChild(CHILD_NAME_STR);\
    if(child_trans_handle.IsValid())\
    {\
      GOHandle GOHANDLE_CHILD = child_trans_handle->GetGOHandle();\
      assert(GOHANDLE_CHILD.IsValid());\
      CODE;\
    }\
    else\
    {\
      if (WANRING_ON)\
        LogWarning("SCIRPT: APPLYTO_CHILD, cannot find child name :", CHILD_NAME_STR); \
    }\
  }\
  else\
  {\
    if(WANRING_ON)\
      LogWarning("SCIRPT: APPLYTO_ALLCHILD, p_handle invalid");\
  }\
}

template<typename CompType>
struct BlinkEff
{
  BlinkEff() : _currCount(0), _maxCount(_currCount), _spd(1.f), _towardsup(true) {}
  void Init(GOHandle _obj, Vec4 destClr, float spdfac = 2.0f, int count = 3);
  void Update(float dt);
  void Blink();
  inline bool DoneBlinking() { return _currCount == _maxCount; }
  void StopAnimate();

  Vec4 _initialclr, _vecClr;
  GOHandle _obj;
  int _currCount, _maxCount;
  float _t, _spd;
  bool _towardsup;
};

template<typename CompType>
void BlinkEff<CompType>::Init(GOHandle obj, Vec4 destClr, float spdfac, int count)
{
  _obj = obj;
  if (_obj.IsValid())
  {
    _currCount = _maxCount = count;
    _initialclr = _obj->GetHdl<CompType>()->tint;
    _vecClr = destClr - _initialclr;
    _spd = spdfac;
    _t = 0.f;
  }
}

/*************************BLINk eff helper**************************/
template<typename CompType>
void BlinkEff<CompType>::Update(float dt)
{
  if (_obj.IsValid())
  {
    if (_currCount < _maxCount)
    {
      if (_t < 1.0f)
      {
        _t += dt * _spd;
        _t = min(max(_t, 0.f), 1.0f);
        float _tt = _towardsup ? _t : 1.0f - _t;
        _obj->GetHdl<CompType>()->tint = _initialclr + _tt * _vecClr;
      }
      else {
        _towardsup = !_towardsup;
        _t = 0.f;
        if (_towardsup) ++_currCount;
      }
    }
  }
}

template<typename CompType>
void BlinkEff<CompType>::Blink()
{
  if (_obj.IsValid())
  {
    _currCount = 0;
    _t = 0.f;
  }
}

template<typename CompType>
void BlinkEff<CompType>::StopAnimate()
{
  _t = 1.0f;
  _currCount = _maxCount;
  if (_obj.IsValid())
  {
    _obj->GetHdl<CompType>()->tint = _initialclr;
  }
}
/*************************BLINk eff helper**************************/



#endif