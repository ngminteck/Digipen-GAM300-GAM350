#ifndef WATER_CONTAINER
#define WATER_CONTAINER

class WaterContainer : public SCRIPTING::ScriptCommonIF<WaterContainer>
{
private:
 

    int missionType = 0;
    
public:
  void ToogleWaterContainerBehaviour();
  WaterContainer();
    ~WaterContainer();

    std::string targetObj;
    GOHandle obj;
    std::string scriptName = "EventTrigger";
    std::vector< std::string> missionNames
    {
      "NONE",
      "SINK",
      "WATER_BUCKET",
    };
    int index1 = 0;

    void CollectWater();
    void DumbWater();
    bool hasWater = false;
    bool activate = false;
    bool isOn = false;

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

    //how much to water to fill per frame 
    float fillRate = 0.1f;
    float maxScale = 1.25f;
    float timer = 0.0f;
    //static std::unordered_map<std::string, int> soundName;
};

#endif