#include "pch.h"


UIManager::UIManager()
{
	ASC_SETUP_SCRIPT(UIManager);
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_MISSION_END, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_PLAYER_PRESS_MISSIONLIST, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_MISSION_UPDATE_MISSION, HandleMessage);

}
// when button is switch on 

//void UIManager::ForceRegStuff()
//{
//  auto obj = this->GetGOHandle();
//  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE, obj, GOTYPE::go_player));
//  if (linkToOtherMission)
//  {
//  }
//    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_LINK_TO_AS_ONE_MISSION, GetGOHandle()));
//
//}
void UIManager::OnStart()
{
  promptBox = ASC_FACTORY.GetGameObjectByName("UI_Quest_Prompt");
  if (!promptBox.IsValid())
  {
    promptBox = ASC_FACTORY.CreateGOfromPB("UI_Quest_Prompt");
  }
  missionListUI = ASC_FACTORY.GetGameObjectByName("UI_MissionList");

  if (!missionListUI.IsValid())
  {
    missionListUI = ASC_FACTORY.CreateGOfromPB("UI_MissionList");
  }
  Handle<GraphicsComp> obj = promptBox->GetHdl<GraphicsComp>();
  obj->SetTransparencyVal(0);

  obj = missionListUI->GetHdl<GraphicsComp>();
  obj->SetVisible(false);



}

void UIManager::ExcutingActions(GOHandle objToRun, float dt)
{
  if (std::invoke(actions.front(), this, objToRun, dt))
  {

    // startAction = false;
    actions.pop();
    if (actions.empty())
    {
      startAction = false;
    }
  }
}
void UIManager::Update(float dt)
{
  if (startAction)
  {
    if (std::invoke(actions.front(), this, objToExcute, dt))
    {
   
     // startAction = false;
      actions.pop();
      if (actions.empty())
      {
        startAction = false;
      }
    }
  }
}


void UIManager::Start_QuestCompletePrompt()
{
  ChangeTextureMissionList(missionListUI, 0);
 
  actions.push(&UIManager::ChangeTextureCompleteMission);
  actions.push(&UIManager::FadeOutObj);

  Handle<GraphicsComp> obj = promptBox->GetHdl<GraphicsComp>();

  obj->tint.w = 10.0f;

  obj->SetVisible(false);
  obj->SetVisible(true);
  objToExcute = promptBox;
  startAction = true ;
}

void UIManager::Start_QuestInitalPrompt()
{
  ChangeTextureMissionList(missionListUI, 0);

  actions.push(&UIManager::ChangeTextureNextMission);
  actions.push(&UIManager::FadeOutObj);

  Handle<GraphicsComp> obj = promptBox->GetHdl<GraphicsComp>();

  obj->tint.w = 10.0f;

  obj->SetVisible(false);
  obj->SetVisible(true);
  objToExcute = promptBox;
  startAction = true;
}


void UIManager::Start_OpenMissionList()
{
  actions.push(&UIManager::DoesNothing);


  Handle<GraphicsComp> obj = missionListUI->GetHdl<GraphicsComp>();

  obj->tint.w = 10.0f;

  obj->SetVisible(false);
  obj->SetVisible(true);
  objToExcute = missionListUI; 
  startAction = true;
}
void UIManager::Start_CloseMissionList()
{
 
  actions.push(&UIManager::FadeOutObj);


  Handle<GraphicsComp> obj = missionListUI->GetHdl<GraphicsComp>();

  obj->tint.w = 10.0f;

  obj->SetVisible(false);
  obj->SetVisible(true);
  objToExcute = missionListUI;

  startAction = true;
}


bool UIManager::FadeOutObj(GOHandle obj,float dt)
{
  Handle<GraphicsComp> graph = obj->GetHdl<GraphicsComp>();
  graph->tint.w -= dt;
  if (graph->tint.w <= 0)
  {
    return true;
  }
  return false;
 /* std::cout << " okay\n";*/
}
bool UIManager::ShiftDown(GOHandle obj, float dt)
{
  obj;
  return false;
}
bool UIManager::ShiftUp(GOHandle, float dt )
{
  return false;
}
bool UIManager::ChangeTexture(GOHandle obj,std::string textureName  )
{
  Handle<GraphicsComp> graph = obj ->GetHdl<GraphicsComp>();

  graph->materialList[0].AlbedoMap = textureName;

  //graph->tint.w = 10.0f;
  //graph->SetVisible(false);
  //graph->SetVisible(true);
  //graph->SetVisible(false);


  return true; 
}
bool UIManager::ChangeTextureNextMission(GOHandle obj, float dt)
{
  if (currMission < missNumToTextureNextMission.size())
  {
    ChangeTexture(obj, missNumToTextureNextMission[currMission]);
    obj->GetHdl<GraphicsComp>()->tint.w = 5.0f;
  }

  return true;
}
bool UIManager::ChangeTextureCompleteMission(GOHandle obj, float dt)
{
  if (currMission < missNumToTexture.size())
  {
    ChangeTexture(obj, missNumToTexture[currMission]);
    obj->GetHdl<GraphicsComp>()->tint.w =5.0f;
  }
  return true; 
}
bool UIManager::ChangeTextureMissionList(GOHandle obj , float dt)
{
  if(currMission < missNumToTextureForMissionList.size() )
    ChangeTexture(obj, missNumToTextureForMissionList[currMission]);
  return true; 
}

