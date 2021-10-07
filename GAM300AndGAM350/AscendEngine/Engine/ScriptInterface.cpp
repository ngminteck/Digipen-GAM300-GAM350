/******************************************************************************/
/*!
\file   ScriptInterface.cpp
\author Goh Zihi
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for Serializer & deserializer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

std::unordered_map<ScriptIndex, std::shared_ptr<SCRIPTING::ScriptInterface>> SItype::m_creator;

int SCRIPTING::ScriptInterface::scriptCount = 0;

SCRIPTING::ScriptInterface::ScriptInterface() : inUse(true), obj(nullptr), goHdl(), runTime(0), m_active(true), m_lastActive(true), m_persist(false)
{
    //colliding_list.clear();
}

SCRIPTING::ScriptInterface::ScriptInterface(const std::string& name)
    : inUse(true), name(name), script_type(SItype::StrToType[name]), obj(nullptr), goHdl(), runTime(0), m_active(true), m_lastActive(true), m_persist(false)
{
    //colliding_list.clear();
}

SCRIPTING::ScriptInterface::~ScriptInterface()
{
   // colliding_list.clear();
    --scriptCount;
    ASC_MSGSYS.Remove_Observer(msgList);
}

void SCRIPTING::ScriptInterface::ScriptStart()
{
    //Update the comp handles
    obj = ASC_GETGO_PTR(goHdl);
    assert(obj != nullptr);

#if 0
    hdl_t = ASC_GETCOMP_HANDLE(COMPONENT_TYPE::TRANSFORM, goHdl);
    //hdl_p = ASC_GETCOMP_HANDLE(COMPONENT_TYPE::PHYSICS, goHdl);
    //hdl_g = ASC_GETCOMP_HANDLE(COMPONENT_TYPE::GRAPHICS, goHdl);
    //hdl_s = ASC_GETCOMP_HANDLE(COMPONENT_TYPE::SOUND, goHdl);

    assert(hdl_t.IsValid());
    //assert(hdl_p.IsValid());
    //assert(hdl_g.IsValid());
    //assert(hdl_s.IsValid());
#else
    for (int i = 0; i < COMPONENT_TYPE::MAX; ++i)
    {
        COMPONENT_TYPE type = COMPONENT_TYPE(i);
        BaseHandle h = ASC_GETCOMP_HANDLE(type, goHdl);
        if (h.IsValid())
            hdl_arr[i] = h;
#if !_SUBMISSION
        else
        {
            //LogWarning(obj->name, " : handle type ", compStrMap[type], "Invalid");
        }
#endif
    }
#endif
    ScriptInterface::ScriptUpdate(0);
}

void SCRIPTING::ScriptInterface::ScriptUpdateHandle()
{
  
    t = goHdl->GetHdl<TransformComp>();
    p = goHdl->GetHdl<PhysicsComp>();
    g = goHdl->GetHdl<GraphicsComp>();
    s = goHdl->GetHdl<SoundComp>();
    l = goHdl->GetHdl<LightComp>();
    u = goHdl->GetHdl<UITextComp>();
    pe = goHdl->GetHdl<ParticleEmitter>();
    tc = goHdl->GetHdl<TagComp>();
    np = goHdl->GetHdl<NewPhysicsComp>();
}

void SCRIPTING::ScriptInterface::ScriptUpdate(float)
{
    //for (int i = 0; i < COMPONENT_TYPE::MAX; ++i)
    //{
    //  COMPONENT_TYPE type = COMPONENT_TYPE(i);
    //  if (hdl_arr[type].IsValid())

    //    //ptr_arr[type] = ASC_GETCOMP_PTR_BYTYPE(type, goHdl);
    //}
    ScriptUpdateHandle();
}

void SCRIPTING::ScriptInterface::Reflect(const Reflector&)
{
}

void SCRIPTING::ScriptInterface::ScriptDestroy()
{
}

void SCRIPTING::ScriptInterface::OnCollisionEnter(GOHandle h)
{
  

}

void SCRIPTING::ScriptInterface::OnCollisionStay(GOHandle h)
{
  
}

void SCRIPTING::ScriptInterface::OnCollisionExit(GOHandle h)
{
  
}

void SCRIPTING::ScriptInterface::DestroySelf() const
{
    ASC_FACTORY.DestroyObject(goHdl);
}

void SCRIPTING::ScriptInterface::InitHandle(GOHandle idx)
{
    goHdl = idx;
    //tweenRef = TweenInfoRef(goHdl);
}

ScriptIndex SCRIPTING::ScriptInterface::GetScriptType() const
{
    return script_type;
}

const std::string& SCRIPTING::ScriptInterface::GetName() const
{
    return name;
}

GameObject& SCRIPTING::ScriptInterface::GetGameObjectFromHandle(GOHandle objHandle)
{
    GameObject* obj = ASC_FACTORY.GetGameObjectPtr(objHandle);
    assert(obj);
    return *obj;
}

//void SCRIPTING::ScriptInterface::RegisterMsg(MsgType msgType, const std::function<void(MsgData_Base*)> &fn)
//{
//  msgList.push_back(ASC_MSGSYS.RegisterCallBack(msgType, fn));
//}

void SCRIPTING::ScriptInterface::SendMsg(MsgData_Base&& msg)
{
    ASC_MSGSYS.SendMsg(std::move(msg));
}

GOHandle SCRIPTING::ScriptInterface::LoadFromPrefab(const std::string& name)
{
    return ASC_FACTORY.CreateGOfromPB(name);
}