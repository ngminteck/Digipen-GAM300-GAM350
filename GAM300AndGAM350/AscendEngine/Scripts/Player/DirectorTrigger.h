#pragma once
class DirectorTrigger : public SCRIPTING::ScriptCommonIF<DirectorTrigger>
{
public:
    bool runOneAction = false;
    std::string DirectObject;
    GOHandle DirectedObject;
    std::vector<std::string> DirectObjects;
    std::vector<GOHandle> DirectedObjects;
    bool Multiple = false;
    DirectorTrigger();
    ~DirectorTrigger();
    void Initialize() override;
    void FindDirectedObject();
    void StartDirector();
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    bool Fading(const Vec3& pos, float dt);
    bool Fading(bool&, float);
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);
    void OnCollisionEnter(GOHandle other) override;
};