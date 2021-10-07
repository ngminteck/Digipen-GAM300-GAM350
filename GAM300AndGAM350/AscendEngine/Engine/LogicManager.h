/******************************************************************************/
/*!
\file   LogicManager.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for LogicManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef LOGIC_MANAGER_H
#define LOCIC_MANAGER_H

namespace SCRIPTING
{
  struct ScriptInterface;
}

enum ScriptIndex;

struct ScriptPoolInfo
{
  ScriptPoolInfo() {};
  ScriptPoolInfo(bool v, int i, SCRIPTING::ScriptInterface* p) : valid(v), idx(i), ptr(p) {}
  bool valid = false;
  int idx = -1;
  SCRIPTING::ScriptInterface* ptr = nullptr;
};

typedef ComponentManagerBase<LogicComp>::COMPHANDLETYPE LogicHandle;

#define ASC_SCRIPTMAN LogicManager::Inst()
class LogicManager : public ComponentManagerBase<LogicComp>, public Singleton<LogicManager>
{
public:

  LogicManager();
  virtual ~LogicManager();

  bool Init = false;
  void Load() override;
  void UnLoad() override;

  void Update(float dt) override;

  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;

  //Handle CreateComponent(const GameObject &gameObject);
  //LogicComp* GetComponent(Handle index) override;
  void DestroyComponent(BaseHandle hdl) override;
  //Handle CloneComponent(Handle hdl) override;
  void InitComponent(const GameObject & go, BaseHandle hdl) override;

  //function from CompManBase
  void CopyComponentToComponent(LogicComp& target, const LogicComp& source) override;

  //For Msging
  void HandleMsg(MsgData_Base *) override;
  void HandleMsgPlay(MsgData_Base *);
  void HandleMsgStop(MsgData_Base *);
  void HandleMsgPause(MsgData_Base *);

  SCRIPTING::ScriptInterface* GetScript(int index);

  void AddScript(LogicHandle compIndex, const std::string&);
  
  template<typename T>
  void AddScript(LogicHandle compIndex);
  //move to public so editor can use
  //void AddScriptToCont(Handle compIndex, SCRIPTING::ScriptInterface*, ScriptPoolInfo& info);
  void AddScriptToCont(LogicHandle compIndex, ScriptPoolInfo& info);

  std::unordered_map<ScriptIndex, std::vector<ScriptPoolInfo>> recycle_map;

  //for sending internal msg between the scripts
  template<typename msgType>
  void SendInternalMsg(LogicHandle destination_comp, const msgType& msg)
  {
    for (int idx : destination_comp->scriptsHdl)
    {
      m_scripts[idx]->HandleInternalMsg(msg);
    }
  }

  //for delay of call of onstart
  void UpdateScriptStart();

  bool isPlaying;
private:
  //for creating script pool in logic ctor only
  void SP_CreateScriptPool();
  void SP_CreateScript(std::string& scriptName, ScriptIndex type);
  //
  void UnloadScripts();
  void Reflect_RemoveScript(LogicHandle& compIdx, int& idx);
  //for play pause function
  typedef void (LogicManager::*SOD)(LogicHandle Hdl);
  void ScriptOnDestroy_Play(LogicHandle hdl);
  void ScriptOnDestroy_Pause(LogicHandle hdl) {};
  SOD ScriptOnDestroy;

  //std::vector<std::string> scriptTypes;

  //std::vector<LogicComp> m_components;
  //HandleManager<LogicComp, Handle> m_components;
  std::vector<SCRIPTING::ScriptInterface*> m_scripts;
   
  //SCRIPTING::ScriptInterface* CreateScriptWOHandle(const std::string& scriptName);
  ScriptPoolInfo CreateScriptWOHandle(const std::string& scriptName);

  

  //to delay the call of on start to the next frame
  std::vector<LogicHandle> m_startdelaylist;
};

template<typename T>
void LogicManager::AddScript(LogicHandle compIndex)
{
  AddScriptToCont(compIndex, new T);
}

#endif
