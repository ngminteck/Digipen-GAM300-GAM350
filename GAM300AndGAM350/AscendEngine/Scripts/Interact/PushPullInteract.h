#pragma once
class PushPullInteract : public Interact
{
private:
    std::string scriptName = "PushPullInteract";

public:
    void InteractAction() override;

    PushPullInteract();
    ~PushPullInteract() {}

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;

    GOHandle TargetInteract;
   


    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void SetupTarget(std::string name);
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;
    void Initialize() {}
    void HandleMessage(MsgData_Base* m);
    void RefreshHintFile() override;

};