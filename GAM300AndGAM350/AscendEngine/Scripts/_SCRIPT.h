/******************************************************************************/
/*!
\file   _SCRIPT.h
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
#pragma once

/*
  Add new class/type to :
  1) ScriptHelper.h     ScriptIndex
  2) ScriptCreator.h    SItype::Create
*/
class _SCRIPT : public SCRIPTING::ScriptCommonIF<_SCRIPT>
{
public:
  _SCRIPT();
  ~_SCRIPT();

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