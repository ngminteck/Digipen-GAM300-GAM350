#pragma once
class PuppyInteract : public Interact
{
private:
    std::string scriptName = "PuppyInteract";

public:
    void InteractAction() override;

    PuppyInteract();
    ~PuppyInteract() {}

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;
    GOHandle pup;
    GOHandle dialogBG;
    GOHandle dialogDog;
    GOHandle dialogDogname;
    GOHandle dialogText;
    GOHandle dialogMouse;
    GOHandle Fade;
    PlayerController * player;
    bool moving = false;
    float fadeTime;
    float barkInterval = 15.0f;
    bool fadeout;
    bool showDialog;
    bool DialogFinished;
    bool emitParticle; 
    int emitTimes = 2;
    int emitedAtm = 0;
    int times = 3;
    bool voiceStop = true;
    bool endDog = false;
    bool startInteract = false;
    int numLines;
    Vec3 positionToMove;
    Vec3 OrigPos;
    double time;
    double barkTime;
    std::string positionsFile;
    std::string dialogsFile;
    std::string textureFile;
    std::vector<std::pair<bool, std::string>> dialog;
    std::vector<std::string> positions;
    std::vector<std::string> dialogs;
    std::vector<std::string> textures;
    int currentIndex = 0;
    std::string dialogPath;
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void Initialize() {}
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void HandleMessage(MsgData_Base* m);
    void RefreshHintFile() override;
    void RefreshDialogFile();
    void RefreshPositionFile();
    void RefreshDialogList();
    void RefreshTextures();

    //GOHandle Number[5];
};

// Quest Number to reflect on screen
// static int puppyCollected = 1;