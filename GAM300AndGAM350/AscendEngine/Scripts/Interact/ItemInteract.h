#pragma once
class ItemInteract : public Interact
{
private:
    std::string scriptName = "ItemInteract";

public:
    void InteractAction() override;

    ItemInteract();
    ~ItemInteract(){}

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void RefreshHintFile() override;

    void HandleMessage(MsgData_Base* m);

    void Initialize() {}
    void GrabItem();


};