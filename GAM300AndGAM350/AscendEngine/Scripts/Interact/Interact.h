#ifndef INTERACT
#define INTERACT

class Interact : public SCRIPTING::ScriptCommonIF<Interact>, public  SCRIPTING::ObjInfor
{
public:
    enum Prompts 
    {
        NONE,
        LEFTCLICK,
        INTRACT,
        MOVE,
        PICKUP,
        SWIVEL,
        RIGHTCLICK,
        ORIGIN,
        TRAILS,
        ALL
    };
    Interact();
    ~Interact() {}
    virtual void InteractAction() { std::cout << "Interact??" << std::endl; };
     void ForceRegStuff() override;

    void Reflect(const Reflector& r)override;
    void Serialize(Serializer& sr, LogicHandle idx)override;
    void Deserialize(Deserializer& d)override;

    /*int missionNum = 0;
    ObjectiveManager::LEVELS belongToWhichLevel = ObjectiveManager::LEVELS::BEDROOM;*/
    bool linkToOtherMission = false;
    std::vector<std::string> hint;
    std::string path;
    GOHandle clue, dest;
    virtual void UseParticles(float, bool) { std::cout << "What particles?" << std::endl; }
    bool activated = true;
    float range = 0.0f;
    bool usedMe = false;
    bool myParticle = false;
    GOHandle particleUser;
    int prompt;
    std::string InteractSoundOn;
    std::string InteractSoundOff;
    std::string InteractTriggerSound;
    virtual int Controls() { return 0; }
    virtual void RefreshHintFile() {}
    void Initialize() {}

};


#endif

