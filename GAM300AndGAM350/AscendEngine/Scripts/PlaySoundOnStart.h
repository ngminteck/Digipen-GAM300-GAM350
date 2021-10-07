/******************************************************************************/
/*!
\file   PlaySoundOnStart.h
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
#pragma once

/*
  Add new class/type to :
  1) ScriptHelper.h     ScriptIndex
  2) ScriptCreator.h    SItype::Create
*/
class PlaySoundOnStart : public SCRIPTING::ScriptCommonIF<PlaySoundOnStart>
{
public:
  PlaySoundOnStart();
  ~PlaySoundOnStart();
  std::string soundName;
private:
  void OnStart() override;
  void Update(float dt) override;
  void OnDestroy() override;
  
  void Serialize(Serializer &sr, LogicHandle idx) override;
  void Deserialize(Deserializer &d) override;
  void Reflect(const Reflector& r) override;
  
  //void OnCollisionEnter(GOHandle other) override;
  //void OnCollisionStay(GOHandle other) override;
  //void OnCollisionExit(GOHandle other) override;

  void HandleMessage(MsgData_Base *m);
};