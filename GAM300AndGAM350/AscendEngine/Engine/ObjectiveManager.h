/******************************************************************************/
/*!
\file   ObjectiveManager.h
\author Cai Wangyang
\par    email: wangyang.cai\@digipen.edu
\par    Digipen login: wangyang.cai
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#ifndef OBJ_MAN
#define OBJ_MAN

#define ASC_OM ObjectiveManager::Inst()

class ObjectiveManager : public SCRIPTING::ScriptCommonIF<ObjectiveManager>, public Singleton<ObjectiveManager>
{
private:
  //std::string scriptName = "_ObjectiveManager";
  std::unordered_map<GOTYPE, ObjTypeItr> objectiveMap;

protected:
  //Variables
  //Handle handle;
  //TransformComp * t1 = nullptr;z
  //PhysicsComp *   p1 = nullptr;
  //GraphicsComp *  g1 = nullptr;
  //SoundComp*      s1 = nullptr;

public:
  GOHandle player;
  GOHandle Fade;
  //Narrative Variables
  GOHandle NarrativeText;
  GOHandle dialogBG;
  std::string narrationText;
  
  std::vector<std::vector< std::string>> narrativeLines;

  std::vector<std::string >narrativeFiles;
  float time;
  bool showNarration;
  bool showHint;
  int numLines;
  int currNarrativeIndex =0;
  ///
  void GetObjects(const std::vector<GOTYPE>& vec);
  ObjTypeItr& GetObjectiveType(GOTYPE type);

  ObjectiveManager();
  ~ObjectiveManager();

  void OnStart() override;

  void Update(float dt) override;

  virtual void Serialize(Serializer &sr, LogicHandle idx) override;

  virtual void Deserialize(Deserializer &d) override;

  virtual void Reflect(const Reflector& r) override;

  virtual void OnDestroy() override;

  virtual void OnCollisionEnter(GOHandle h) override;

  virtual void OnCollisionStay(GOHandle h) override;

  virtual void OnCollisionExit(GOHandle h) override;

  void PlayFireBgm();

  void HandleMessageRegObjects(MsgData_Base* m);
  void HandleMessageMissonComplete(MsgData_Base* m);
  void RefreshNarrationFile();
  void StartNarrtive(int index) { showNarration = true; currNarrativeIndex = index; showHint = true; };
  enum LEVELS
  {
    BEDROOM,
    STUDYROOM,
    LIVINGROOM,
  };
  struct gameObjDone
  {
    bool currDone = false;
    GOHandle gameobj;
  };
  struct MissionData
  {
    int numDone = 0;
    bool done = false; 
    //possible to have more than 1 object as one mission 
    //therefore you need all object in this container to be done to considered it as done
   // std::unordered_map<Handle<GameObject>, bool> mapGameObjs;
    std::unordered_map<int, gameObjDone> mapGameObj;
   
   // std::vector<Handle<GameObject>> mapGameObjs;
   std::vector< GOHandle> gameObjectToUnlock;
  };
  typedef  std::unordered_map<int,MissionData> Data;
  std::unordered_map<LEVELS, Data> levelData;


  //note that if that mission has many other object link it wont auto complete
  bool SetAndCheckComplete(GOHandle, MissionData*);
  bool CheckIfAllMissionIsDone(GOHandle obj);

  void ChangeLevel(std::string);
  void IntroText();
  void RefreshHintFile();
  void IntroCutScene();
  void CheckInput();

  void NarrativeUpdate(float);
  void CutSceneUpdate();




 // bool CheckIfMissionComplete(GOHandle);
  bool  startFade  = false ;
  GOHandle dialogText;
  bool  showDialog = false;
  bool DialogFinished = false;
  bool playEndSound = false;
  bool startCutScene = false;
  bool FadeBlack(float );
  std::string finalLvlName = "";

  std::vector<std::string> hint;
  std::string path;
  int currCutSceneIndex = 0;
  std::vector<std::string> endingCutscenes;
  std::unordered_map<int,  std::string  > unlockThese;


  //std::vector<std::string> textures;
};

#endif 
