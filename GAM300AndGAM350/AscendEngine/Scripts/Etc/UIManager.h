#pragma once
class UIManager : public   SCRIPTING::ScriptCommonIF<UIManager>
{
private:
    std::string scriptName = "UIManager";


public:
  GOHandle objToExcute;
  GOHandle promptBox;
  GOHandle missionListUI ;
  GOHandle strikeOut;
  //testing
  GOHandle text;
  int currMission = 0;

  //for end current misison 
  std::vector<std::string>missNumToTexture;
  //for start next mission
  std::vector<std::string>missNumToTextureNextMission;
  //for the mission list 
  std::vector<std::string>missNumToTextureForMissionList;

   // void ForceRegStuff() override;

    UIManager();
    ~UIManager() {}


    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void HandleMessage(MsgData_Base* m);


    void ExcutingActions(GOHandle,float dt);
    void Start_QuestCompletePrompt();
    void Start_QuestInitalPrompt();
    void Start_OpenMissionList();
    void Start_CloseMissionList();

    bool DoesNothing(GOHandle a, float b) { return true; };
    bool ShiftDown (GOHandle, float);
    bool ShiftUp   (GOHandle, float);
    bool FadeOutObj(GOHandle, float );
    bool ChangeTexture(GOHandle,std::string  );

    bool ChangeTextureMissionList(GOHandle, float);
    bool ChangeTextureCompleteMission(GOHandle, float);
    
    bool ChangeTextureNextMission(GOHandle, float);

    bool Wait(float);

    bool startAction = false;
    
    std::queue< bool (UIManager::*)(GOHandle,float)> actions;

    bool isTurnOn = false;

    std::string targetObj = "";
    int type = 0;
};