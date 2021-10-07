#pragma once

class Director : public SCRIPTING::ScriptCommonIF<Director>
{
public:
    Director();
    ~Director();


    enum Manipulations
    {
        Translate,
        Rotate,
        Scale

    };
    struct DirectorActions
    {
        DirectorActions(int _m, const Vec3& v, float _time ) :
            m(static_cast<Manipulations>(_m)), Val(v),time(_time)
        {}
        Manipulations m = Translate;
        Vec3 Val = Vec3::ZERO;
        float time = 0.0f;


    };

    bool direct = false;
    int index = 0;
    bool origVis = false;
    bool enableCollisionAfter =false;
    std::vector<DirectorActions> actionList;
    Vec3 PositionToStart, intPos;
    Quaternion OrigRot, intQuat ;
    Vec3 OrigScale, intScale;
    float time =0.0f,ratio = 0.0f;
    bool Pause= false;
    int editorVal = 0;
    float editorTime = 0.0f;

    //Variables
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;
    void HandleMessage(MsgData_Base* m);
    void WriteToDirectorFile();
    void ReadFromDirectorFile();
    void  HandleKeyboard(float);
    void HandleController(float);

    


};