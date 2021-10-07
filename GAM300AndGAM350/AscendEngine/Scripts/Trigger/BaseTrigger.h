#ifndef BASE_TEXT
#define BASE_TEXT

class BaseTrigger  : public SCRIPTING::ScriptCommonIF<BaseTrigger> ,  public  SCRIPTING::ObjInfor
{
public:

  enum TRIGGER_TYPE
  {
    MISSION_END_SPOT

  };

  int missionType = 0;//0 means just noting ,will just do default behaviour 
  BaseTrigger();
    ~BaseTrigger() {}

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void ObjectPlaceCorrectly();
    void RoombaEnd();

    void ActivateFire();

    void HandleMsgObjectiveDone(MsgData_Base* m);

    void HandleMessage(MsgData_Base* m);
    void ForceRegStuff() override;
    std::string targetedObj = "";
    bool linkToOtherMission = false; 
    //Vec3 
private:
  int index1 = 0;
  std::vector< const char* > triggerMissionNames
  {
    "NONE",
    "ROOMBA",
    "ORDERPUZZLE",
    "Sink",
    "OVEN_Fire",

  };
  std::string scriptName = "BaseTrigger";

};


#endif

