#ifndef EVENT_TRIGGER
#define EVENT_TRIGGER

class EventTrigger  : public BaseTrigger

{
public:

  enum TRIGGER_TYPE
  {
    MISSION_END_SPOT

  };

  int missionType = 0;//0 means just noting ,will just do default behaviour 
  EventTrigger();
    ~EventTrigger() {}

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

  


    void HandleMsgObjectiveDone(MsgData_Base* m);

    void HandleMessage(MsgData_Base* m);
    void ForceRegStuff() override;
    std::string targetedObj = "";
    bool linkToOtherMission = false; 
    bool happenOnceOnly = false;
    int narrativeIndex=0;
    //Vec3 
private:
  
  GOHandle objectiveManager;
  GOHandle gameobjectToUse;
  void OffALLLightsEvent();

  std::string scriptName = "EventTrigger";
  std::vector< std::string> missionNames
  {
    "NONE",
    "OFF_ALL_LIGHT",
    "ON_ALL_LIGHT",
    "NARRATIVE",
    "WATER",
    "PROMPT",
    "START_STOVE"
  };
  int index1 = 0;


  void LightOnBehaviour(GOHandle );
  void LightOffBehaviour(GOHandle);
  void NarrativeTriggerBehaviour(GOHandle);
  void SinkTrigger(GOHandle);
  void PromptTrigger(GOHandle);
  void StartFireTrigger(GOHandle);
};


#endif

