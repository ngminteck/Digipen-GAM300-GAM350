/******************************************************************************/
/*!
\file   MainMenuScript.h
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

#define MAX_RANGE 9.0f
class MainMenuScript : public SCRIPTING::ScriptCommonIF<MainMenuScript>
{
public:
  MainMenuScript();
  ~MainMenuScript();

private:
  void ShowQuitDialog();

  enum class STATE
  {
      Main,
      Options,
      HowToPlay,
      Credits,
      Quit,
      END
  };

  enum class HOVERSTATE
  {
      Main,
      Options,
      HowToPlay,
      Credits,
      Quit,
      Yes,
      No,
     END


  };

  std::vector < std::vector<void (MainMenuScript::*)()> > functionStates;

  STATE CurrentState = STATE::Main;
  HOVERSTATE HoveredState = HOVERSTATE::Main;
  HOVERSTATE previousState = HOVERSTATE::Main;
  int index = 0; 
  bool buttonHovered = false;

  void OnStart()        override;
  void Update(float dt) override;
  void OnDestroy()      override;
  
  void Serialize(Serializer &sr, LogicHandle idx) override;
  void Deserialize(Deserializer &d) override;
  void Reflect(const Reflector& r) override;
  
  void HandleMessage(MsgData_Base *m);

  //Slider stuff
  float slider_width = 0.0674f;//0.0575f;
  float slider_height = 0.165256f; //0.06f;

  float slider_x_min = -0.04f;//0.03f;
  float slider_x_max = 0.41f;//-0.325f;
  float slider_length = 0.4481f;

  GOHandle VolumeSlider;
  bool VolumeSliderSelected = false;
  GOHandle BrightnessSlider;
  bool BrightnessSliderSelected = false;
  GOHandle PawSelector;
  float minRot = 0.0f;
  float maxRot = 25.0f;
  float rotSpeed = 50.0f;
  bool rotIncreasing = true;

  GOHandle Fade;
  bool fadein = true;
  bool fadeout = false;
  float fadeTime = 1.5f;

  GOHandle StartButton;
  GOHandle OptionsButton;
  GOHandle HowToPlayButton;
  GOHandle CreditsButton;
  GOHandle ExitButton;
  bool usingMouse;
  void CheckInput(float);

  bool Up();
  bool Down();
  bool Left();
  bool Right();
  bool Accept();

  bool UpMouseKey();
  bool DownMouseKey();
  bool LeftMouseKey();
  bool RightMouseKey();
  bool AcceptMouseKey();

  void UpdateSelection(float);
  void Main_UpdateSelection(float);
  void Options_UpdateSelection(float);
  void Exit_UpdateSelection(float);

  void MouseUpdate(float dt);
  void KeyUpdate(float dt);
  void PadUpdate(float dt);

  void Main_Start();
  void Main_Options();
  void Main_HowToPlay();
  void Main_Credits();
  void Main_Exit();

  void Options_Return();
 
  float UpdateSlider(GOHandle);


  void HowToPlay_ReturnToMain();
  void Credits_ReturnToMain();

  void Quit_ExitGame();
  void Quit_ReturnToMain();

  //does nothing but we need this to add space 
  void Blank() {};
};