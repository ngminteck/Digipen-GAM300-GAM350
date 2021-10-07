#pragma once


class PuzzleKeyHole : public SCRIPTING::ScriptCommonIF<PuzzleKeyHole>
{
private:
    std::string parPuzName;
    std::string ExpectedKeyName;
public:
    PuzzleKeyHole();
    ~PuzzleKeyHole();

    GOHandle currentInsertedKey;
    GOHandle ExpectedKey;
    GOHandle parentPuzzle;
    Vec3 Keypos;
    Vec3 KeyRot;

    void Init();
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void GetKeyLocation();
    void GetKeyRotation();




};
