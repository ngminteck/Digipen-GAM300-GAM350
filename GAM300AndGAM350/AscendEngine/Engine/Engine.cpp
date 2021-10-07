/******************************************************************************/
/*!
\file   Engine.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for Engine

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

// No .xml extension
#ifdef _SUBMISSION
static const char *startUpFile = "Splash";
#else
static const char *startUpFile = "PTest";//"Level1";
#endif
std::chrono::duration<float, std::milli> realtime;

Engine::Engine()
    : currState(LOADING), 
  man_graph(man_trans.m_components),
  man_phy(man_trans.m_components),
  man_waypoint(man_trans.m_components),
  man_uitext(man_trans.m_components),
  man_newphy(man_trans.m_components),
  man_animation(man_trans.m_components),
  managers({
    { PHYSICS,   &man_phy},
    { LOGIC,     &man_logic },
    { TRANSFORM, &man_trans },
    { LIGHT,     &man_light },
    { SOUND,     &man_sound },
    { PARTICLE,  &man_particle },
    { GRAPHICS,  &man_graph },
    { UITEXT,    &man_uitext },
    { WAYPOINT,  &man_waypoint },
    { TAG,       &man_tag},
    { NEWPHYSICS,   &man_newphy},
    { ANIMATION,   &man_animation},
    //{ OBJMAN,  &man_ObjMan },

}),
factory(managers),
//sceneToLoad(move(GetLastOpenedFile()))
sceneToLoad(GetLastOpenedFile())
{
  ASC_LINERENDER.InitGL();
  ASC_LIGHTS.InitGL();

  ASC_EDITOR.Init(factory, man_graph.GetCamera(), COMPONENT_TYPE::MAX + auxSysNum);
  ASC_EDITOR.SetLoadedScene(sceneToLoad);

  // Register the messages to use for engine
  ASC_MSGSYS.RegisterCallBack(mTYPE_CHANGE_SCENE, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_RESUME, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PAUSE, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAMEPAUSE, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_LOADING, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_SYSTEM_GAIN_FOCUS, &Engine::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_SYSTEM_QUIT, &Engine::HandleMsg, this);

  //extern PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
  //extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
  //extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
  //wglSwapIntervalEXT(-1);
  //Log("Vsync: ", wglGetSwapIntervalEXT());
  //TowerTypeConvert ttc;
  //EnemyTypeConvert etc;

  std::vector<void (Engine::*)() > temp;

  //Main
  temp.push_back(&Engine::PauseState_Resume);
  temp.push_back(&Engine::PauseState_HTP);
  temp.push_back(&Engine::PauseState_Quit);
  //temp.push_back(&Engine::Main_Credits);
  //temp.push_back(&Engine::Main_Exit);
  functionStates.push_back(std::move(temp));
  temp.clear();

  ////HowToPlay
  //temp.push_back(&Engine::Blank);
  temp.push_back(&Engine::HowToPlayState_Back);
  functionStates.push_back(std::move(temp));
  temp.clear();

  ////Quit
  temp.push_back(&Engine::Quit_Yes);
  temp.push_back(&Engine::Quit_No);

  functionStates.push_back(std::move(temp));
  temp.clear();


}

Engine::~Engine()
{
 
  
}


void Engine::Update(bool& closeTriggered, float dt)
{
  inputManager.Update(dt);
  Timer::Update(dt);

  if (ASC_KEYTRIGGERED(ASC_KEY_F11))
    ASC_SYSTEM.ToggleFullscreen();

  // PAUSE
#if _SUBMISSION
  if (currState != HTP  && currState != PAUSECONFIRMATION && ASC_KEYTRIGGERED(ASC_KEY_ESCAPE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::START))
      if (ASC_ENGINE.CurrentScene() != "MainMenu")
          ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAMEPAUSE));
#else
  if (currState != STOP && currState != HTP && currState != PAUSECONFIRMATION && ASC_KEYTRIGGERED(ASC_KEY_ESCAPE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::START))
          ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAMEPAUSE));
#endif
  // Exit from Pause Menu (temp method)
  if (currState == Engine::STATES::PAUSE && (ASC_KEYTRIGGERED(ASC_KEY_BACKSPACE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::BACK)))
      ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
  
  //grid stuff
  man_grid.DrawAllGrid();
  //tweening.Update(dt);
  mTween.Update(dt);
  ProcessState(closeTriggered, dt);

#ifndef _SUBMISSION
  ASC_EDITOR.Update(dt);
#endif
  ASC_MSGSYS.Update();

  factory.Update();
}



void Engine::HandleMsg(MsgData_Base* msg)
{
  switch (msg->type)
  {
  case mTYPE_CHANGE_SCENE:
  {
    Msg_Change_Scene *m = static_cast<Msg_Change_Scene *>(msg);     
    sceneToLoad = m->sceneName;
    currState = CHANGESCENE;
  } break;

  case mTYPE_LOADING:
  {
    currState = LOADING;
  }
  break;

  case mTYPE_SYSTEM_QUIT:
  {
    currState = QUIT;
  }
  break;

  case mTYPE_PLAY:
  {
    ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PLAY_SCRIPT_INIT));
    currState = PLAY;
  }
  break;

  case mTYPE_RESUME:
  {
    currState = PLAY;
  } break;
  case mTYPE_GAMEPAUSE:
  {
      auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
      auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
      auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
      auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
      auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
      auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");

      if (currState == Engine::STATES::PAUSE)
      {
          ASC_INPUT.DisplayCursor(false);
          if (PauseScreen.IsValid() && Paused_BG.IsValid())
          {
              Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
          }

          currState = Engine::STATES::PLAY;
      }
      else
      {
          ASC_INPUT.DisplayCursor(true);
          if (PauseScreen.IsValid() && Paused_BG.IsValid())
          {
              Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
              PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
              Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
              htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
              Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
              Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
          }

          currState = Engine::STATES::PAUSE;
      }

      break;
  }
  case mTYPE_PAUSE:
  case mTYPE_STOP:
  {
    currState = STOP;
  } break;

  case mTYPE_SYSTEM_GAIN_FOCUS:
  {
    for (auto &elem : managers)
      elem.second->Load();
  } break;

  default: break;
  }
}
void Engine::CheckInput(float dt)
{
  //Animate PawSelector
 /* if (PawSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
    rotIncreasing = false;
  if (PawSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
    rotIncreasing = true;

  PawSelector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? PawSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : PawSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;*/
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");
  if (ConfirmationSelector.IsValid())
      //Animate PawSelector
  {
      if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
          rotIncreasing = false;
      if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
          rotIncreasing = true;

      ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation =
          rotIncreasing ?
          ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt :
          ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;
  }
  /*if (ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE) || ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
  {
    usingMouse = true;
  }

  if (Up() || Down() || Left() || Right() || Accept())
  {
    usingMouse = false;

  }*/

}
void Engine::ProcessState(bool& closeTriggered, float dt)
{
  if (currState == CHANGESCENE)
  {
    ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_STOP));
    ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_LOADING));
  }
  else if (currState == LOADING)
  {
    if (!man_graph.IsLoading()) 
    {
      factory.UnloadScene();
      char scene[256];
      sprintf_s(scene, ASC_SCENES_FLD"%s.xml", sceneToLoad.c_str());
      factory.LoadScene(scene);

      for (auto &elem : managers)
        elem.second->Load();

      if (ASC_SYSTEM.hack_releasemode_submission)
      {
          ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PLAY));
      }
      else
      {

#ifdef  _SUBMISSION
          ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PLAY));
