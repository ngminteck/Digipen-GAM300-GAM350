#pragma once

class FakePlacement : public SCRIPTING::ScriptCommonIF<FakePlacement>
{
private:
    bool usedMe;
    bool show;
    float time = 2.0f;
    float alpha = 0.35f;
    std::string correctObjectName, temp;
    GOHandle placeText;
    GOHandle dialogBG;
    
public:
    GOHandle correctObject;
    GOHandle fakeObject;
    FakePlacement();
    ~FakePlacement();
    void Initialize() {}
    void ShowText(std::string);
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);
    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;


};
