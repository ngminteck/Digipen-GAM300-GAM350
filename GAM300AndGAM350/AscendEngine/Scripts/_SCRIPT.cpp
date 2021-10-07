/******************************************************************************/
/*!
\file   _SCRIPT.cpp
\author
\par    email: \@digipen.edu
\par    Digipen login:
\date   14/4/2017
\brief

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

_SCRIPT::_SCRIPT() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(_SCRIPT))
{
  /*auto fn = [&](MsgData_Base *mb)
  {
    switch (mb->type)
    {
    }
  };*/
  //RegisterMsg(mTYPE_GAME_SKIPTUTORIAL, &_SCRIPT::HandleMessage);
  //ASC_SCRIPTMAN.SendInternalMsg(moneyFb->GetHdl<LogicComp>(),
  //Msg_Game_Vec3Val(goHdl->GetHdl<TransformComp>()->pos, mTYPE_GAME_EARN_MONEY));
  //SendMsg(mTYPE_GAME_STOP_SPAWN_ENEMIES);
}

_SCRIPT::~_SCRIPT()
{
}

void _SCRIPT::OnStart()
{
}

void _SCRIPT::Update(float dt)
{
}

void _SCRIPT::OnDestroy()
{
}

void _SCRIPT::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write(GetName());

  // Write whaever you need to save here
  // sr.Write("Health", someInt);
}

void _SCRIPT::Deserialize(Deserializer& d)
{
  // Read whaever you need to save here
  // d.Read("Health", someInt);
}

void _SCRIPT::Reflect(const Reflector& r)
{
  // Reflect whatever
  // r.Reflect("Health", someInt);
}

//void _SCRIPT::OnCollisionEnter(GOHandle other)
//{
//}
//
//void _SCRIPT::OnCollisionStay(GOHandle other)
//{
//}
//
//void _SCRIPT::OnCollisionExit(GOHandle other)
//{
//}

void _SCRIPT::HandleMessage(MsgData_Base *m)
{
}