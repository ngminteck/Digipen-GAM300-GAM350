/******************************************************************************/
/*!
\file   LogicManager.cpp
\author Goh Zi He
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for LogicManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

#define S SCRIPTING


LogicManager::LogicManager()
  : ScriptOnDestroy(&LogicManager::ScriptOnDestroy_Pause),
  isPlaying(false)
{
  //m_components.reserve(50);

  ASC_MSGSYS.RegisterCallBack(mTYPE_LOGIC_ONCOLLIDE_ENTER, &LogicManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_LOGIC_ONCOLLIDE_EXIT, &LogicManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_LOGIC_ONCOLLIDE_STAY, &LogicManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY_SCRIPT_INIT, &LogicManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &LogicManager::HandleMsg, this);

  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &LogicManager::HandleMsgPlay, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &LogicManager::HandleMsgStop, this);

  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_PAUSE, &LogicManager::HandleMsgPause, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_RESUME, &LogicManager::HandleMsgPause, this);

  //SP_CreateScriptPool();

  m_startdelaylist.reserve(1000);
}

LogicManager::~LogicManager()
{
  LogicManager::UnLoad();
}

void LogicManager::Load()
{
    if (!Init)
    {
        for (size_t i = 0; i < m_components.m_objects.size(); ++i)
        {
            auto& elem = m_components.m_objects[i];

            for (auto& idx : elem.scriptsHdl) {
                auto script = m_scripts[idx];
                script->ScriptInitialize();
            }

        }
        Init = true;
    }
   // std::cout << "load" << std::endl;
}

void LogicManager::UnLoad()
{
  for (auto & elem : m_components.m_objects)
    elem.scriptsHdl.clear();

  m_components.Clear();

  UnloadScripts();

  for (auto& elem : recycle_map)
    elem.second.clear();
}

void LogicManager::Update(float dt)
{
    if (isPlaying)
    {
        start = Clock::now();



        //start the script created previous frame if exist
        UpdateScriptStart();

        //for (auto& elem : m_components.m_objects)
        for (size_t i = 0; i < m_components.m_objects.size(); ++i)
        {
            auto& elem = m_components.m_objects[i];
            if (elem.Enabled && elem.GetGOHandle()->GetIsEnable()) {
                for (auto& idx : elem.scriptsHdl) {
                    auto script = m_scripts[idx];
                    if (script->IsActive())
                        script->ScriptUpdate(dt);
                }
            }
        }

        duration = Clock::now() - start;
    }
}

BaseHandle LogicManager::Deserialize(const GameObject &gameObject, Deserializer &d)
{
  //Load script from scene file here!
  const int numOfScripts = d.m_xmlElem.IntAttribute("count");

  LogicHandle c_idx(CreateComponent());
  LogicComp& comp = *m_components.GetObj(c_idx); //[c_idx];
  comp.SetGOHandle(gameObject.GetSelfHandle());
  
  //if (numOfScripts)
  {
    //logic child -> script element
    tinyxml2::XMLElement *next = d.m_xmlElem.FirstChildElement();

    //for (int i = 0; i < numOfScripts; ++i)
    while (next)
    {
      Deserializer d2(*next);
      std::string name(next->Attribute(ASC_SERIAL_NAME));
      ScriptPoolInfo info = CreateScriptWOHandle(name);

      SCRIPTING::ScriptInterface* script = info.ptr;
      script->InitHandle(gameObject.GetSelfHandle());
      script->ScriptDeserialize(d2);

      if (!info.valid)
      {
        m_scripts.push_back(script);
        //comp.scriptsHdl.push_back(SCRIPTING::StrIdx_map[name]);
        comp.scriptsHdl.push_back((int)m_scripts.size() - 1);
      }
      else
      {
        //recycle
        comp.scriptsHdl.push_back(info.idx);
      }

      next = next->NextSiblingElement();
    }
  }
  d.Read("IsEnable", comp.Enabled);
  return c_idx;
}

void LogicManager::Serialize(Serializer &sr, BaseHandle h)
{
  //saving to file for each object
  LogicHandle c_idx(h);
  //if (c_idx >= m_components.size() || m_components.empty()) return;
  //if (m_components.m_objects.empty()) return;

  LogicComp& comp = *m_components[c_idx];
  //sr.Write("IsEnable", comp.Enabled);
  for (auto& s_idx : comp.scriptsHdl)
  {
    tinyxml2::XMLElement& newelem = sr.CreateNewChild("Script");
    Serializer sr2(newelem);

    S::ScriptInterface* s = m_scripts[s_idx];
    s->ScriptSerialize(sr2, c_idx);
  }

  //sr.m_xmlElem.SetAttribute("count", (int)comp.scriptsHdl.size());
}

void LogicManager::Reflect(BaseHandle h, const Reflector & r)
{
#define INVALID_IDX -1

  LogicHandle compIdx(h);
  LogicComp& comp = *GetComponent(h);
  //if (compIdx > m_components.size()) return;
  //if (compIdx.Val() > m_components.m_objects.size()) return;

  //ImGui::PushID(10);
  static int curr_SCRIPT_type = 0;
  static int curr_SCRIPTs_nameIdx = INVALID_IDX;
  bool val = comp.Enabled;
  
 /* if (ImGui::Checkbox("Is Enable##EnableLogic",&val))
  {
    comp.Enabled = val;
  }*/
  //Adding available scripts to comp
  //r.Reflect("", scriptTypes, &curr_SCRIPT_type);
  ImGui::Combo("", &curr_SCRIPT_type, SItype::TypeList.data(), (int)SItype::TypeList.size());

  ImGui::SameLine();

  // Don't allow add while game is running
  if (!isPlaying && ImGui::Button("+ Script"))
  {
    auto SIitr = SItype::m_creator.find((ScriptIndex)curr_SCRIPT_type);
    if (SIitr != SItype::m_creator.end())
    {
      // get a new object
      //ASC_SCRIPTMAN.AddScriptToCont(compIdx, SIitr->second->CreateNew());
      ASC_SCRIPTMAN.AddScriptToCont(compIdx, SIitr->second->CreateNew(SItype::TypeList[curr_SCRIPT_type]));
    }
    else
      LogWarning("LogicManager::Reflect cannot find script~!\n");

    curr_SCRIPTs_nameIdx = (int)m_components[compIdx]->scriptsHdl.size() - 1;
  }
  //ImGui::PopID();

  //tracking the scripts in this component
  std::vector<std::string> name;
  for (auto elem : m_components[compIdx]->scriptsHdl)
  {
    S::ScriptInterface* s = m_scripts[elem];
    if (!s->inUse)
      continue;
    name.push_back(s->GetName());
  }

  ImGui::Separator();
  ImGui::Separator();
  ImGui::PushID(11);
  //tracking the scripts in this component
  r.Reflect("", name, &curr_SCRIPTs_nameIdx);
  ImGui::SameLine();
  ImGui::Text(("(" + std::to_string(name.size()) + ") Scripts  ").data());
  ImGui::SameLine();
  if (ImGui::Button("-"))
  {
    Reflect_RemoveScript(compIdx, curr_SCRIPTs_nameIdx);
  }
  ImGui::PopID();

  //make sure doesnt go out of range when selecting another GO with diff# of scripts
  if (curr_SCRIPTs_nameIdx >= m_components[compIdx]->scriptsHdl.size())
  {
    curr_SCRIPTs_nameIdx = (int)m_components[compIdx]->scriptsHdl.size() - 1;
  }

  if (curr_SCRIPTs_nameIdx != INVALID_IDX && m_components[compIdx]->scriptsHdl.size())
  {
    S::ScriptInterface* s = m_scripts[m_components[compIdx]->scriptsHdl[curr_SCRIPTs_nameIdx]];
    if (s->inUse)
    {
      ImGui::PushID(1);
      //need to retrieve value once, if not when game is playing, cannot adjust values
      s->Reflect(r);
      ImGui::PopID();

      if (ImGui::Button("Edit Done"))
      {
        curr_SCRIPTs_nameIdx = INVALID_IDX;
      }

      ImGui::SameLine();

      if (ImGui::Button("- Script"))
      {
        Reflect_RemoveScript(compIdx, curr_SCRIPTs_nameIdx);
        //m_scripts[curr_SCRIPTs_nameIdx]->inUse = false;
        //std::vector<int>::iterator iter = m_components[compIdx]->scriptsHdl.begin();
        //std::advance(iter, curr_SCRIPTs_nameIdx);
        //m_components[compIdx]->scriptsHdl.erase(iter);

        //--curr_SCRIPTs_nameIdx;
      }
    }
  }

  name.clear();

