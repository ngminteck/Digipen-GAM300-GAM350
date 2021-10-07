#ifndef ROOMBA_CONTROLLER
#define ROOMBA_CONTROLLER

class RoombaController : public SCRIPTING::ScriptCommonIF<RoombaController>
{
private:
    std::string scriptName = "RoombaController";

    std::vector<Vec3> listOfWaypoint;
    std::vector<std::string> listOfWaypointTempName;
    int sizeOfWaypoint;

    float checkWaypointOffset;
    int waypointIndex;
    bool revertDirection;
    bool changeDirection = false;
    bool stop = false;
    
public:
    RoombaController();
    ~RoombaController();

    std::string targetObj;
    GOHandle obj;

    float offSety =0.0f;

    float speedRomba=10.0f;
    std::string StopRoomba;
    std::string GoRoomba;
    void SetWayPoints();
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;
    void SetRoombaActive(bool val);
    void HandleMessage(MsgData_Base* m);
};

#endif