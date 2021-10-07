
class DoorInteract : public Interact
{
private:
    std::string scriptName = "DoorInteract";

public:
    void InteractAction() override;

    DoorInteract();
    ~DoorInteract() {}

    std::string textToDisplayForLock = "I cant leave yet";
    bool textFade = false;
    float textFadeTime = 0.0f;
    GOHandle textObj;
    GOHandle dialogBG;

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;
    bool lock = false;
    GOHandle Fade;
    bool moving = false;
    double fadeTime;
    bool fadeout;
    //At position 1 or not
    bool PositionOne = true;
    //Spawn positions
    std::string pos1;
    std::string pos2;
    GOHandle position1;
    GOHandle position2;


    void TextUpdate(float);
    void Initialize() {}
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void SetupSpawnPoints();

    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    int Controls() override;
        
    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void HandleMessage(MsgData_Base* m);
    void RefreshHintFile() override;

    void GrabItem();
};