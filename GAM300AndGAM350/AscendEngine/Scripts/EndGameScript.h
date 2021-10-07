/******************************************************************************/
/*!
\file   _SCRIPT.h
\author 
\par    email: \@digipen.edu
\par    Digipen login: 
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
class EndGameScript : public SCRIPTING::ScriptCommonIF<EndGameScript>
{
public:
  EndGameScript();
  ~EndGameScript();

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

  //the screens imag
  GOHandle congrats;
  GOHandle pressanykey;
  GOHandle credits;

  int state = -1;

  float creditWaitTime;
  float time;
  inline void checkTimeAndSetState(int nextstage)
  {
    if (time >= 1.0f)
    {
      state = nextstage;
      time = 0.f;
    }
  }

  void HandleMessage(MsgData_Base *m);
};