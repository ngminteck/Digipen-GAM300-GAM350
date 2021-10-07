#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

typedef ComponentManagerBase<AnimationComp>::COMPHANDLETYPE AnimationHandle;

#define ASC_AMAN AnimationManager::Inst()

class AnimationManager : public ComponentManagerBase<AnimationComp>, public Singleton<AnimationManager>
{
public:
    explicit AnimationManager(HandleManager<TransformComp>& transform_comps);

    ~AnimationManager();

    void Load() override;
    //void UnLoad() override;
    void Update(float dt) override;
    BaseHandle Deserialize(const GameObject& gameObject, Deserializer&) override;
    void Serialize(Serializer&, BaseHandle) override;
    void Reflect(BaseHandle, const Reflector&) override;
    void InitComponent(const GameObject& go, BaseHandle hdl) override;
    void Reset();
    void Running();
    BaseHandle CreateComponent() override;
    //this function every manager needs to implement as it is a deep copy
    void CopyComponentToComponent(AnimationComp& target, const AnimationComp& source) override;
    void HandleMsg(MsgData_Base*) override;
    void ModelAnimation(AnimationComp& aComp);
    void TextureAnimation(AnimationComp& aComp);
    void ChangeModel(std::string nameModel, std::string name, int num);

    std::map<std::string, std::vector<std::string>> listOfModel;
    std::map<std::string, int> numOfModel;
private:
    std::string name;
    bool isLoop;
    std::map<std::string, std::vector<std::string>> listOfAnimation;
    std::map<std::string, int> numOftexture;
    HandleManager<TransformComp>& m_transform;
    float currFrame = 0.0f;
    float speed = 0.016f;
    float maxFrame = 0.0f;
    int currIndex = 0;
    std::vector<std::string> animationTypeList;
};
#endif
