#pragma once
#pragma once


class MoveInteract : public Interact
{
private:
    std::string scriptName = "MoveInteract";

public:
    void InteractAction() override;

    MoveInteract();
    ~MoveInteract() {}

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;

    GOHandle TargetInteract;

    bool Pushable = false;
    bool Pullable = false;
    bool lockedPositions = false;
    int myIndex, side = 3;
    GOHandle player;
    std::vector<Vec3> winds;
    PlayerController* p = nullptr;
    float Distance =0.0f, CurrDistance =0.0f;
    bool Forward = false;
    bool StaticObject = false;
    Vec3 OriginalPos = Vec3::ZERO;
    void SetPlayerPos();
    void ResetWinds();
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