#undef INVALID_IDX
}

//Handle LogicManager::CreateComponent(const GameObject & gameObject)
//{
//  Handle idx = m_components.GetNewObject();
//  //int idx = (int)m_components.size();
//  //m_components.push_back(LogicComp());
//  //scripts[idx].parent_index = gameObject.GetSelfHandle();
//  return idx;
//}

//LogicComp* LogicManager::GetComponent(Handle index)
//{
//  //if (index.Val() < m_components.m_objects.size())
//    return m_components[index];
//  //return nullptr;
//}

void LogicManager::DestroyComponent(BaseHandle hdl)
{
  LogicHandle lchdl(hdl);
  LogicComp* lc = GetComponent(lchdl);
  (this->*ScriptOnDestroy)(lchdl);
  //for (auto &elem : lc->scriptsHdl)
  //{
  //  delete m_scripts[elem];
  //  m_scripts[elem] = nullptr;
  //}

  //for (auto& elem : lc->scriptsHdl)
  for (int i = 0; i < lc->scriptsHdl.size(); ++i)
  {
    int idx = lc->scriptsHdl[i];
    SCRIPTING::ScriptInterface* script = m_scripts[idx];
    ScriptIndex type = sitpInit.StrToType[script->GetName()];

    recycle_map[type].push_back(ScriptPoolInfo(true, idx, script));
    script->~ScriptInterface();
  }

  lc->scriptsHdl.clear();
  m_components.ReleaseObj(lchdl);
}

