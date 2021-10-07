/******************************************************************************/
/*!
\file   PlaySoundOnStart.cpp
\author zelin ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

PlaySoundOnStart::PlaySoundOnStart() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(PlaySoundOnStart))
{
  //ASCPlaySoundOnStart_REGISTER_MSG(, &PlaySoundOnStart::HandleMessage);
}

PlaySoundOnStart::~PlaySoundOnStart()
{
}

void PlaySoundOnStart::OnStart()
{
  //ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), soundName);
}

void PlaySoundOnStart::Update(float dt)
{
  // Get whatever comp needed for every frame
  //GetComponent<PhysicsComp>(p);
  //GetComponent<TransformComp>(t);

  // Logic
}

void PlaySoundOnStart::OnDestroy()
{
}

void PlaySoundOnStart::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write(GetName());
  sr.Write("soundName", soundName);
  // Write whaever you need to save here
  // sr.Write("Health", someInt);
}

void PlaySoundOnStart::Deserialize(Deserializer& d)
{
  // Read whaever you need to save here
  // d.Read("Health", someInt);
  d.Read("soundName", soundName);
}

void PlaySoundOnStart::Reflect(const Reflector& r)
{
  // Reflect whatever
  // r.Reflect("Health", someInt);
  r.Reflect("soundName", soundName);
  ASC_EDITOR.HelpMarker("Make sure object have sound comp and the respective sound");
}

//void PlaySoundOnStart::OnCollisionEnter(GOHandle other)
//{
//}
//
//void PlaySoundOnStart::OnCollisionStay(GOHandle other)
//{
//}
//
//void PlaySoundOnStart::OnCollisionExit(GOHandle other)
//{
//}

void PlaySoundOnStart::HandleMessage(MsgData_Base *m)
{
}