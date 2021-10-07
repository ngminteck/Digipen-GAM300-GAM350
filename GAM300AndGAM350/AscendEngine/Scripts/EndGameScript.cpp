/******************************************************************************/
/*!
\file   EndGameScript.cpp
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
#include "pch.h"

EndGameScript::EndGameScript() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(EndGameScript))
{
  /*auto fn = [&](MsgData_Base *mb)
  {
    switch (mb->type)
    {
    }
  };*/
  //RegisterMsg(mTYPE_GAME_SKIPTUTORIAL, &EndGameScript::HandleMessage);
  //ASCEndGameScriptMAN.SendInternalMsg(moneyFb->GetHdl<LogicComp>(),
  //Msg_Game_Vec3Val(goHdl->GetHdl<TransformComp>()->pos, mTYPE_GAME_EARN_MONEY));
  //SendMsg(mTYPE_GAME_STOP_SPAWN_ENEMIES);
}

EndGameScript::~EndGameScript()
{
}

void EndGameScript::OnStart()
{
	ASC_LOOP("BGM_ambience_01.ogg");

  congrats = ASC_FACTORY.CreateGOfromPB("UI_ENDGAME_SCREEN");
  congrats->GetHdl<GraphicsComp>()->SetVisible(false);
  pressanykey = ASC_FACTORY.CreateGOfromPB("UI_PRESS_ANYKEY");
  pressanykey->GetHdl<GraphicsComp>()->SetVisible(false);
  credits = ASC_FACTORY.CreateGOfromPB("UI_ENDGAME_CREDITS");
  credits->GetHdl<GraphicsComp>()->SetVisible(false);

  state = 0;
  time = 0.f;
  creditWaitTime = 0.f;
}

void EndGameScript::Update(float dt)
{
  if (state != -1)
  {
    time += dt;
    if (time > 1.0f) time = 1.0f;
  }

  //state machine
  switch (state)
  {
    //enter into credit end scene and start transition
  case 0:
  {
    //go to show presskey
    congrats->GetHdl<GraphicsComp>()->tint.a = time;
    checkTimeAndSetState(1);
  }break;
  //enter pressany key fade in
  case 1:
  {
    pressanykey->GetHdl<GraphicsComp>()->tint.a = time;
    //flipping between 2 and 1 until user press something
    checkTimeAndSetState(2);
    if (ASC_ANYKEYPRESSED())
    {
      state = 3;
      time = 0.f;
    }
  }break;
  //enter pressany key fade off
  case 2:
  {
    pressanykey->GetHdl<GraphicsComp>()->tint.a = 1.0f - time;
    checkTimeAndSetState(1);
    if (ASC_ANYKEYPRESSED())
    {
      state = 3;
      time = 0.f;
    }
  }break;
  //fade off to credit
  case 3:
  {
    pressanykey->GetHdl<GraphicsComp>()->tint.a = 1.0f - time;
    congrats->GetHdl<GraphicsComp>()->tint.a = 1.0f - time;
    checkTimeAndSetState(4);
  }break;
  //fade in credit
  case 4:
  {
    credits->GetHdl<GraphicsComp>()->tint.a = time;
    checkTimeAndSetState(5);
  }break;
  //wait for a few second
  //user must press esc
  case 5:
  {
    /*creditWaitTime += dt;
    if (creditWaitTime > 3.0f)
    {
      state = 6;
      time = 0.f;
    }*/
    if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE))
    {
      state = 6;
      time = 0.f;
    }
  }break;
  //fade off and goto main menu
  case 6:
  {
    credits->GetHdl<GraphicsComp>()->tint.a = 1.0f - time;
    if(time >= 1.0f)
      ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
  }break;

  }//end of crazy state machine

  //ASC_ANYKEYPRESSED();
  /*if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE))
  {
    ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
  }*/
}

void EndGameScript::OnDestroy()
{
}

void EndGameScript::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write(GetName());

  // Write whaever you need to save here
  // sr.Write("Health", someInt);
}

void EndGameScript::Deserialize(Deserializer& d)
{
  // Read whaever you need to save here
  // d.Read("Health", someInt);
}

void EndGameScript::Reflect(const Reflector& r)
{
  // Reflect whatever
  // r.Reflect("Health", someInt);
}

//void EndGameScript::OnCollisionEnter(GOHandle other)
//{
//}
//
//void EndGameScript::OnCollisionStay(GOHandle other)
//{
//}
//
//void EndGameScript::OnCollisionExit(GOHandle other)
//{
//}

void EndGameScript::HandleMessage(MsgData_Base *m)
{
}