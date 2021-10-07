#pragma once

class Waypointer : public SCRIPTING::ScriptCommonIF<Waypointer>
{
public:
    Waypointer();
    ~Waypointer();
    std::vector<Vec3> waypoints;
    std::vector<Vec3> CameraLookats;
    bool scent = false;
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    bool useCam = false;
    Camera* cam;
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void  HandleKeyboard(float);
    void HandleController(float);

    void WritetoWaypointFile();
    void ReadfromWaypointFile();

    void AddWaypoint();



};