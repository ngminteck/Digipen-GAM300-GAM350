#pragma once

class Hint : public SCRIPTING::ScriptCommonIF<Hint>
{
public:
	Hint();
	~Hint();

    //Variables
    Interact* targetScript;
    PlayerController* player;
    GOHandle hintText;

    GOHandle dialogBG;
    std::vector<std::string> lines;
    int numLines;
    double time;
    bool happy;
    bool sad;
    bool showHint;
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void  HandleKeyboard( float);
    void HandleController(float);



};