//Handle LogicManager::CloneComponent(Handle hdl)
//{
//  return CloneComponent(*m_components[hdl]);
//}


//new gameobject will need to call this so that the components know which gameobject they
//are connected to
void LogicManager::InitComponent(const GameObject & go, BaseHandle hdl)
{
  LogicComp* lc = m_components[LogicHandle(hdl)];
  lc->SetGOHandle(go.GetSelfHandle());

  if (isPlaying) 
  {
    //init the scripts in logic comp
    //for (int scriptIdx : lc->scriptsHdl)
    //{
    //  m_scripts[scriptIdx]->InitHandle(lc->GetGOHandle());
    //  //if (isPlaying)
    //  {
    //    //m_scripts[scriptIdx]->ScriptStart();
    //    m_scripts[scriptIdx]->ScriptInterface::ScriptStart();
    //  }
    //}

    //to delay calling of start to next frame 
    m_startdelaylist.push_back(lc->GetSelfHandle());
    //to delay until start has been called
    lc->Enabled = false;
  }

  //this is a must to update all script of their
  //GoHandle and respective components handle 
  for (int scriptIdx : lc->scriptsHdl)
  {
    m_scripts[scriptIdx]->InitHandle(lc->GetGOHandle());
    m_scripts[scriptIdx]->ScriptUpdateHandle();
  }
}

void LogicManager::CopyComponentToComponent(LogicComp & target, const LogicComp & source)
{
  //need to clear the vec in the new comp since it is holding the old comp script handle
  target.scriptsHdl.clear();
  LogicHandle tarhdl = target.GetSelfHandle();
  for (int idx : source.scriptsHdl)
    AddScript(tarhdl, m_scripts[idx]->GetName());
}

SCRIPTING::ScriptInterface* LogicManager::GetScript(int index)
{
  if (index < m_scripts.size())
    return m_scripts[index];
  return nullptr;
}

void LogicManager::AddScript(LogicHandle compIndex, const std::string& name)
{
  AddScriptToCont(compIndex, CreateScriptWOHandle(name));
}