#else
          currState = STOP;
#endif
      }
    }

    man_trans.Update(dt);
    man_light.Update(dt);
    man_particle.Update(dt);
    man_graph.Update(dt);
    man_uitext.Update(dt);
  }
  else if (currState == QUIT) {
    closeTriggered = true;
    return;
  }
  else if (currState == STOP)
  {
    man_trans.Update(dt);

   // 

#ifdef _SUBMISSION
    man_newphy.isEnabled = true;
 

#endif // 
    man_newphy.Update(dt);

#ifdef _SUBMISSION
    man_logic.isPlaying = true;

    man_logic.Update(dt);

#endif

    man_light.Update(dt);
    man_particle.Update(dt);
    man_graph.Update(dt);
    man_phy.Update(dt);
    man_uitext.Update(dt);
    man_animation.Update(dt);
  

  }
  else if (currState == PLAY)
  {
#ifdef _SUBMISSION
      man_newphy.isEnabled = true;
      man_particle.isGamePause = false;
#endif 
    for (auto &elem : managers)
    {
      elem.second->Update(dt);
      ASC_EDITOR.SetSysPerformaceTime(elem.first, elem.second->duration.count());
    }
    //extra sys
    //ASC_EDITOR.SetSysPerformaceTime(COMPONENT_TYPE::MAX + 1, ASC_TWEEN.duration.count());
  }
  else if (
    currState == Engine::STATES::PAUSE  ||
    currState == Engine::STATES::PAUSECONFIRMATION ||
    currState == Engine::STATES::HTP)
  {
    man_particle.Update(dt);
    man_graph.Update(dt);
    man_uitext.Update(dt);

    if (hState != prevState)
    {
        ASC_SOUNDMAN.Play(ASC_FACTORY.GetGameObjectByName("PauseBackground")->GetHdl<SoundComp>(), "SFX_Hover.ogg");
        prevState = hState;
    }
    man_sound.Update(dt);
    CheckInput(dt);
    if (usingMouse)
    {
        MouseUpdate(dt);
        KeyUpdate(dt);
    }
    else
      GamePadUpdate(dt);
  }
 
}
void Engine::MouseUpdate(float dt)
{
  if (currState == Engine::STATES::PAUSE)
  {
    PauseState(dt);
  }
  else if (currState == Engine::STATES::PAUSECONFIRMATION)
  {
    PauseConfrimation(dt);
  }
  else if (currState == Engine::STATES::HTP)
  {
    HowToPlay(dt);
  }
}
void Engine::KeyUpdate(float dt)
{
    auto currIndex = pauseState[currState];
    int amt = static_cast<int>(functionStates[currIndex].size());
    if (currIndex != 2)
    {
        //up
        if (Up())
        {
            index = (index <= 0) ? amt - 1 : index - 1;
            UpdateSelection(dt);
        }
        //down
        if (Down())
        {
            index = (index >= amt - 1) ? 0 : index + 1;
            UpdateSelection(dt);
        }
    }
    switch (currIndex)
    {
        //std::cout << currIndex << std::endl;  
    case 2://quit 
    {
        if (Left())
        {
            index = (index <= 0) ? amt - 1 : index - 1;
            UpdateSelection(dt);
        }
        //down
        if (Right())
        {
            index = (index >= amt - 1) ? 0 : index + 1;
            UpdateSelection(dt);
        }

    }

    default:
    {
        if (Accept())
        {
            std::invoke(functionStates[static_cast<int>(currIndex)][index], this);
            index = 0;
            UpdateSelection(dt);
            std::cout << "hi\n";
        }

        break;
    }
    }

}
void Engine::GamePadUpdate(float dt)
{
  auto currIndex = pauseState[currState];
  int amt = static_cast<int>(functionStates[currIndex].size());
  if (currIndex != 2)
  {
    //up
    if (Up())
    {
      index = (index <= 0) ? amt - 1 : index - 1;
      UpdateSelection(dt);
    }
    //down
    if (Down())
    {
      index = (index >= amt - 1) ? 0 : index + 1;
      UpdateSelection(dt);
    }
  }
  switch (currIndex)
  {
    //std::cout << currIndex << std::endl;  
    case 2://quit 
    {
      if (Left())
      {
        index = (index <= 0) ? amt - 1 : index - 1;
        UpdateSelection(dt);
      }
      //down
      if (Right())
      {
        index = (index >= amt - 1) ? 0 : index + 1;
        UpdateSelection(dt);
      }
    
    }
  
    default:
    {
      if (Accept())
      {
        std::invoke(functionStates[static_cast<int>(currIndex)][index], this);
        index = 0;
        UpdateSelection(dt);
        std::cout << "hi\n";
      }

      break;
    }
  }
  
}
void Engine::UpdateSelection( float dt )
{
  auto currIndex = pauseState[currState];
 

  switch (currIndex)
  {
  case 0:
  {
 
    UpdateSelection_PauseState(dt);
    break;
  }
  case 1:
    break;
  case 2:
    UpdateSelection_ExitState(dt);
    break;
  default:
    break;
  }
}
void Engine::UpdateSelection_PauseState( float dt )
{
  //first make all not highlight
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
 
  {

    Resume->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
    htpScreen->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
    Quit->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
  }
  switch (index)
  {
  case 0://Resume
  {
    APPCONSOLE.AddLog("Resume Hovered");
    Selector->GetHdl<GraphicsComp>()->posOffset.y = Resume->GetHdl<GraphicsComp>()->posOffset.y;
    Resume->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
    hState = HOVEREDSTATE::Resume;
    std::cout << "test\n";
    break;
  }
  case 1://HowToplay
  {
    APPCONSOLE.AddLog("How To Play Hovered");
    Selector->GetHdl<GraphicsComp>()->posOffset.y = htpScreen->GetHdl<GraphicsComp>()->posOffset.y;
    htpScreen->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
    hState = HOVEREDSTATE::How;
    break;
  }
  case 2://QUit
  {
    APPCONSOLE.AddLog("Quit Hovered");
    Selector->GetHdl<GraphicsComp>()->posOffset.y = Quit->GetHdl<GraphicsComp>()->posOffset.y;
    Quit->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
    hState = HOVEREDSTATE::Quit;
    break;
  }
  }
}
void Engine::UpdateSelection_ExitState(float dt)
{


  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
 
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");

  {
    YesButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
    NoButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
  }

  switch (index)
  {
  case 0:
  {
    APPCONSOLE.AddLog("YesButton Hovered");
    YesButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
    ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = -0.22f;
    hState = HOVEREDSTATE::Yes;
    break;
  }
  case 1:
  {
    APPCONSOLE.AddLog("NoButton Hovered");
    NoButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
    ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.09f;
    hState = HOVEREDSTATE::No;
    break;
  }

  }
}
void Engine::PauseState(float dt)
{
  APPCONSOLE.AddLog("currState == PAUSE");
  man_particle.Update(dt);
  man_graph.Update(dt);
  man_uitext.Update(dt);

  // mouse fn
  float width = static_cast<float>(ASC_SYSTEM.winWidth);
  float height = static_cast<float>(ASC_SYSTEM.winHeight);

  Vector2 MousePos = ASC_INPUT.GetMousePos();
  MousePos.x = (MousePos.x - width / 2) / width * 2;
  MousePos.y = -(MousePos.y - height / 2) / height * 2;

  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");

  if (PauseScreen.IsValid() && Paused_BG.IsValid())
  {
    // Resume 
    {
      auto& model_container = ASC_GMAN.m_objLoader.m_models.find(Resume->GetHdl<GraphicsComp>()->modelfile)->second;
      Vec3 size = model_container[0].maxPos - model_container[0].minPos;
      Vec3 pos = Resume->GetHdl<GraphicsComp>()->positionHdl->pos + Resume->GetHdl<GraphicsComp>()->posOffset;
      Vec3 min;
      min.x = pos.x - size.x / 1.2f;
      min.y = pos.y - size.y / 4.0f;
      Vec3 max;
      max.x = pos.x + size.x / 5.0f;
      max.y = pos.y + size.y / 4.2f;

      if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
      {
        if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
        {
          PauseState_Resume();
          
        }
        else
        {
          APPCONSOLE.AddLog("Resume Hovered");
          Selector->GetHdl<GraphicsComp>()->posOffset.y = Resume->GetHdl<GraphicsComp>()->posOffset.y;
          Resume->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
          hState = HOVEREDSTATE::Resume;
          //ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.16f;
        }
      }
      else
      {
        Resume->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
      }
    }
    // How To Play 
    {
      auto& model_container = ASC_GMAN.m_objLoader.m_models.find(htpScreen->GetHdl<GraphicsComp>()->modelfile)->second;
      Vec3 size = model_container[0].maxPos - model_container[0].minPos;
      Vec3 pos = htpScreen->GetHdl<GraphicsComp>()->positionHdl->pos + htpScreen->GetHdl<GraphicsComp>()->posOffset;
      Vec3 min;
      min.x = pos.x - size.x / 1.2f;
      min.y = pos.y - size.y / 4.0f;
      Vec3 max;
      max.x = pos.x + size.x / 5.0f;
      max.y = pos.y + size.y / 4.2f;

      if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
      {
        if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
        {
          APPCONSOLE.AddLog("How To Play Clicked");
          PauseState_HTP();
          return;
        }
        else
        {
          APPCONSOLE.AddLog("How To Play Hovered");
          hState = HOVEREDSTATE::How;
          Selector->GetHdl<GraphicsComp>()->posOffset.y = htpScreen->GetHdl<GraphicsComp>()->posOffset.y;
          htpScreen->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
          //ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.16f;
        }
      }
      else
      {
        htpScreen->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
      }
    }
    // Quit 
    {
      auto& model_container = ASC_GMAN.m_objLoader.m_models.find(Quit->GetHdl<GraphicsComp>()->modelfile)->second;
      Vec3 size = model_container[0].maxPos - model_container[0].minPos;
      Vec3 pos = Quit->GetHdl<GraphicsComp>()->positionHdl->pos + Quit->GetHdl<GraphicsComp>()->posOffset;
      Vec3 min;
      min.x = pos.x - size.x / 1.2f;
      min.y = pos.y - size.y / 4.0f;
      Vec3 max;
      max.x = pos.x + size.x / 5.0f;
      max.y = pos.y + size.y / 4.2f;

      if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
      {
        if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
        {
          PauseState_Quit();
        }
        else
        {
          APPCONSOLE.AddLog("Quit Hovered");
          hState = HOVEREDSTATE::Quit;
          Selector->GetHdl<GraphicsComp>()->posOffset.y = Quit->GetHdl<GraphicsComp>()->posOffset.y;
          Quit->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
          //ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.16f;
        }
      }
      else
      {
        Quit->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
      }
    }

    //Animate PawSelector
    if (Selector->GetHdl<GraphicsComp>()->zRotation > maxRot)
      rotIncreasing = false;
    if (Selector->GetHdl<GraphicsComp>()->zRotation < minRot)
      rotIncreasing = true;

    Selector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? Selector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : Selector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;
  }
}
void Engine::PauseConfrimation(float dt)
{
  man_particle.Update(dt);
  man_graph.Update(dt);
  man_uitext.Update(dt);

  Vector2 MousePos = ASC_INPUT.GetMousePos();
  float width = static_cast<float>(ASC_SYSTEM.winWidth);
  float height = static_cast<float>(ASC_SYSTEM.winHeight);
  MousePos.x = (MousePos.x - width / 2) / width * 2;
  MousePos.y = -(MousePos.y - height / 2) / height * 2;

  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");


  // Display(Unhide) the Selector
  ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  //YES 
  {
    auto& model_container = ASC_GMAN.m_objLoader.m_models.find(YesButton->GetHdl<GraphicsComp>()->modelfile)->second;
    Vec3 size = model_container[0].maxPos - model_container[0].minPos;
    Vec3 pos = YesButton->GetHdl<GraphicsComp>()->positionHdl->pos + YesButton->GetHdl<GraphicsComp>()->posOffset;
    Vec3 min;
    min.x = pos.x - size.x / 4.2f;
    min.y = pos.y - size.y / 4.0f;

    Vec3 max;
    max.x = pos.x + size.x / 6.5f;
    max.y = pos.y + size.y / 3.0f;
    if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
    {
      if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
      {
        APPCONSOLE.AddLog("YesButton Clicked");
        ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
        return;
      }
      else
      {
        APPCONSOLE.AddLog("YesButton Hovered");
        hState = HOVEREDSTATE::Yes;
        YesButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
        ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = -0.22f;
      }
    }
    else
    {
      YesButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
    }
  }
  //No 
  {
    auto& model_container = ASC_GMAN.m_objLoader.m_models.find(NoButton->GetHdl<GraphicsComp>()->modelfile)->second;
    Vec3 size = model_container[0].maxPos - model_container[0].minPos;
    Vec3 pos = NoButton->GetHdl<GraphicsComp>()->positionHdl->pos + NoButton->GetHdl<GraphicsComp>()->posOffset;
    Vec3 min;
    min.x = pos.x - size.x / 4.7f;
    min.y = pos.y - size.y / 4.0f;
    Vec3 max;
    max.x = pos.x + size.x / 7.5f;
    max.y = pos.y + size.y / 3.0f;
    if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
    {
      if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
      {
        APPCONSOLE.AddLog("NoButton Clicked");
        //Hide Dialog
        ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
        YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
        NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
        ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

        // Unhide Dialog
        Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
        PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
        Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
        htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
        Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
        Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
        currState = Engine::STATES::PAUSE;
        return;
      }
      else
      {
        APPCONSOLE.AddLog("NoButton Hovered");
        hState = HOVEREDSTATE::No;
        NoButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
        ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.09f;
      }
    }
    else
    {
      NoButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
    }
  }

  ////Animate PawSelector
  //if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
  //  rotIncreasing = false;
  //if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
  //  rotIncreasing = true;

  //ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation =
  //  rotIncreasing ?
  //  ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt :
  //  ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;
}
void Engine::HowToPlay(float dt)
{
  man_particle.Update(dt);
  man_graph.Update(dt);
  man_uitext.Update(dt);

  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");

  if (ASC_KEYRELEASED(ASC_KEY_ESCAPE))
  {
    // Hide HTPScreen
    GOHandle& HowToPlayScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayScreen");
    HowToPlayScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

    // Unhide Dialog
    Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
    PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

    currState = Engine::STATES::PAUSE;
    ASC_INPUT.ClearBuffer();
    return;
  }
}

