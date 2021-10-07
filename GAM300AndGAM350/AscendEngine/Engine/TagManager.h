#pragma once

#ifndef TAG_MAN_H
#define TAG_MAN_H
// is use to hold input Temp data
struct InputTextData
{

    char buffer[100] = { 0 };
    //not meant to be use 
    char empty[1] = { 0 };
};


//typedef Handle TransHandle;
typedef ComponentManagerBase<TagComp>::COMPHANDLETYPE TagHandle;

#define ASC_TAGMANAGER TagManager::Inst()

class TagManager : public ComponentManagerBase<TagComp>
{
public:
	TagManager();
	~TagManager();

    void Load() override;
    void UnLoad() override;

    void Update(float dt) override;

    BaseHandle Deserialize(const GameObject& gameObject, Deserializer&) override;
    void Serialize(Serializer&, BaseHandle) override;
    void Reflect(BaseHandle, const Reflector&) override;
   // BaseHandle CloneComponent(BaseHandle) override;

    BaseHandle CreateComponent() override;
    void DestroyComponent(BaseHandle hdl) override;
    void InitComponent(const GameObject& go, BaseHandle hdl) override;

    //function from CompManBase
    void CopyComponentToComponent(TagComp& target, const TagComp& source) override;

    //For Msging
    void HandleMsg(MsgData_Base*) override;

    InputTextData inputText;

};


#endif