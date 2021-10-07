/******************************************************************************/
/*!
\file   PauseScript.h
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

#ifndef PAUSESCRIPT_H
#define PAUSESCRIPT_H
/*
Add new class/type to :
1) ScriptHelper.h     ScriptIndex
2) ScriptCreator.h    SItype::Create
*/
class PauseScript : public SCRIPTING::ScriptCommonIF<PauseScript>
{
public:
  PauseScript();
  ~PauseScript();

  void EnableMenu(bool, float);
  void EnableSubMenu(bool, float);
  void CheckInput();

private:
  void OnStart() override;
  void Update(float dt) override;
  void OnDestroy() override;

  void Serialize(Serializer &sr, LogicHandle idx) override;
  void Deserialize(Deserializer &d) override;
  void Reflect(const Reflector& r) override;

  void ShowQuitDialog();
  void ShowRestartGameDialog();

  bool isGameOver;

  std::string currentLevel;

  void HandleMessage(MsgData_Base *m);

  GOHandle pauseOverlay;
  GOHandle pauseLCr;
  GOHandle h2pOverlay;
  GOHandle bgTint;

  GOHandle restartPanel;
  GOHandle restartYes;
  GOHandle restartNo;

  GOHandle quitPanel;
  GOHandle quitYes;
  GOHandle quitNo;

  GOHandle quitArrow;

  enum OptionState
  {
    fullscreen,
    bgm,
    volume
  };
  OptionState optionButtonState;
  GOHandle optionFullScreenText, optionVolumeText, optionBgmText;
  GOHandle optionBG, optionArrow;

  GOHandle creditsScreen;

  int currentSel;
  float oldVolume;
  std::vector<GOHandle> btns;

  enum PAUSESTATE { FADEOUT, FADEIN, INACTIVE, ACTIVE, FADEOUT_SUB, FADEIN_SUB, SUBMENU, OPTIONS, CREDITS, RESTART, QUIT };
  PAUSESTATE currState;
};

#endif