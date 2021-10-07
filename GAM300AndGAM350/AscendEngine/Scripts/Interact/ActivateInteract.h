#pragma once
class ActivateInteract : public Interact
{
private:
    std::string scriptName = "ActivateInteract";

public:

    //for turn off light 
  std::vector<std::string>objParticleToTurnOn;
  std::vector<GOHandle>objs;

  std::vector<std::string> nameOfObjs;
  std::vector<GOHandle>ObjsToControl;


    //only used for this script 
    bool isOn = false;
    //only for Tap
    bool pluged = false;
    bool linkToOtherMission = false;
    void InteractAction() override;

    void ForceRegStuff() override;

    ActivateInteract();
    ~ActivateInteract() {}

    void Initialize() {}

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
    void RefreshHintFile() override;

    bool isTurnOn = false;

    std::string targetObj = "";
    int type = 0;
    float timer = 0.0f;
};