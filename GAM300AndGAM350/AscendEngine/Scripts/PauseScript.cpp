/******************************************************************************/
/*!
\file   PauseScript.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

PauseScript::PauseScript() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(PauseScript))
{
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PAUSE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_RESUME, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_WIN, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_LOSE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_QUIT_MENU, HandleMessage);
}

PauseScript::~PauseScript()
{
}

void PauseScript::EnableMenu(bool isEnabled, float dt)
{
  float inc = (isEnabled ? dt : -dt) * 10.0f;

  auto &overlay = pauseOverlay->GetHdl<GraphicsComp>();
  overlay->tint.a += inc;

  pauseLCr->GetHdl<GraphicsComp>()->tint.a += inc;

  for (auto& elem : btns)
    elem->GetHdl<GraphicsComp>()->tint.a += inc;

  if (isEnabled)
  {
    if (overlay->tint.a >= 1.0f)
    {
      overlay->tint.a = 1.0f;
      for (auto& elem : btns)
        elem->GetHdl<GraphicsComp>()->tint.a = 1.0f;

      pauseLCr->GetHdl<GraphicsComp>()->tint.a = 1.0f;

      currState = PAUSESTATE::ACTIVE;
    }
  }
  else
  {
    if (overlay->tint.a <= 0.0f)
    {
      overlay->tint.a = 0.0f;
      for (auto& elem : btns)
        elem->GetHdl<GraphicsComp>()->tint.a = 0.0f;

      //Send resume msg here!
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_RESUME));
      ASC_PLAY("SFX_Pause_menu_unpause_01.ogg");
      ASC_SOUNDMAN.SetGlobaVolume(oldVolume);
      currState = PAUSESTATE::INACTIVE;

      pauseLCr->GetHdl<GraphicsComp>()->tint.a = 0.0f;

      bgTint->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    }
  }
}

void PauseScript::EnableSubMenu(bool isEnabled, float dt)
{
  float inc = (isEnabled ? dt : -dt) * 10.0f;

  auto& h2p = h2pOverlay->GetHdl<GraphicsComp>();

  h2p->tint.a += inc;
  pauseOverlay->GetHdl<GraphicsComp>()->tint.a -= inc;

  pauseLCr->GetHdl<GraphicsComp>()->tint.a -= inc;

  for (auto& elem : btns)
    elem->GetHdl<GraphicsComp>()->tint.a -= inc;

  if (isEnabled)
  {
    if (h2p->tint.a >= 1.0f)
      currState = PAUSESTATE::SUBMENU;
  }
  else
  {
    if (h2p->tint.a < 0.0f)
      currState = PAUSESTATE::ACTIVE;
  }
}

void PauseScript::ShowRestartGameDialog()
{
  restartPanel->GetHdl<GraphicsComp>()->SetVisible(true);
  restartYes->GetHdl<GraphicsComp>()->SetVisible(true);
  restartNo->GetHdl<GraphicsComp>()->SetVisible(true);
  quitArrow->GetHdl<GraphicsComp>()->SetVisible(true);

  currState = RESTART;

  for (auto &elem : btns)
    elem->GetHdl<GraphicsComp>()->tint.a = 0.0f;

  pauseLCr->GetHdl<GraphicsComp>()->tint.a = 0.0f;
}

void PauseScript::ShowQuitDialog()
{
  quitPanel->GetHdl<GraphicsComp>()->SetVisible(true);
  quitYes->GetHdl<GraphicsComp>()->SetVisible(true);
  quitNo->GetHdl<GraphicsComp>()->SetVisible(true);
  quitArrow->GetHdl<GraphicsComp>()->SetVisible(true);

  currState = QUIT;

  for (auto &elem : btns)
    elem->GetHdl<GraphicsComp>()->tint.a = 0.0f;

  pauseLCr->GetHdl<GraphicsComp>()->tint.a = 0.0f;
}

void PauseScript::CheckInput()
{
  if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::START) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::BACK))
  {
    currState = FADEOUT;
    return;
  }

  if (ASC_KEYTRIGGERED(ASC_KEY_W) || ASC_KEYTRIGGERED(ASC_KEY_UP) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP))
  {
    currentSel--;
    ASC_PLAY("SFX_Hover.ogg");
  }
  else if (ASC_KEYTRIGGERED(ASC_KEY_S) || ASC_KEYTRIGGERED(ASC_KEY_DOWN) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN))
  {
    currentSel++;
    ASC_PLAY("SFX_Hover.ogg");
  }

  currentSel = (currentSel + int(btns.size())) % int(btns.size());
  static const float offsetGap = 0.16f;
  float topPos = btns[0]->GetHdl<GraphicsComp>()->posOffset.y;
  float yPos = topPos - (offsetGap * currentSel);

  pauseLCr->GetHdl<GraphicsComp>()->posOffset.x = -0.315f;
  pauseLCr->GetHdl<GraphicsComp>()->posOffset.y = yPos;

  for (unsigned int i = 0; i < btns.size(); ++i)
  {
    btns[i]->GetHdl<GraphicsComp>()->uvOffset.y = 0.0f / 3.0f;
    btns[i]->GetHdl<GraphicsComp>()->posOffset.x = 0;
    btns[i]->GetHdl<GraphicsComp>()->posOffset.y = topPos - (offsetGap * i);
  }
  btns[currentSel]->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;

  if (ASC_KEYTRIGGERED(ASC_KEY_ENTER) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A))
  {
      ASC_PLAY("SFX_ButtonClick.ogg");

    switch (currentSel)
    {
    case 0: // Resume
      currState = PAUSESTATE::FADEOUT;
      break;

    case 1: // Restart
      ShowRestartGameDialog();
      break;

    case 2: // Options
      currState = PAUSESTATE::OPTIONS;

      optionBG->GetHdl<GraphicsComp>()->SetVisible(true);
      optionButtonState = fullscreen;
      optionArrow->GetHdl<GraphicsComp>()->SetVisible(true);
      optionFullScreenText->GetHdl<UITextComp>()->SetVisible(true);
      optionBgmText->GetHdl<UITextComp>()->SetVisible(true);
      optionVolumeText->GetHdl<UITextComp>()->SetVisible(true);


      optionArrow->GetHdl<GraphicsComp>()->posOffset.y = 0.31f;

      break;

    case 3: // How To Play
    {
      currState = PAUSESTATE::FADEIN_SUB;
    } break;

    case 4: // Credits
      currState = PAUSESTATE::CREDITS;
      creditsScreen->GetHdl<GraphicsComp>()->SetVisible(true);
      break;

    case 5: // Quit
      ShowQuitDialog();
      break;
    };
  }
}

void PauseScript::OnStart()
{
  isGameOver = false;

  bgTint = LoadFromPrefab("PauseOverlay");
  currentLevel = ASC_ENGINE.CurrentScene().c_str();

  quitArrow = LoadFromPrefab("UI_QuitArrow");
  quitPanel = LoadFromPrefab("UI_Quit_Panel");
  restartPanel = LoadFromPrefab("UI_Restart_Panel");

  optionBG = LoadFromPrefab("UI_OPTION_BG");
  optionBG->GetHdl<GraphicsComp>()->SetVisible(false);
  optionArrow = LoadFromPrefab("UI_OPTION_ARROW");
  optionArrow->GetHdl<GraphicsComp>()->SetVisible(false);

  optionFullScreenText = LoadFromPrefab("UI_OPTION_FULLSCREEN_TEXT");
  optionFullScreenText->GetHdl<UITextComp>()->SetVisible(false);

  optionBgmText = LoadFromPrefab("UI_OPTION_BGM_TEXT");
  optionBgmText->GetHdl<UITextComp>()->SetVisible(false);

  optionVolumeText = LoadFromPrefab("UI_OPTION_VOLUME_TEXT");
  optionVolumeText->GetHdl<UITextComp>()->SetVisible(false);
  optionVolumeText->GetHdl<UITextComp>()->SetText("1");

  creditsScreen = LoadFromPrefab("UI_CREDITS_SCREEN");
  creditsScreen->GetHdl<GraphicsComp>()->SetVisible(false);

  auto &tComp = quitPanel->GetHdl<TransformComp>();
  quitYes = tComp->FindChild("UI_Quit_Yes")->GetGOHandle();
  quitNo = tComp->FindChild("UI_Quit_No")->GetGOHandle();

  auto &restarttComp = restartPanel->GetHdl<TransformComp>();
  restartYes = restarttComp->FindChild("UI_Quit_Yes")->GetGOHandle();
  restartNo = restarttComp->FindChild("UI_Quit_No")->GetGOHandle();

  quitPanel->GetHdl<GraphicsComp>()->SetVisible(false);
  restartPanel->GetHdl<GraphicsComp>()->SetVisible(false);

  quitYes->GetHdl<GraphicsComp>()->SetVisible(false);
  quitYes->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;

  quitNo->GetHdl<GraphicsComp>()->SetVisible(false);

  restartYes->GetHdl<GraphicsComp>()->SetVisible(false);
  restartYes->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;
  restartNo->GetHdl<GraphicsComp>()->SetVisible(false);

  quitArrow->GetHdl<GraphicsComp>()->SetVisible(false);

  if (bgTint.IsValid())
  {
    bgTint->GetHdl<GraphicsComp>()->tint.a = 0.0f;

    GOHandle resumeBtn;
    GOHandle restartBtn;
    GOHandle optionBtn;
    GOHandle howtoplayBtn;
    GOHandle creditsBtn;
    GOHandle quitBtn;

    auto &tBG = bgTint->GetHdl<TransformComp>();
    pauseOverlay = tBG->FindChild("Paused_BG")->GetGOHandle();
    h2pOverlay = tBG->FindChild("HowToPlay_BG")->GetGOHandle();
    pauseLCr = tBG->FindChild("UI_LeftArrow")->GetGOHandle();

    pauseOverlay->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    h2pOverlay->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    pauseLCr->GetHdl<GraphicsComp>()->tint.a = 0.0f;

    resumeBtn = tBG->FindChild("RESUME_BTN")->GetGOHandle();
    restartBtn = tBG->FindChild("RESTART_BTN")->GetGOHandle();
    optionBtn = tBG->FindChild("OPTIONS_BTN")->GetGOHandle();
    howtoplayBtn = tBG->FindChild("H2P_BTN")->GetGOHandle();
    creditsBtn = tBG->FindChild("CREDITS_BTN")->GetGOHandle();
    quitBtn = tBG->FindChild("QUIT_BTN")->GetGOHandle();

    resumeBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    restartBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    optionBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    howtoplayBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    creditsBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;
    quitBtn->GetHdl<GraphicsComp>()->tint.a = 0.0f;

    btns.push_back(resumeBtn);
    btns.push_back(restartBtn);
    btns.push_back(optionBtn);
    btns.push_back(howtoplayBtn);
    btns.push_back(creditsBtn);
    btns.push_back(quitBtn);
  }

  currentSel = 0;
  oldVolume = ASC_SOUNDMAN.GetGlobalVolume();
}

void PauseScript::Update(float dt)
{
  std::cout << "what \n";
  if (isGameOver) return;

  switch (currState)
  {
  case PAUSESTATE::INACTIVE:
    std::cout << " whatttttttt\n";
    if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE))
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_PAUSE));
    return;
    break;
  case PAUSESTATE::FADEIN_SUB:
  case PAUSESTATE::FADEOUT_SUB:
    EnableSubMenu(currState == PAUSESTATE::FADEOUT_SUB ? false : true, dt);
    break;

  case PAUSESTATE::FADEIN:
  case PAUSESTATE::FADEOUT:
    EnableMenu(currState == FADEOUT ? false : true, dt);
    break;
  case PAUSESTATE::ACTIVE:
    CheckInput();
    break;
  case PAUSESTATE::SUBMENU:
    if (ASC_ANYKEYPRESSED()) currState = PAUSESTATE::FADEOUT_SUB;
    break;

  case PAUSESTATE::CREDITS:
  {
    if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::B))
    {
      currState = PAUSESTATE::SUBMENU;
      creditsScreen->GetHdl<GraphicsComp>()->SetVisible(false);
    }
  } break;

  case PAUSESTATE::OPTIONS:
  {
    static const auto move = 0.27f;
    if (ASC_KEYTRIGGERED(ASC_KEY_ESCAPE) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::B))
    {
      optionBG->GetHdl<GraphicsComp>()->SetVisible(false);
      currState = SUBMENU;
      optionArrow->GetHdl<GraphicsComp>()->SetVisible(false);

      optionFullScreenText->GetHdl<UITextComp>()->SetVisible(false);
      optionBgmText->GetHdl<UITextComp>()->SetVisible(false);
      optionVolumeText->GetHdl<UITextComp>()->SetVisible(false);

    }

    if (ASC_KEYTRIGGERED(ASC_KEY_W) ||
      ASC_KEYTRIGGERED(ASC_KEY_UP) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP))
    {
      if (optionButtonState == fullscreen)
      {
        return;
      }

      optionButtonState = OptionState(optionButtonState - 1);
      optionArrow->GetHdl<GraphicsComp>()->posOffset.y += move;
    }
    else if (ASC_KEYTRIGGERED(ASC_KEY_S) ||
      ASC_KEYTRIGGERED(ASC_KEY_DOWN) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN))
    {
      if (optionButtonState == volume)
      {
        return;
      }

      optionButtonState = OptionState(optionButtonState + 1);
      optionArrow->GetHdl<GraphicsComp>()->posOffset.y -= move;
    }

    if (optionButtonState == fullscreen)
    {
      if (ASC_KEYTRIGGERED(ASC_KEY_A) ||
        ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
        ASC_KEYTRIGGERED(ASC_KEY_D) ||
        ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A))
      {
        ASC_SYSTEM.ToggleFullscreen();
      }

      auto text = ASC_SYSTEM.GetFullScreen() ? "Yes" : " No";
      optionFullScreenText->GetHdl<UITextComp>()->SetText(text);
    }
    else if (optionButtonState == bgm)
    {
      if (ASC_KEYTRIGGERED(ASC_KEY_A) ||
        ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
        ASC_KEYTRIGGERED(ASC_KEY_D) ||
        ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A))
      {
        ASC_SOUNDMAN.MuteBGM(!ASC_SOUNDMAN.IsBGMMuted());
      }

      auto text = ASC_SOUNDMAN.IsBGMMuted() ? "Yes" : " No";
      optionBgmText->GetHdl<UITextComp>()->SetText(text);
    }
    else if (optionButtonState == volume)
    {
      auto vol = ASC_SOUNDMAN.GetGlobalVolume();

      if (ASC_KEYTRIGGERED(ASC_KEY_A) ||
        ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT))
      {
        if (vol <= 0.0f)
          return;
        //float setVol = ASC_SOUNDMAN.IsBGMMuted() ? 0.0f : vol - 0.1f;
        ASC_SOUNDMAN.SetGlobaVolume(vol - 0.1f);
        oldVolume = ASC_SOUNDMAN.GetGlobalVolume();
      }
      else if (ASC_KEYTRIGGERED(ASC_KEY_D) ||
        ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT))
      {
        if (vol >= 1.0f)
          return;
        //float setVol = ASC_SOUNDMAN.IsBGMMuted() ? 0.0f : vol + 0.1f;
        ASC_SOUNDMAN.SetGlobaVolume(vol + 0.1f);
        oldVolume = ASC_SOUNDMAN.GetGlobalVolume();
      }

      auto text = std::to_string(static_cast<int>(vol * 10.0f));
      optionVolumeText->GetHdl<UITextComp>()->SetText(text);
    }
  } break;

  case PAUSESTATE::QUIT:
  {
    static auto state = 0;

    if (state == 0) //No
    {
      quitYes->GetHdl<GraphicsComp>()->uvOffset.y = 0.0f / 3.0f;
      quitNo->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;
      quitArrow->GetHdl<GraphicsComp>()->posOffset.y = -0.2f;
    }
    else //Yes
    {
      quitYes->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;
      quitNo->GetHdl<GraphicsComp>()->uvOffset.y = 0.0f / 3.0f;
      quitArrow->GetHdl<GraphicsComp>()->posOffset.y = -0.04f;
    }

    if (ASC_KEYTRIGGERED(ASC_KEY_UP) ||
      ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
      ASC_KEYTRIGGERED(ASC_KEY_W) ||
      ASC_KEYTRIGGERED(ASC_KEY_A) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT))
    {
      ASC_PLAY("SFX_Button_UI_scroll_01.ogg");
      state = 1;
    }
    else if (ASC_KEYTRIGGERED(ASC_KEY_DOWN) ||
      ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
      ASC_KEYTRIGGERED(ASC_KEY_D) ||
      ASC_KEYTRIGGERED(ASC_KEY_S) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT))
    {
      ASC_PLAY("SFX_Button_UI_scroll_01.ogg");
      state = 0;
    }

    if (ASC_KEYTRIGGERED(ASC_KEY_ENTER) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A))
    {
      ASC_PLAY("SFX_Button_UI_click_01.ogg");
      if (state == 0)
      {
        quitPanel->GetHdl<GraphicsComp>()->SetVisible(false);
        quitYes->GetHdl<GraphicsComp>()->SetVisible(false);
        quitNo->GetHdl<GraphicsComp>()->SetVisible(false);
        quitArrow->GetHdl<GraphicsComp>()->SetVisible(false);

        for (auto &elem : btns)
          elem->GetHdl<GraphicsComp>()->tint.a = 1.0f;

        pauseLCr->GetHdl<GraphicsComp>()->tint.a = 1.0f;
        pauseOverlay->GetHdl<GraphicsComp>()->tint.a = 1.0f;

        currState = SUBMENU;
      }
      else if (state == 1)
      {
        ASC_PLAY("SFX_Pause_menu_unpause_01.ogg");
        ASC_SOUNDMAN.SetGlobaVolume(oldVolume);
        //ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));

        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));
      }
    }
  } break;

  case PAUSESTATE::RESTART:
  {
    static auto state = 0;

    if (state == 0) //No
    {
      restartYes->GetHdl<GraphicsComp>()->uvOffset.y = 0.0f / 3.0f;
      restartNo->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;
      quitArrow->GetHdl<GraphicsComp>()->posOffset.y = -0.2f;
    }
    else //Yes
    {
      restartYes->GetHdl<GraphicsComp>()->uvOffset.y = 1.0f / 3.0f;
      restartNo->GetHdl<GraphicsComp>()->uvOffset.y = 0.0f / 3.0f;
      quitArrow->GetHdl<GraphicsComp>()->posOffset.y = -0.04f;
    }

    if (ASC_KEYTRIGGERED(ASC_KEY_UP) ||
      ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
      ASC_KEYTRIGGERED(ASC_KEY_W) ||
      ASC_KEYTRIGGERED(ASC_KEY_A) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT))
    {
      ASC_PLAY("SFX_Button_UI_scroll_01.ogg");
      state = 1;
    }
    else if (ASC_KEYTRIGGERED(ASC_KEY_DOWN) ||
      ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
      ASC_KEYTRIGGERED(ASC_KEY_D) ||
      ASC_KEYTRIGGERED(ASC_KEY_S) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT))
    {
      ASC_PLAY("SFX_Button_UI_scroll_01.ogg");
      state = 0;
    }

    if (ASC_KEYTRIGGERED(ASC_KEY_ENTER) ||
      ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A))
    {
      ASC_PLAY("SFX_Button_UI_click_01.ogg");
      if (state == 0)
      {
        restartPanel->GetHdl<GraphicsComp>()->SetVisible(false);
        restartYes->GetHdl<GraphicsComp>()->SetVisible(false);
        restartNo->GetHdl<GraphicsComp>()->SetVisible(false);
        quitArrow->GetHdl<GraphicsComp>()->SetVisible(false);

        for (auto &elem : btns)
          elem->GetHdl<GraphicsComp>()->tint.a = 1.0f;

        pauseLCr->GetHdl<GraphicsComp>()->tint.a = 1.0f;
        pauseOverlay->GetHdl<GraphicsComp>()->tint.a = 1.0f;

        currState = SUBMENU;
      }
      else if (state == 1)
      {
        ASC_PLAY("SFX_Pause_menu_unpause_01.ogg");
        ASC_SOUNDMAN.SetGlobaVolume(oldVolume);
        //ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));

        ASC_MSGSYS.SendMsg(Msg_Change_Scene(currentLevel));
      }
    }
  } break;
  }
}

void PauseScript::OnDestroy()
{
  btns.clear();
}

void PauseScript::Serialize(Serializer & sr, LogicHandle idx)
{
  sr.Write(GetName());
}

void PauseScript::Deserialize(Deserializer & d)
{
}

void PauseScript::Reflect(const Reflector & r)
{
}

void PauseScript::HandleMessage(MsgData_Base * m)
{
  switch (m->type)
  {
  case  mTYPE_GAME_PAUSE:
  {
    // Make sure quit/restart dialog is not activated
    if (quitPanel->GetHdl<GraphicsComp>()->GetVisible() || 
     restartPanel->GetHdl<GraphicsComp>()->GetVisible())
    {
      return;
    }

    oldVolume = ASC_SOUNDMAN.GetGlobalVolume();
    ASC_SOUNDMAN.SetGlobaVolume(oldVolume * 0.1f);
    ASC_PLAY("SFX_Pause_menu_pause_01.ogg");
    currState = PAUSESTATE::FADEIN;
    GraphicsComp * bg = ASC_GETCOMP_PTR(GraphicsComp, bgTint);
    bg->tint.a = 0.2f;
  }
  break;

  case mTYPE_GAME_RESUME:
  {
    ASC_SOUNDMAN.SetGlobaVolume(oldVolume);
  }break;

  case mTYPE_GAME_QUIT_MENU:
    ShowQuitDialog();
    break;

  case mTYPE_GAME_LOSE:
  case mTYPE_GAME_WIN:
    isGameOver = true;
    break;

  default: break;
  };
}
