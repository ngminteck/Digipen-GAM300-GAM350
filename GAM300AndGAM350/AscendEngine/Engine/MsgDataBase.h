/******************************************************************************/
/*!
\file   MsgDataBase.h
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef _MSGDATA_BASE
#define _MSGDATA_BASE
#include "GameObject.h"

enum MsgType
{
  mTYPE_INVALID,
  mTYPE_RESIZE,
  mTYPE_SELECTED,
  mTYPE_DESELECTED,
  mTYPE_CHANGE_SCENE,
  mTYPE_PLAY,
  mTYPE_RESUME,
  mTYPE_PAUSE,
  mTYPE_GAMEPAUSE,
  mTYPE_STOP,
  mTYPE_LOADING,
  mTYPE_PLAY_SCRIPT_INIT,
  mTYPE_SCENE_LOADED,
  mTYPE_DRAGDROP,   //currently for sound

  mTYPE_LOGIC_ONCOLLIDE_ENTER,
  mTYPE_LOGIC_ONCOLLIDE_EXIT,
  mTYPE_LOGIC_ONCOLLIDE_STAY,

  mTYPE_SYSTEM_SHORTCUT,
  mTYPE_SYSTEM_GAIN_FOCUS,
  mTYPE_SYSTEM_QUIT,

  mTYPE_SCENE_LOAD_RESULT,
  mTYPE_OBJ_DESTROYED,

  mTYPE_GAME_STARTCUTSCENE,
  mTYPE_GAME_ENDCUTSCENE,
  mTYPE_GAME_SKIPTUTORIAL,
  mTYPE_GAME_STARTINGRESOURCE,
  mTYPE_GAME_SETGAMETIME,
  mTYPE_GAME_LOAD_ENERGY,
  mTYPE_GAME_UNLOAD_ENERGY,
  mTYPE_GAME_ACTIVATE_TELEPORT_BEACON,
  mTYPE_GAME_HARVESTER_COLLECT,
  mTYPE_GAME_EARN_MONEY,
  mTYPE_GAME_WIN,
  mTYPE_GAME_LOSE,
  mTYPE_GAME_RESUME,
  mTYPE_GAME_PAUSE,
  mTYPE_GAME_POWERPLANT_ENERGY,
  mTYPE_GAME_POWERPLANT_CREATED,
  mTYPE_GAME_POWERPLANT_DESTROYED,
  mTYPE_GAME_PLAYER_ENABLE_CONTROLS,
  mTYPE_GAME_PLAYER_DISABLE_CONTROLS,
  mTYPE_GAME_PLAYER_DISABLE_CAM,
  mTYPE_GAME_PLAYER_ENABLE_CAM,
  mTYPE_GAME_START_HARVESTER,
  mTYPE_GAME_STOP_HARVESTER,
  mTYPE_GAME_SPAWN_BURST_ENEMIES,
  mTYPE_GAME_START_SPAWN_ENEMIES,
  mTYPE_GAME_STOP_SPAWN_ENEMIES,
  mTYPE_GAME_SHOW_UI,
  mTYPE_GAME_HIDE_UI,
  //mTYPE_PLAYER_CHANGE_WEAPON,

  mTYPE_CHANGE_COLOR_BUILDHUD,
  mTYPE_CHANGE_COLOR_WEAPONHUD,

  mTYPE_SHOOTKEY,

  mTYPE_GAME_UPGRADE_TOWER,

  //damage stuff
  mTYPE_GAME_TAKE_DAMAGE,
  mTYPE_GAME_GIVE_DAMAGE,

  //slow effect on target
  mTYPE_GAME_TAKE_SLOW,
  mTYPE_GAME_RESTORE_SLOW,  //gain back orignal speed

  // Change tower build type
  mTYPE_GAME_TOWER_SCROLL_UP,
  mTYPE_GAME_TOWER_SCROLL_DOWN,
  mTYPE_GAME_TOWER_SET,

  mTYPE_GAME_QUIT_MENU,
  mTYPE_NUM,

  ////*------------Logic/scripting ---------------**//
  ////////////////////////////////////////////////////
    //      Mission stuff
    mTYPE_MISSION_UPDATE_MISSION,
    mTYPE_MISSION_END,
    mTYPE_MISSION_PART_DONE,
    // when mission are done, this is send to some stuff to unlock stuff( like doors)
    mTYPE_MISSION_UNLOCK, 

    //      objective manager
    mTYPE_REG_TO_AS_ONE_MISSION,
    mTYPE_LINK_TO_AS_ONE_MISSION,// when you want 2 object to belong to the 1 mission


     //**------------ Msg_Force_Reg_Collide ---------**//
     mTYPE_BY_GO_HANDLE,
     mTYPE_BY_GO_TYPE,
     mTYPE_BY_GO_NAME,
     //**------------ Msg_Force_Reg_Collide ---------**//
     mTYPE_BY_GO_TYPE_IGNORERAY,

  ////////////////////////////////////////////////////
  ////*------------ End --------------------------**//
    mTYPE_PLAYER_PRESS_MISSIONLIST,
};