void UIManager::Serialize(Serializer& sr, LogicHandle idx)
{
 // sr.Write("targetObj" ,targetObj );
	//sr.Write(GetName());
	//sr.Write("Filename", path);
	////if(TargetInteract.IsValid())
	////  sr.Write("Target", TargetInteract->name);
	//sr.Write("Activated", activated);
 // //mission stat stuff 
 // sr.Write("MissionNum", missionNum);
 // sr.Write("MissionLvl", misionlvl);
 // sr.Write("linkToOtherMission", linkToOtherMission);
 // sr.Write("range", range);

 // sr.Write("type", type);


 // sr.Write("NameToOff", objParticleToTurnOn);

 // Interact::Serialize(sr, idx);
  sr.Write("numToTexture", missNumToTexture);
  sr.Write("missNumToTextureNextMission", missNumToTextureNextMission);
  sr.Write("missNumToTextureForMissionList", missNumToTextureForMissionList);
}

void UIManager::Deserialize(Deserializer& d)
{
  d.Read("numToTexture", missNumToTexture);
  d.Read("missNumToTextureNextMission", missNumToTextureNextMission);
  d.Read("missNumToTextureForMissionList", missNumToTextureForMissionList);
 // d.Read("targetObj", targetObj);
 // d.Read("type", type);
	//d.Read("Filename", path);
	//std::string name = "";
	////d.Read("Target", name);
	////  SetupTarget(name);
	//d.Read("Activated", activated);
 // 
 // d.Read("MissionNum", missionNum);

 // d.Read("MissionLvl", misionlvl);
 // d.Read("linkToOtherMission", linkToOtherMission);
 // d.Read("range", range);

 // d.Read("NameToOff", objParticleToTurnOn);
	//RefreshHintFile();

 // Interact::Deserialize(d);
  
}

void UIManager::Reflect(const Reflector& r)
{
  auto source = ASC_GMAN.GetTextureList();
  ImGui::Text("EndMission");
  r.Reflect("numToTexture", missNumToTexture,source);
  ImGui::Text("Nextmission");
  r.Reflect("missNumToTextureNextMission", missNumToTextureNextMission, source);
  ImGui::Text("MissionListTexture");
  r.Reflect("missNumToTextureForMissionList", missNumToTextureForMissionList, source);
  ImGui::Text(std::to_string(type).c_str());

  if (ImGui::Button("test"))
  {
    Start_QuestCompletePrompt();
  }

}

void UIManager::OnDestroy()
{

}

void UIManager::OnCollisionEnter(GOHandle other)
{
}

void UIManager::OnCollisionStay(GOHandle other)
{

}

void UIManager::OnCollisionExit(GOHandle other)
{
}


void UIManager::HandleMessage(MsgData_Base* m)
{
  Msg_Logic_Mission* realMsg = static_cast<Msg_Logic_Mission*> (m);
  
 // auto infor = ASC_GETSCRIPT_INFOR_PTR(realMsg->gameObject);
  //Msg_MissionListStuff* secondMsg = static_cast<Msg_MissionListStuff*> (m);


  switch (m->type)
  {
    case mTYPE_MISSION_END:
    {
      Start_QuestCompletePrompt();
      currMission =realMsg->num;
      break;
    }
    case mTYPE_MISSION_UPDATE_MISSION:
    {
      std::cout << "what\n";
      Start_QuestInitalPrompt();
      currMission = realMsg->num;
      break;
    }
    case mTYPE_PLAYER_PRESS_MISSIONLIST://PlayerController Press Q
    {
      if (missionListUI->GetHdl<GraphicsComp>()->GetVisible())
      {
        Start_CloseMissionList();
      }
      else
      {
        Start_OpenMissionList();
      }
      break;
    }
  }

}