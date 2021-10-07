#pragma once

enum Interacts
{
    MoveI,
    PuppyI,
    PickI,
    ItemI,
    DoorI,
    ActivateI,
    DoNotPassThis
} ;

class PuzzleKey : public SCRIPTING::ScriptCommonIF<PuzzleKey>
{
public:
    PuzzleKey();
    ~PuzzleKey();
    GOHandle ParentPuzzleObject;
    int use = -1;
    std::vector<Interact*> in;
    int myIndex = -1;
    Vec3 localPos;
    bool inKeyHole = false;
    int orderInput = -1;
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void  HandleKeyboard(float);
    void HandleController(float);

    void Initialize();
    bool DetectScript(int ignore = -1);
    void UpdateVariables();

    float var;
    bool check, check2;
    bool oneTime = false;

};
