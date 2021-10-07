#pragma once


class CameraTrigger : public SCRIPTING::ScriptCommonIF<CameraTrigger>
{
private:
    int index = 0;
    int editorVal = 0;
    int viewVal = 0;
    std::string inputName;
    float tt;
    float wt;
    int actionindex = 0;
    bool run = false;
    bool useCurve = true;
    float runTime, waitTime, ratio, waitRatio;
    int wayPointerIndex = 0, nodePointerIndex = 0;
    float timeForEachWaypoint = 0.0f;
    float timeForEachNode = 0.0f;
    Vec3 intPos, intRot;
    bool waiting = false;
    GOHandle fade;
    bool DoTheFade = false;
    float fadeTime = 0.0f;
    Vec3 pos;
    bool toggle = false;
    bool activate = true;
public:
    CameraTrigger();
    ~CameraTrigger();

    enum CameraMovements
    {
        TELEPORT,
        MOVETO,
        ROTATE,
        WAYPOINT,
        DONOTGOPASTTHISPOINT

    };

    enum CameraViews
    {
        LOOKAT,
        GETROTATION
    };

    struct Actions
    {
        Actions(int c = 0, int v= 0, float t= 0.0f, const Vec3& pos = Vec3::ZERO, const Vec3& rot = Vec3::ZERO) : camAct(static_cast<CameraMovements>(c)), viewType(static_cast<CameraViews>(v)),posToMove(pos),posToRot(rot), _time(t)
        {
        }
        CameraMovements camAct;
        CameraViews viewType;
        Waypointer* _w = nullptr;
        bool _curve = false;
        GOHandle focusObj;
        Vec3 posToMove;
        Vec3 posToRot;
        float _time;
        float _waitTime = 0.0f;

    };
    Curve* c;
    std::vector<Actions> actionList;
    std::string NameOfEndingObj;
    GOHandle EndingObj;
    Vec3 CamLERP(Vec3 a, Vec3 b, float t);
    void Initialize() override;
    void FormCurve();
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
    void AddCameraAction(int c = 0, int v = 0, float t = 0, bool wayPoint = false);
    void WriteToCameraFile();
    void ReadFromCameraFile();
    void  HandleKeyboard(float);
    void HandleController(float);
    void StartMovements();
    bool DetectScript(int ignore = -1);
    void UpdateVariables();

};
