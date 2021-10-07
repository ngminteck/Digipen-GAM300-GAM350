/******************************************************************************/
/*!
\file   Engine.h
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
#ifndef AE_ENGINE_H
#define AE_ENGINE_H

#define FPSLIMIT(x) (1.0 / (x))

extern std::chrono::duration<float, std::milli> realtime;
const int auxSysNum = 1; //num of extra system w/o comp to be included in performace measurement
const std::vector<std::string> auxSysName{"TWEEN"};



//Handles all managers
#define ASC_ENGINE Engine::Inst()
class Engine : public Singleton<Engine>
{
public:
  
 
  enum STATES {
    LOADING,
    CHANGESCENE,
    PLAY,
    PAUSE,//part of pause
    STOP,
    RESTART,
    QUIT,
    PAUSECONFIRMATION,
    HTP };
  std::map<STATES, int> pauseState
  {
    {PAUSE,0},
    {HTP,1 },
    {PAUSECONFIRMATION,2}
  };

  enum HOVEREDSTATE
  {
      Resume,
      How,
      Quit,
      Yes,
      No,
      END


  };

  STATES currState;
  HOVEREDSTATE hState = HOVEREDSTATE::Resume;
  HOVEREDSTATE prevState = HOVEREDSTATE::Resume;
  Engine();
  ~Engine();

  void Update(bool &, float);

  std::string CurrentScene() { return sceneToLoad; }
  std::string sceneToLoad;
  void SetUsingMouse(bool val) { usingMouse = val; }
  bool GetUsingMouse()const { return usingMouse; }
private:
  void HandleMsg(MsgData_Base*);
  void ProcessState(bool& closeTriggered, float dt);


  //Tween tweening; //need to be destroyed last due to deregistering
  MTween mTween;
  TransformManager man_trans;

  GraphicsManager man_graph;
  LightManager man_light;
  PhysicsManager man_phy;
  LogicManager man_logic;
  SoundManager man_sound;
  WayPointManager man_waypoint;
  GridManager man_grid;
  UITextManager man_uitext;
  ParticleManager man_particle;
  TagManager man_tag;
  NewPhysicsManager man_newphy;
  AnimationManager man_animation;

  Sequencer sequencer;
  DialogBoxSystem dialogSystem;
  
  
  std::unordered_map<COMPONENT_TYPE, I_Manager *> managers;
  ObjectFactory factory;
  Editor editor;
  InputManager inputManager;

  float minRot = 0.0f;
  float maxRot = 25.0f;
  float rotSpeed = 50.0f;
  bool rotIncreasing = true;



  //pause shit 
  std::vector < std::vector<void (Engine::*)()> > functionStates;

  void MouseUpdate(float);
  void KeyUpdate(float);
  void GamePadUpdate(float);

  void PauseState(float);
  void PauseConfrimation(float);
  void HowToPlay(float);

  void UpdateSelection(float dt );
  void UpdateSelection_PauseState(float dt);
  void UpdateSelection_ExitState(float dt);

  void PauseState_Resume();
  void PauseState_HTP();
  void PauseState_Quit();
  
  void HowToPlayState_Back();
  void Quit_Yes();
  void Quit_No();

  void CheckIfPauseIsPress();
  void Blank() {};

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

  bool usingMouse = true;
  int index = 0;
};





#endif