//this is to delay the call of onstart of script to the next frame
//this is so all comp could be init first before onstart is call
//as well as other script that created the script could do stuff
//with it like init with some sort of default value
void LogicManager::UpdateScriptStart()
{
	for (auto lgchdl : m_startdelaylist)
	{
    if (lgchdl.IsValid())
    {
      for (int scptidx : lgchdl->scriptsHdl)
        m_scripts[scptidx]->ScriptStart();

      lgchdl->Enabled = true;
    }
	}
	m_startdelaylist.clear();
}

void LogicManager::SP_CreateScriptPool()
{
  for (int i = 0; i < sitpInit.TypeList.size(); ++i)
  {
    std::string scriptName = sitpInit.TypeList[i];
    ScriptIndex type = sitpInit.StrToType[sitpInit.TypeList[i]];

      for (int i = 0; i < 20; ++i)
      {
        SP_CreateScript(scriptName, type);
      }
  }
}

void LogicManager::SP_CreateScript(std::string& scriptName, ScriptIndex type)
{
  auto SIitr = SItype::m_creator.find(SItype::StrToType[scriptName]);
  if (SIitr != SItype::m_creator.end())
  {
    // get a new object
    SCRIPTING::ScriptInterface* script = SIitr->second->CreateNew_Raw(scriptName);
    m_scripts.push_back(script);
    recycle_map[type].push_back(ScriptPoolInfo(true, (int)m_scripts.size() - 1, script));
  }
}

void LogicManager::UnloadScripts()
{
  std::vector<int> v1;
  for (auto& type : recycle_map)
  {
    for (auto& s : type.second)
    {
      int size = (int)sizeof(type);
      free(s.ptr);
      v1.push_back(s.idx);
    }
  }
  std::sort(v1.begin(), v1.end());
  std::vector<int> total(m_scripts.size());         // vector with 100 ints.
  std::iota(std::begin(total), std::end(total), 0); // Fill with 0, 1, ..., 99.

  std::vector<int> result;
  std::set_difference(total.begin(), total.end(), v1.begin(), v1.end(), back_inserter(result));

  for (auto& idx : result)
  {
    delete m_scripts[idx];
  }

  m_scripts.clear();
}

void LogicManager::Reflect_RemoveScript(LogicHandle & compIdx, int& curr_SCRIPTs_nameIdx)
{
  m_scripts[curr_SCRIPTs_nameIdx]->inUse = false;
  std::vector<int>::iterator iter = m_components[compIdx]->scriptsHdl.begin();
  std::advance(iter, curr_SCRIPTs_nameIdx);
  m_components[compIdx]->scriptsHdl.erase(iter);

  --curr_SCRIPTs_nameIdx;
}

void LogicManager::ScriptOnDestroy_Play(LogicHandle hdl)
{
  LogicComp* lcp = m_components[hdl];
  for (int shdl : lcp->scriptsHdl)
  {
    m_scripts[shdl]->ScriptDestroy();
  }
}

//Handle LogicManager::CloneComponent(const LogicComp & lcomp)
//{
//  Handle chdl = m_components.GetNewObject(lcomp);
//  LogicComp* newcomp = m_components[chdl];
//
//
//  return chdl;
//}

void LogicManager::AddScriptToCont(LogicHandle cont_idx, ScriptPoolInfo& info)
{
  if (info.valid)
  {
    m_components[cont_idx]->AddScript(info.idx);
    return;
  }

  int scriptIdx = (int)m_scripts.size();
  m_components[cont_idx]->AddScript(scriptIdx);
  m_scripts.push_back(info.ptr);

  m_scripts[scriptIdx]->InitHandle(m_components[cont_idx]->GetGOHandle());
  m_scripts[scriptIdx]->ScriptUpdateHandle();

  //setting up of the gameobject handle will be done in InitComp
}