void Engine::PauseState_Resume()
{
  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");
  APPCONSOLE.AddLog("Quit Clicked");
  //ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));

  APPCONSOLE.AddLog("Resume Clicked");

  // Hide Dialog
  Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

  ASC_INPUT.DisplayCursor(false);
  currState = Engine::STATES::PLAY;

}
void Engine::PauseState_HTP()
{
  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");
  
  APPCONSOLE.AddLog("How To Play Clicked");
  // Hide Dialog
  Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

  htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayScreen");
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  currState = STATES::HTP;
}
void Engine::PauseState_Quit()
{
  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");
  APPCONSOLE.AddLog("Quit Clicked");
  //ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));

  // Hide Dialog
  Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

  // Show Confirmation Dialog
  ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  currState = STATES::PAUSECONFIRMATION;
  return;
}

void Engine::HowToPlayState_Back()
{

  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");


    // Hide HTPScreen
    GOHandle& HowToPlayScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayScreen");
    HowToPlayScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

    // Unhide Dialog
    Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
    PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
    Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

    currState = Engine::STATES::PAUSE;
   // ASC_INPUT.ClearBuffer();
   // return;

}
void Engine::Quit_Yes()
{
  APPCONSOLE.AddLog("YesButton Clicked");
  ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
}
void Engine::Quit_No()
{
  auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
  auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
  auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
  auto& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
  auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
  auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
  auto& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  auto& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  auto& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  auto& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");


  APPCONSOLE.AddLog("NoButton Clicked");
  //Hide Dialog
  ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

  // Unhide Dialog
  Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  currState = Engine::STATES::PAUSE;
}

bool Engine::Up()
{
  
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP);
  // (ASC_CONTROLLER1.LeftStick_Y() >0.1)? true : false;
}
bool Engine::Down()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN);

}
bool Engine::Left()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT);
}
bool Engine::Right()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT);
}
bool Engine::Accept()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::B);

}

bool Engine::UpMouseKey()
{

    return ASC_KEYTRIGGERED(ASC_KEY_UP) ||
        ASC_KEYTRIGGERED(ASC_KEY_W) ||
        ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP);
    // (ASC_CONTROLLER1.LeftStick_Y() >0.1)? true : false;
}
bool Engine::DownMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_DOWN) ||
        ASC_KEYTRIGGERED(ASC_KEY_S);

}
bool Engine::LeftMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
        ASC_KEYTRIGGERED(ASC_KEY_A);
}
bool Engine::RightMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
        ASC_KEYTRIGGERED(ASC_KEY_D);
}
bool Engine::AcceptMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_SPACE);

}