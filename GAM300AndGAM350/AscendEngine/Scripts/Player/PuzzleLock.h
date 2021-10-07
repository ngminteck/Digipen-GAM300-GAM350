#pragma once
struct Key
{
    GOHandle obj;
    bool inplace = false;
    int order = 0;
    Vec3 posToBe = Vec3::ZERO;
    Vec3 rotToBe = Vec3::ZERO;

};
class PuzzleLock : public SCRIPTING::ScriptCommonIF<PuzzleLock>
{
public:
    PuzzleLock();
    ~PuzzleLock();
    std::vector<Key> Objects;
    std::vector<bool> Combination;
    std::vector<int> Order;
    std::vector<bool> KeyInput;
    std::string PuzzleOrderWrong;
    std::string PuzzleOrderCorrect;
    std::string UnlockSound;
    int orderToBeUsed = 0;
    int index = 0;
    bool unlocked = false;
    bool useInplace = false;
    bool useOrdering = false;
    double wrongTime = -100.0f;
    int maxVal;

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void Initialize() override;
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void  HandleKeyboard(float);
    void HandleController(float);

    void WritetoKeysFile();
    void ReadfromKeysFile();

 
    void ResetKeys();
    bool CheckAllReady();
    bool CheckCombination();
    bool CheckOrder();
    bool InputKey(const GOHandle& g);
    bool InputKey(int index, bool b);
    int GetKeyIndex(const GOHandle& g);
    void AssignAllObjects();
};