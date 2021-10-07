#pragma once
class PickupInteract : public Interact
{
private:
    std::string scriptName = "PickupInteract";

public:
    void InteractAction() override;

    PickupInteract();
    ~PickupInteract() {}
    // default behaviour is once an item is pickup it will not be climable anymore
    //Note  enabling this to true at run time may not really do help you make it climbable 
    // check InteractAction() for where its disable
    bool climbableAfterPickup = false;
    bool moveCam;
    double time;
    Vec3 origRot;
    GOHandle player;
    Vec3 DropPos;
    Vec3 placeLoc = Vec3::ZERO;
    Vec3 GrabRot = Vec3::ZERO, placeRot = Vec3::ZERO;
    PlayerController* py;
    Vec3 s, d;
    std::string posFile;
    GOHandle clue, dest;
    bool useParticle;
    bool inTrigger = false;
    GOHandle otherObj;

    //for plug 
    std::string nameofSinkBody;
    GOHandle targetObject;
    void ReadyTheSink();
    bool CheckTheSink();

    //Vec3 GetPlayerCamPos() const;
    //int GetCamHandle() const;
    void WriteToPosFile(bool);
    void ReadFromPosFile();
    void Initialize() {}
    void UseParticles(float, bool) override;
    void UpdateDest(float);
    void UpdateClue(float);
    void OnStart() override;
    void Update(float dt) override;
    void OnDestroy() override;
    void AssignSmells();
    void Serialize(Serializer& sr, LogicHandle idx) override;
    void Deserialize(Deserializer& d) override;
    void Reflect(const Reflector& r) override;

    void OnCollisionEnter(GOHandle other) override;
    void OnCollisionStay(GOHandle other) override;
    void OnCollisionExit(GOHandle other) override;

    void HandleMessage(MsgData_Base* m);
    void RefreshHintFile() override;
    void PlaceItem(const Vec3& pos, const Vec3& rot);
    void GrabItem();

    // only for water , im lazy lol
    bool isItforCollectingWater = false;
    bool hasWater = false;
    void CollectWater();
    void DumbWater();


};
