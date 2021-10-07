#pragma once

#ifndef DIABOX_H
#define DIABOX_H

//typedef Handle TransHandle
//typedef ComponentManagerBase<TagComp>::COMPHANDLETYPE TagHandle;

#define ASC_DIALOGBOX DialogBoxSystem::Inst()

class DialogBoxSystem 
{
public:
	DialogBoxSystem();
	~DialogBoxSystem();

    void Load() ;
    void UnLoad() ;

    void Update(float dt) ;

    BaseHandle Deserialize(const GameObject& gameObject, Deserializer&) ;
    void Serialize(Serializer&, BaseHandle) ;
    void Reflect(BaseHandle, const Reflector&) ;
   // BaseHandle CloneComponent(BaseHandle) override;

    BaseHandle CreateComponent() ;
    void DestroyComponent(BaseHandle hdl) ;
    void InitComponent(const GameObject& go, BaseHandle hdl) ;

    //For Msging
    void HandleMsg(MsgData_Base*) ;

   

};


#endif