/******************************************************************************/
/*!
\file   LevelManager.h
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
#pragma once

/*
  Add new class/type to :
  1) ScriptHelper.h     ScriptIndex
  2) ScriptCreator.h    SItype::Create
*/
class LevelManager : public SCRIPTING::ScriptCommonIF<LevelManager>
{
public:
  LevelManager();
  ~LevelManager();
  void AddLevelTransitionUIObj(GOHandle gohdl);

private:
  void OnStart() override;
  void OnDestroy() override;
  void Update(float dt) override;
  void Serialize(Serializer &sr, LogicHandle idx) override;
  void Deserialize(Deserializer &d) override;
  void Reflect(const Reflector& r) override;

  void HandleMessage(MsgData_Base *);

  bool mPaused = false;

  int startingScrap = 10;
  float gameTime = 1.0f;

  bool playedOnce = false;
  bool SendVictoryPauseOnce = false, SendGameOverResumeOnce = false;
  float winSoundVol = 0.f;

  std::string currentLevel;
  bool isGameOver, isLose;
  float transitionTime;
  float countdown, oCountdown;

  float heightOffset = 5000.0f, currentHeight, stopCamShakeHeight;
  float expandedFOV = 180.0f, currentFOV;
  float camStartFOV = 60.0f;
  Vec3 userLookPos;
  Vec3 camStartPos, camLookPos, camLookIncrement;

  GOHandle UI_WinLose;
  GOHandle UI_Stats1;
  GOHandle UI_Stats2;
  GOHandle UI_Btn1, UI_Btn2;

  GOHandle UI_WhiteOut;
  GOHandle UI_FadeOut;
  bool m_showText, isCutscene;
  bool waitUserInput;
  enum MsgType state;

  GOHandle beaconRing;

  float counter;
  bool Transition(float);
  bool WinLoseScreen(float);

  GOHandle player;
  Handle<TransformComp> player_t;
  Handle<GraphicsComp> player_g;

  //for generic fade in of the UI
  std::vector<GOHandle> UIobjs;
  float alpha = 0.f;
  void UpdateUIObjState(float dt);
};