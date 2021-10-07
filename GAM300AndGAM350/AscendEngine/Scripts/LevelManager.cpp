/******************************************************************************/
/*!
\file   LevelManager.cpp
\author Zihe Goh
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

std::map<std::string, std::string> LevelCycle = {
  { "Level1","Level2" },
  { "Level2","Level3" },
  { "Level3","Credits" }
};

LevelManager::LevelManager() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(LevelManager))
{
  auto fn = [&](MsgData_Base *mb)
  {
  };

  //ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_STARTCUTSCENE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_ENDCUTSCENE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_LOSE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_WIN, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PAUSE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_RESUME, HandleMessage);
}

LevelManager::~LevelManager()
{
}

void LevelManager::AddLevelTransitionUIObj(GOHandle _gohdl)
{
  if (_gohdl.IsValid())
  {
    UIobjs.emplace_back(_gohdl);
    _gohdl->GetHdl<GraphicsComp>()->tint.a = alpha;
  }
}

void LevelManager::OnStart()
{
  state = mTYPE_GAME_STARTCUTSCENE;
  currentLevel = ASC_ENGINE.CurrentScene().c_str();

  if (currentLevel == "Level1")
    transitionTime = 37.0f;
  else if (currentLevel == "Level2")
    transitionTime = 33.0f;
  else if (currentLevel == "Level3")
    transitionTime = 10.0f;

  countdown = 5.0f;
  isCutscene = true;
  isGameOver = false;
  isLose = false;
  counter = 0.5f;

  m_showText = false;

  UI_WhiteOut = ASC_FACTORY.CreateGOfromPB("UI_WhiteOut");
  UI_FadeOut = UI_WhiteOut->GetHdl<TransformComp>()->GetChild(0)->GetGOHandle();

  UI_WinLose = ASC_FACTORY.CreateGOfromPB("UI_LevelText");
  ASC_GETCOMP_PTR(UITextComp, UI_WinLose)->SetVisible(m_showText);

  UI_Stats1 = UI_WinLose->GetHdl<TransformComp>()->FindChild("UI_Stats1")->GetGOHandle();
  UI_Stats2 = UI_WinLose->GetHdl<TransformComp>()->FindChild("UI_Stats2")->GetGOHandle();
  UI_Btn1 = UI_WinLose->GetHdl<TransformComp>()->FindChild("UI_Btn1")->GetGOHandle();
  UI_Btn2 = UI_WinLose->GetHdl<TransformComp>()->FindChild("UI_Btn2")->GetGOHandle();

  UI_Stats1->GetHdl<UITextComp>()->tint.a = 0.0f;
  UI_Stats2->GetHdl<UITextComp>()->tint.a = 0.0f;
  UI_Btn2->GetHdl<UITextComp>()->tint.a = 0.0f;
  UI_Btn1->GetHdl<UITextComp>()->tint.a = 0.0f;

  UI_Stats1->GetHdl<GraphicsComp>()->tint.a = 0.0f;
  UI_Stats2->GetHdl<GraphicsComp>()->tint.a = 0.0f;

  auto uiBG = ASC_GETCOMP_PTR(GraphicsComp, UI_WinLose);
  uiBG->tint.a = 0.0f;

  ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "BGM_level_start.ogg");

  //HB, from now, level manager will handle player start pos
  player = ASC_FACTORY.GetObjectHandleList(go_player)[0];
  player_t = player->GetHdl<TransformComp>();
  player_g = player->GetHdl<GraphicsComp>();
  player_t->pos = Vec3(0, 10, 0);
    ASC_FACTORY.GetObjectHandleList(go_powerbeacon)[0]->GetHdl<TransformComp>()->pos;
  player_t->pos.y += 30.0f;
  //set player to transparent
  player_g->SetTransparencyVal(0.0f);

  t = player_t;
  ////HB, from now, level manager will handle all cameras
  Vec3 camFwd = (userLookPos - t->pos).Normalize();
  camStartPos = player_t->pos;
  camLookPos = camStartPos + (camFwd * 20.0f);

  ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::LEVEL_CAM);

  Camera& player_cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
  player_cam.SetPosition(camStartPos);
  player_cam.LookAt(camStartPos + Vec3(0,0,1));

  Camera& editor_cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);
  editor_cam.SetPosition(camStartPos);
  editor_cam.LookAt(camLookPos);

  Camera& level_cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::LEVEL_CAM);
  level_cam.SetPosition(camStartPos);
  level_cam.LookAt(camLookPos);

  currentHeight = heightOffset;
  stopCamShakeHeight = 0.1f* currentHeight;
  currentFOV = expandedFOV;
  MTweenFloat(MTweenType::EaseOutCubic, currentHeight, 0.0f, transitionTime);
  MTweenFloat(MTweenType::EaseOutCubic, currentFOV, camStartFOV, transitionTime);

  if (!playedOnce)
  {
    playedOnce = true;
    if (currentLevel == "Level1")
      ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "VO_level_end_narrative_01.ogg");
    else if (currentLevel == "Level2")
      ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "VO_level_end_narrative_02.ogg");
    else if (currentLevel == "Level3")
      ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "VO_level_end_narrative_03.ogg");
  }

  //particles
  Vec3 particle_pos = player_t->pos;
  particle_pos.y = -20.0f;
  {
    GOHandle particle = LoadFromPrefab("Particle_LevelStart");
    auto emitter = particle->GetHdl<ParticleEmitter>();
    particle->GetHdl<TransformComp>()->pos = particle_pos;
    emitter->StartCountdown(transitionTime);
    emitter->Emit();
  }
  {
    particle_pos.y = 10.0f;
    GOHandle particle = LoadFromPrefab("Particle_LevelStart");
    auto emitter = particle->GetHdl<ParticleEmitter>();
    particle->GetHdl<TransformComp>()->pos = particle_pos;
    emitter->emissionType = ParticleEmitter::EMISSION::OVERTIME;
    emitter->spawnRate = 0.001f;
    emitter->SetMaxParticle(300);
    emitter->isLooping = true;
    emitter->StartCountdown(transitionTime);
  }

  Vec4 colorArr[5] = { Color::RED, Color::ORANGE ,Color::GREEN, Color::BLUE, Color::BLUE_LIGHT };
  for(int i = 0; i < 5; ++i)
  {
    Vec3 particle_pos = player_t->pos;
    particle_pos.y = currentHeight / i;
    GOHandle particle = LoadFromPrefab("Particle_LevelStart");
    auto emitter = particle->GetHdl<ParticleEmitter>();
    emitter->SetMaxParticle(200);
    emitter->maxLifetime = transitionTime;
    emitter->minLifetime = transitionTime;
    emitter->startMinCol = emitter->startMaxCol = colorArr[i];
    emitter->isBlending = true;
    emitter->StartCountdown(transitionTime);
    particle->GetHdl<TransformComp>()->pos = particle_pos;
    emitter->Emit();
  }
  {
    //ring
    GOHandle particle = LoadFromPrefab("Level_Start_Pulse");
    auto emitter = particle->GetHdl<ParticleEmitter>();
    particle->GetHdl<TransformComp>()->pos = particle_pos;
    emitter->SetMaxParticle(300);
    emitter->maxLifetime = transitionTime;
    emitter->minLifetime = transitionTime / 2.0f;
    emitter->StartCountdown(transitionTime / 2.0f);
    emitter->isLooping = true;
  }

  {
    GOHandle beacon = ASC_FACTORY.GetObjectHandleList(go_powerbeacon)[0];
    APPLYTO_CHILD(false, beacon, child, "BeaconRing", beaconRing = child;);
    Vec4& col = beaconRing->GetHdl<GraphicsComp>()->tint;
    col = Color::GREEN;
    //col.a = 0.7f;
    auto red = Color::RED;
    red.a = 0.7f;
    MTweenClr(MTweenType::QuinticIn, beaconRing, red, transitionTime);
  }

  ASC_PLAY("SFX_Player_collide_portal_01.ogg");
  ASC_MSGSYS.SendMsg(Msg_Game_FloatVal(transitionTime, mTYPE_GAME_STARTCUTSCENE));
  //SendMsg(Msg_Game_FloatVal( transitionTime, mTYPE_GAME_STARTCUTSCENE));
}

void LevelManager::OnDestroy()
{
}

void LevelManager::Update(float dt)
{
  //Log(ASC_SOUNDMAN.IsPlaying(goHdl->GetHdl<SoundComp>(), "MUSIC_BGM_level_start.ogg"));

  switch (state)
  {
  case mTYPE_GAME_STARTCUTSCENE:
  {
    if (mPaused) return;

    if (!Transition(dt))
    {
      counter = 0.0f;
      state = mTYPE_GAME_ENDCUTSCENE;
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_START_HARVESTER));

      if (isGameOver)
      {
        //std::string tmpkey = currentLevel;
        auto itr = LevelCycle.find(currentLevel);
        std::string tmp; 
        if(itr != LevelCycle.end()) tmp = itr->second;
        //Log("----------------------------",tmp);
        ASC_MSGSYS.SendMsg(Msg_Change_Scene(tmp));
        ASC_GMAN.GetCamera().SetFOV(60.0f);
      }
      else
      {
        ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);

        Camera& player_cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
        player_cam.LookAt(camLookPos);

        ASC_MSGSYS.SendMsg(Msg_UINT(startingScrap, mTYPE_GAME_STARTINGRESOURCE));
        ASC_MSGSYS.SendMsg(Msg_Game_FloatVal(gameTime, mTYPE_GAME_SETGAMETIME));
        SendMsg(state);

        {
          Vec3 particle_pos = player_t->pos;
          particle_pos.y = 20.0f;
          GOHandle particle = LoadFromPrefab("Particle_LevelStart2");
          auto emitter = particle->GetHdl<ParticleEmitter>();
          particle->GetHdl<TransformComp>()->pos = particle_pos;
          emitter->Emit();
        }
        {
          Vec4& col = beaconRing->GetHdl<GraphicsComp>()->tint;
          col = Color::RED;
          col.a = 0.7f;
        }

        //set player to opaque
        player->GetHdl<GraphicsComp>()->SetTransparencyVal(1.0f);
        //ASC_PLAY("SFX_Button_UI_hover_01.ogg");
        //ASC_PLAY("VO_hostiles_detected_01.ogg");
      }
    }
  }
    break;
  case mTYPE_GAME_LOSE:
  case mTYPE_GAME_WIN:
  {
    if (!WinLoseScreen(dt))
    {
      counter = 0.0f;
      state = mTYPE_GAME_STARTCUTSCENE;
      SendMsg(state);
      ASC_PLAY("SFX_Player_collide_portal_01.ogg");
    }
  }
  break;
  default: break;
  }

  UpdateUIObjState(dt);

#if CHEATS
  if (ASC_KEYDOWN(ASC_KEY_LSHIFT))
  {
	  if (ASC_KEYTRIGGERED(ASC_KEY_F1))
	  {
		  ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_WIN));
	  }
	  else if (ASC_KEYTRIGGERED(ASC_KEY_F2))
	  {
		  ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_LOSE));
	  }
	  else if (ASC_KEYTRIGGERED(ASC_KEY_F3))
	  {
		  ASC_MSGSYS.SendMsg(Msg_Game_IntVal(1, mTYPE_GAME_POWERPLANT_ENERGY));
	  }
	  else if (ASC_KEYTRIGGERED(ASC_KEY_F4))
	  {
		  ASC_MSGSYS.SendMsg(Msg_Game_IntVal(1, mTYPE_GAME_ACTIVATE_TELEPORT_BEACON));
	  }
	  else if (ASC_KEYTRIGGERED(ASC_KEY_F6))
	  {
		  ASC_MSGSYS.SendMsg(Msg_Change_Scene(currentLevel));
	  }
	  else if (ASC_KEYTRIGGERED(ASC_KEY_F7))
	  {
		  ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_SKIPTUTORIAL));
	  }
  }
#endif
}

void LevelManager::UpdateUIObjState(float dt)
{
  //check for state and do respective action
  if (state == mTYPE_GAME_ENDCUTSCENE)
  {
    if (alpha >= 1.0f) return;
    else
    {
      alpha += dt;
      if (alpha > 1.0f) alpha = 1.0f;
    }
  }
  else if (state == mTYPE_GAME_STARTCUTSCENE)
  {
    if (alpha <= 0.0f) return;
    else
    {
      alpha -= dt * 4.0f;
      if (alpha < 0.0f) alpha = 0.0f;
    }
  }
  else
    return;

  size_t sz = UIobjs.size();
  while (sz--)
  {
    //remove invalid obj at the same time updating 
    GOHandle gohdl = UIobjs[sz];
    if (gohdl.IsValid())
    {
      GrapHandle ghdl = gohdl->GetHdl<GraphicsComp>();
      ghdl->tint.a = alpha;
    }
    else
    {
      UIobjs[sz] = UIobjs.back();
      UIobjs.pop_back();
    }
  }
}

void LevelManager::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write(GetName());

  // Write whaever you need to save here
   sr.Write("userLookPos", userLookPos);
   sr.Write("StartingScrap", startingScrap);
   sr.Write("GameTime", gameTime);
}

void LevelManager::Deserialize(Deserializer& d)
{
  // Read whaever you need to save here
   d.Read("userLookPos", userLookPos);
   d.Read("StartingScrap", startingScrap);
   d.Read("GameTime", gameTime);
}

void LevelManager::Reflect(const Reflector& r)
{
  // Reflect whatever
  r.Reflect("LookPosition", userLookPos);
  r.Reflect("StartingScrap", startingScrap);
  r.Reflect("GameTime", gameTime);

  Camera& player_cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
  Vec3 pos = player_cam.GetPos();
  Vec3 forward = pos + (player_cam.GetForward() * 20.0f);
  Vec3 right = pos + (player_cam.GetRight() * 20.0f);
  Vec3 up = pos + (player_cam.GetUp() * 20.0f);

  ASC_LINERENDER.AddLine(pos, forward, Color::GREEN);
  ASC_LINERENDER.AddLine(pos, up, Color::RED);
  ASC_LINERENDER.AddLine(pos, right, Color::BLUE);
}


void LevelManager::HandleMessage(MsgData_Base *msg)
{ 
  switch (msg->type)
  {
  case mTYPE_GAME_PAUSE:
  {
    mPaused = true;
    alpha = 1.0f;
    return;
  }

  case mTYPE_GAME_RESUME:
  {
    mPaused = false;
    alpha = 0.0f;
    return;
  }
  default: break;
  }

  if (state == msg->type || isGameOver) return;
  //get the status of the teleport beacon before trying to set anything


  state = msg->type;

  switch (msg->type)
  {
  case mTYPE_GAME_LOSE:
    isLose = true;
  case mTYPE_GAME_WIN:
  {
    SendMsg(mTYPE_GAME_PLAYER_DISABLE_CONTROLS);
    SendMsg(mTYPE_GAME_PLAYER_DISABLE_CAM);

    //ASC_SOUNDMAN.Stop(goHdl->GetHdl<SoundComp>(), "MUSIC_BGM_level_start.ogg");
    m_showText = true;
    auto uiText = ASC_GETCOMP_PTR(UITextComp, UI_WinLose);
    auto uiBG = ASC_GETCOMP_PTR(GraphicsComp, UI_WinLose);

    if (msg->type == mTYPE_GAME_WIN)
    {
      uiBG->tint = ASC_COLOR(0, 200, 170);
      uiText->SetText("Level Complete");
      uiText->offset.Set(-2.92f, 0.43f, 0.0f);
      ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "SFX_Win_01.ogg");
    }
    else
    {
      uiBG->tint = ASC_COLOR(255, 134, 128);
      uiText->SetText("Level Failed");
      uiText->offset.Set(-2.06f, 0.43f, 0.0f);
      ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "Game_over_01.ogg");
    }

    uiText->SetVisible(false);

    uiBG->tint.a = 0.0f;
    uiBG->posOffset.y = 0.06f;
    uiBG->scaleOffset.x = 0.0f;
    uiBG->scaleOffset.y = 0.01f;
    //countdown = transitionTime;
    oCountdown = countdown;
    isGameOver = true;

  } break;

  default:
    break;
  }
}

bool LevelManager::Transition(float dt)
{
  Camera &cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::LEVEL_CAM);

  float& whiteoutA = UI_WhiteOut->GetHdl<GraphicsComp>()->tint.a;
  float& fadeoutA = UI_FadeOut->GetHdl<GraphicsComp>()->tint.a;

  if (ASC_KEYTRIGGERED(ASC_KEY_SPACE))
  {
    cam.SetPosition(camStartPos);
    cam.SetFOV(camStartFOV);
    cam.LookAt(camLookPos);

    fadeoutA = isGameOver ? 1.0f : 0.0f;
    whiteoutA = isGameOver ? 1.0f : 0.0f;



    return false;
  }

  Vec3 pos = cam.GetPos();

  counter += dt;

  if (isGameOver) 
  {
    fadeoutA = ASC_RANGE(fadeoutA + dt, 0.0f, 1.0f);
    if (fadeoutA >= 1.0f)
      whiteoutA = ASC_RANGE(whiteoutA + dt, 0.0f, 1.0f);

    if(whiteoutA >= 1.0f)
      return false;
  }
  else
  {
    whiteoutA = ASC_RANGE(whiteoutA - (dt * 0.75f), 0.0f, 1.0f);
    if(whiteoutA <= 0)
      fadeoutA = ASC_RANGE(fadeoutA - (dt * 0.75f), 0.0f, 1.0f);

    //currentFOV -= dt * transitionTime;
    //currentFOV = ASC_RANGE(currentFOV, camStartFOV, expandedFOV);

    pos.y = camStartPos.y + currentHeight;

    cam.SetPosition(pos);
    cam.SetFOV(currentFOV);
    cam.LookAt(camLookPos);

    if (currentHeight > stopCamShakeHeight)
    {
      //cam shake
      cam.SetdefaultCamShake(0.2f, 2.0f, 0.95f, 0.95f);
      cam.StartCamShake();
    }
    else
    {
      //cam shake
      cam.SetdefaultCamShake(0.2f, 0.5f, 0.95f, 0.95f);
      cam.StartCamShake();
    }

    if (currentHeight < 0) return false;
  }

  return true;
}

bool LevelManager::WinLoseScreen(float dt)
{
  static const float speed = (transitionTime * 2.0f);
  Camera &cam = ASC_GMAN.GetCamera();

  // Logic
  if (m_showText)
  {
    const float barEndX = 16.0f;
    const float barEndY = 0.3f;

    auto uiText = UI_WinLose->GetHdl<UITextComp>();
    uiText->tint.a = min(uiText->tint.a + dt, 1.0f);

    auto uiBG = ASC_GETCOMP_PTR(GraphicsComp, UI_WinLose);
    uiBG->tint.a = min(uiBG->tint.a + dt, 1.0f);
    uiBG->scaleOffset.x += barEndX * dt;
    uiBG->scaleOffset.y += barEndY * dt;

    if (uiText->tint.a >= 1.0f) 
    {
      m_showText = false;
      waitUserInput = true;

      if (isLose)
      {
        auto uiTextBtn1 = UI_Btn1->GetHdl<UITextComp>();
        //auto uiTextBtn2 = UI_Btn2->GetHdl<UITextComp>();

        uiTextBtn1->SetText("Press Any Key To Continue");
        uiTextBtn1->offset.x = -1.6f;
        uiTextBtn1->offset.y = 1.f;
        
        //ASC_UITEXT.EnableInputCheck(true);
        //ASC_INPUT.DisplayCursor(true);
      }
      else
      {
        auto uiTextBtn1 = UI_Btn1->GetHdl<UITextComp>();

        uiTextBtn1->SetText("Press Any Key To Continue");
        uiTextBtn1->offset.x = -1.6f;
        uiTextBtn1->offset.y = 1.f;
      }
    }
  }
  else if (waitUserInput)
  {
    if (isLose)
    {
      auto uiTextBtn1 = UI_Btn1->GetHdl<UITextComp>();
      //auto uiTextBtn2 = UI_Btn2->GetHdl<UITextComp>();

      uiTextBtn1->tint.a = min(uiTextBtn1->tint.a + dt, 1.0f);
      //uiTextBtn2->tint.a = min(uiTextBtn2->tint.a + dt, 1.0f);

      if (ASC_ANYKEYPRESSED())
      {
        ASC_MSGSYS.SendMsg(Msg_Change_Scene(currentLevel));
        return true;
      }
    }
    else
    {
      if(!SendVictoryPauseOnce) 
      {
        SendMsg(mTYPE_GAME_PAUSE);
        SendVictoryPauseOnce = true;
        winSoundVol = ASC_SOUNDMAN.GetTrackVolume(goHdl->GetHdl<SoundComp>(), "SFX_Win_01.ogg");
        ASC_SOUNDMAN.SetTrackVolume(winSoundVol * 10.f, goHdl->GetHdl<SoundComp>(), "SFX_Win_01.ogg");
      }
      auto uiTextBtn1 = UI_Btn1->GetHdl<UITextComp>();
      uiTextBtn1->tint.a = min(uiTextBtn1->tint.a + dt, 1.0f);

      //auto uiStats1t = UI_Stats1->GetHdl<UITextComp>();
      //auto uiStats2t = UI_Stats2->GetHdl<UITextComp>();

      //auto uiStats1g = UI_Stats1->GetHdl<GraphicsComp>();
      //auto uiStats2g = UI_Stats2->GetHdl<GraphicsComp>();

      //uiStats1g->tint.a = min(uiStats1g->tint.a + dt, 1.0f);
      //uiStats2g->tint.a = min(uiStats2g->tint.a + dt, 1.0f);

      //uiStats1t->tint.a = min(uiStats1t->tint.a + dt, 1.0f);
      //uiStats2t->tint.a = min(uiStats2t->tint.a + dt, 1.0f);

      if (ASC_ANYKEYPRESSED()) waitUserInput = false;
    }
  }
  else
  {
    if (isGameOver)
    {
      countdown -= dt;

      if ((oCountdown - countdown) > 1.5f && !isLose)
      {
        currentFOV += dt * 40.0f;
        currentFOV = ASC_RANGE(currentFOV, camStartFOV, expandedFOV);
        cam.SetFOV(currentFOV);
      }

      if ((oCountdown - countdown) > (oCountdown * 0.75f))
      {
        auto uiBG = ASC_GETCOMP_PTR(GraphicsComp, UI_WinLose);
        uiBG->scaleOffset.y += speed * dt;

        TransformComp * bgTComp = ASC_GETCOMP_PTR(TransformComp, UI_WinLose);
        bgTComp->pos.z = 5.0f;
        bgTComp->dirty = true;
      }

      if (countdown < 0.0f)
        return false;

      if (!SendGameOverResumeOnce)
      {
        SendMsg(mTYPE_GAME_RESUME);
        ASC_SOUNDMAN.SetTrackVolume(winSoundVol, goHdl->GetHdl<SoundComp>(), "SFX_Win_01.ogg");
        SendGameOverResumeOnce = true;
      }
    }
  }

  return true;
}