//SCRIPTING::ScriptInterface *LogicManager::CreateScriptWOHandle(const std::string & scriptName)
ScriptPoolInfo LogicManager::CreateScriptWOHandle(const std::string & scriptName)
{
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(PlayerController)))
  //  return new PlayerController();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(Bullet)))
  //  return new Bullet();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(Tower)))
  //  return new Tower();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(EnemyGround)))
  //  return new EnemyGround();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(FrostTower)))
  //  return new FrostTower();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(EnemySpawner)))
  //  return new EnemySpawner();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(PlayerShoot)))
  //  return new PlayerShoot();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(SplashScreen)))
  //  return new SplashScreen();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(Harvester)))
  //  return new Harvester();
  //if (compare_str(scriptName, ASC_GETSCRIPTNAME(PowerPlant)))
  //  return new PowerPlant();

  auto SIitr = SItype::m_creator.find(SItype::StrToType[scriptName]);
  if (SIitr != SItype::m_creator.end())
  {
    // get a new object
    //return SIitr->second->CreateNew();
    return SIitr->second->CreateNew(scriptName);
  }

  return ScriptPoolInfo();
}

void LogicManager::HandleMsg(MsgData_Base *msg)
{
  using fn = void(SCRIPTING::ScriptInterface::*[])(GOHandle);
  fn collideFns = {
    &SCRIPTING::ScriptInterface::OnCollisionEnter,
    &SCRIPTING::ScriptInterface::OnCollisionExit,
    &SCRIPTING::ScriptInterface::OnCollisionStay,
  };

  switch (msg->type)
  {
  case mTYPE_LOGIC_ONCOLLIDE_ENTER:
  case mTYPE_LOGIC_ONCOLLIDE_EXIT:
  case mTYPE_LOGIC_ONCOLLIDE_STAY:
  {
    auto *onCollideEnterMsg = static_cast<Msg_Logic_OnCollide *>(msg);
    unsigned sz = static_cast<unsigned>(m_components.m_objects.size());
    for (unsigned index = 0; index < sz; ++index)
    {
      auto& elem = m_components.m_objects[index];
      if (elem.GetGOHandle() == onCollideEnterMsg->gameObject)
      {
        if (elem.Enabled)
        {
          // Iterate through the logic in the gameObject
          for (auto &idx : elem.scriptsHdl)
          {
            unsigned int fn_idx = msg->type - mTYPE_LOGIC_ONCOLLIDE_ENTER;
            (m_scripts[idx]->*collideFns[fn_idx])(onCollideEnterMsg->other);
          }
        }
      }
    }
  } break;

  case mTYPE_PLAY_SCRIPT_INIT:
  {
    isPlaying = true;
    //for (auto &elem : m_components.m_objects)
    for (size_t i = 0; i < m_components.m_objects.size(); ++i)
    {
      auto &elem = m_components.m_objects[i];
      if (elem.Enabled)
      {
        // Iterate through the logic in the gameObject
        for (auto &idx : elem.scriptsHdl)
          m_scripts[idx]->ScriptStart();
      }
    }
  } break;

  case mTYPE_STOP:
  {
    isPlaying = false;
    //for (size_t i = 0; i < m_components.m_objects.size(); ++i)
    //{
    //  auto &elem = m_components.m_objects[i];
    //  if (elem.Enabled)
    //    // Iterate through the logic in the gameObject
    //    for (auto &idx : elem.scriptsHdl)
    //      m_scripts[idx]->ScriptDestroy();
    //}
  } break;
  default: break;
  }
}

void LogicManager::HandleMsgPlay(MsgData_Base * msg)
{
  ScriptOnDestroy = &LogicManager::ScriptOnDestroy_Play;
}

void LogicManager::HandleMsgStop(MsgData_Base * msg)
{
  ScriptOnDestroy = &LogicManager::ScriptOnDestroy_Pause;
}

void LogicManager::HandleMsgPause(MsgData_Base *mb)
{
  switch (mb->type)
  {
  //case mTYPE_GAME_PAUSE:
  //{
  //  for (auto i = 0; i < m_components.m_objects.size(); ++i)
  //  {
  //    auto &elem = m_components.m_objects[i];
  //    for (auto &idx : elem.scriptsHdl)
  //      m_scripts[idx]->Pause();
  //  }
  //} break;

  case mTYPE_GAME_RESUME:
  {
    for (auto i = 0; i < m_components.m_objects.size(); ++i)
    {
      auto &elem = m_components.m_objects[i];
      for (auto &idx : elem.scriptsHdl)
        m_scripts[idx]->Resume();
    }
  } break;
  default: break;
  }
}

#undef S