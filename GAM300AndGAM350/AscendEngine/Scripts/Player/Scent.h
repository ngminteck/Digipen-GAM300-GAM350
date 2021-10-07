#pragma once

class Scent : public SCRIPTING::ScriptCommonIF<Scent>
{
public:
    Scent();
    ~Scent();

    Waypointer* w;
    Vec3 closestWaypoint;
    int index;
    int lastwp;
    bool TrailInit;
    bool focused;
    std::vector<GOHandle> Trail;
    GOHandle player;
    std::string objective;
    float distFromObjective;
    std::vector<float> distDiff;
    Curve* c;


    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);

    void  HandleKeyboard(float);
    void HandleController(float);

    void WritetoWaypointFile();
    void ReadfromWaypointFile();
    void SetTrailParticles(float);
    void ResetTrail();
    void FormTrail();
    void InitTrail();




};
