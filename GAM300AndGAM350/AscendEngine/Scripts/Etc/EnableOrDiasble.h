#ifndef ENABLE_DISABLE
#define ENABLE_DISABLE

class EnableOrDisable : public SCRIPTING::ScriptCommonIF<EnableOrDisable>
{
private:
    std::string scriptName = "EnableOrDisable";

    std::vector<GOHandle> listOfWaypoint;
    std::vector<std::string> listOfWaypointTempName;
    int sizeOfWaypoint;

    float checkWaypointOffset;
    int waypointIndex;
    bool revertDirection;

    
public:
    EnableOrDisable();
    ~EnableOrDisable();

    int type = 0;
    std::string stringVal = "";

    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void SwitchTypeBehaviour();

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void HandleMessage(MsgData_Base* m);

    bool willActivate = false; 
    bool willDeActivate = false;
};

#endif