struct MsgData_Base
{
	//MsgData_Base();
	MsgData_Base(MsgType t) : type(t) {}
	virtual ~MsgData_Base() {}
	MsgType type;
};

/*
//Example
struct Vector2 : MsgData_Base
{
Vector2(float x_, float y_)
: MsgData_Base(mTYPE_VECTOR2), x(x_), y(y_) {}
Vector2(const Vector2& rhs) : Vector2(rhs.x,rhs.y){}
~Vector2() {}

float x;
float y;
};
*/

struct Msg_Resize : MsgData_Base
{
  Msg_Resize(int x_, int y_)
		: MsgData_Base(mTYPE_RESIZE), x(x_), y(y_) {}

	int x, y;
};

//struct Msg_Selected : MsgData_Base
//{
//  Msg_Selected(unsigned int idx)
//    : MsgData_Base(mTYPE_SELECTED), pickedID(idx){}
//  unsigned int pickedID;
//};

struct Msg_Change_Scene : MsgData_Base
{
  Msg_Change_Scene(const std::string &scene)
    : MsgData_Base(mTYPE_CHANGE_SCENE), sceneName(std::move(scene)) {}
  Msg_Change_Scene(std::string &&scene)
    : MsgData_Base(mTYPE_CHANGE_SCENE), sceneName(std::move(scene)) {}
  std::string sceneName;
};

struct Msg_Logic_OnCollide : MsgData_Base
{
  Msg_Logic_OnCollide(MsgType type, const Handle<GameObject>& gameObject, const Handle<GameObject>& other)
    : MsgData_Base(type), gameObject(gameObject), other(other) {}
  Handle<GameObject> gameObject;
  Handle<GameObject> other;
};
//use for logic/script only , it will hold gameobject to be use by the waitee
struct Msg_Logic_Mission : MsgData_Base
{
  Msg_Logic_Mission(MsgType type, const Handle<GameObject>& gameObject)
    : MsgData_Base(type), gameObject(gameObject) {}
  Msg_Logic_Mission(MsgType type ,int n)
    : MsgData_Base(type), num{n} {}
  Handle<GameObject> gameObject;
  int num; 
};


// use for forcing triger registsering only 
struct Msg_Force_Reg_Collide : MsgData_Base
{
  Msg_Force_Reg_Collide(MsgType type, Handle<GameObject> obj  , Handle<GameObject>& gameObject)
    : MsgData_Base(type), registeeGameObject{ obj }, targetGameObject(gameObject) {}

  Msg_Force_Reg_Collide(MsgType type, Handle<GameObject> obj, GOTYPE typobj)
    : MsgData_Base(type), registeeGameObject{ obj },objType(typobj) {}

  Msg_Force_Reg_Collide(MsgType type, Handle<GameObject>obj ,std::string name)
    : MsgData_Base(type), registeeGameObject{ obj },nameOfGameObject(name) {}

  //the one requesting to be reg to Trigger/Collide
  Handle<GameObject> registeeGameObject;

  //only for MsgType mTYPE_BY_GO_HANDLE
  Handle<GameObject> targetGameObject;
  //only for MsgType mTYPE_BY_GO_NAME
  std::string nameOfGameObject;
  //only for MsgType mTYPE_BY_GO_TYPE
  GOTYPE objType; 
};

// use for forcing triger registsering only 
struct Msg_MissionListStuff : MsgData_Base
{
  Msg_MissionListStuff(MsgType type)
    : MsgData_Base(type) {}
  bool result;
};




//for system shortcut #defines
struct Msg_UINT : MsgData_Base
{
  Msg_UINT(unsigned int v, MsgType type)
    : MsgData_Base(type), var(v) {}
  unsigned int var;
};

struct Msg_Error : MsgData_Base
{
  Msg_Error(std::string&& s)
    : MsgData_Base(mTYPE_SCENE_LOAD_RESULT), str(s) {}
  std::string str;
};

struct Msg_Game_FloatVal : MsgData_Base
{
  Msg_Game_FloatVal(float v, MsgType t)
    : MsgData_Base(t), val(v) {}
  float val;
};

struct Msg_Game_IntVal : MsgData_Base
{
  Msg_Game_IntVal(int v, MsgType t)
    : MsgData_Base(t), val(v) {}
  int val;
};

struct Msg_Game_BoolVal : MsgData_Base
{
  Msg_Game_BoolVal(bool v, MsgType t)
    : MsgData_Base(t), val(v) {}
  bool val;
};

struct Msg_Game_Vec3Val : MsgData_Base
{
  Msg_Game_Vec3Val(Vec3 const&v, MsgType t)
    : MsgData_Base(t), val(v) {}
  Vec3 val;
};

struct Msg_Game_Vec4Val : MsgData_Base
{
  Msg_Game_Vec4Val(Vec4 const&v, MsgType t)
    : MsgData_Base(t), val(v) {}
  Vec4 val;
};

struct Msg_Game_GOHDL : MsgData_Base
{
  Msg_Game_GOHDL(GOHandle hdl, MsgType t)
    : MsgData_Base(t), hdl(hdl) {}
  GOHandle hdl;
};

#endif