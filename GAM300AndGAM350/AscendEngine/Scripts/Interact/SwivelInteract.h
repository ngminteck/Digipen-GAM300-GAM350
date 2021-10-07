#pragma once
class SwivelInteract : public Interact
{
private:
    std::string scriptName = "SwivelInteract";

public:
    void InteractAction() override;

    SwivelInteract();
    ~SwivelInteract() {}

    GOHandle player;
    GOHandle target;
    std::string activateObject;
    Interact* activateScript;
    PlayerController* p = nullptr;
    Quaternion OrigRot;
    Quaternion ResultRot;
    Quaternion difference;
    float RotationMag;
    float time;
    bool Rotate;
    bool turnMe = false;
    Vec3 OrigPos;
    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void Initialize() override;
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;
    void RefreshHintFile() override;

    void HandleMessage(MsgData_Base* m);


    void GrabItem();


};