#include "pch.h"




EventTrigger::EventTrigger() 
 {
  ASC_SETUP_SCRIPT(EventTrigger);
// ASC_SCRIPT_REGISTER_MSG( MsgType::mTYPE_MISSION_END, HandleMessage);

}
void EventTrigger::ForceRegStuff()
{
  BaseTrigger::ForceRegStuff();
  //if(link)
}

void EventTrigger::Update(float dt)
{
 // assert(false);
 // std::cout << " hi\n";

}

void EventTrigger:: OnStart()
{
  objectiveManager  =  ASC_FACTORY.GetGameObjectByName("ObjectiveManager");

  /*  if(g.IsValid())
  g->Enabled = false;*/
 // u->Enabled = false;
  //auto obj = this->GetGOHandle();
  //if (targetedObj != "")
  //{
  //  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_NAME, obj, targetedObj));
  //}

  //this is the default beahviour , to turn off the graphic 
  if (g.IsValid() && missionType == 0 )
  {
    g->Enabled = false;
  }
  //onlight
  if (missionType == 1)
  {
    g->Enabled = false;
  }
  //offlight
  if (missionType == 2)
  {
    g->Enabled = false;
  }
  //narrative stuff 
  if (missionType == 3)
  {
    g->Enabled = false;
  }
  //water 
  if (missionType == 4)
  {
    //does nothing 
  }
  //prompt
  if (missionType == 5)
  {
    g->Enabled = false;
  }
  //start stove
  if (missionType == 6)
  {
    g->Enabled = false;
    gameobjectToUse = ASC_FACTORY.GetGameObjectByName("MissisonFire");//yes mission is spell wrong, but dont change it <.<
  }
  BaseTrigger::OnStart();
}

void EventTrigger:: OnDestroy()
{

}

void EventTrigger::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write("MissionType",missionType);
  sr.Write("happenOnceOnly", happenOnceOnly);
  if (missionType == 3 || missionType == 5)
  {
    sr.Write("narrativeIndex", narrativeIndex);
  }
  BaseTrigger::Serialize(sr,idx);
}
void EventTrigger::Deserialize(Deserializer& d)
{
  d.Read("happenOnceOnly", happenOnceOnly);
  d.Read("MissionType",missionType );
  if (missionType == 3 || missionType == 5)
  {
    d.Read("narrativeIndex", narrativeIndex);
  }

  BaseTrigger::Deserialize(d);
}
void EventTrigger::Reflect(const Reflector& r)
{
  r.Reflect("happenOnceOnly", happenOnceOnly);
  ImGui::SameLine();
  ImGui::Text(missionNames[missionType].c_str());
  if (r.Reflect("MissionType", missionNames, &index1))
  {
    missionType = index1;
  }
  if (missionType == 3 || missionType == 5)
  {
    r.Reflect("IndexToUse", narrativeIndex);
  }
  ImGui::Separator();
  ImGui::Text("base trigger variables");
  BaseTrigger::Reflect(r);
}

void EventTrigger::HandleMsgObjectiveDone(MsgData_Base* m)
{
  switch (m->type)
  {
    case mTYPE_GAME_PLAYER_ENABLE_CONTROLS:
    {
      break;
    }
  }
}

void EventTrigger::HandleMessage(MsgData_Base* m)
{

}

void EventTrigger::LightOnBehaviour(GOHandle other)
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);
  if (player != nullptr)
  {
    // std::cout << " Player touching event trigger \n";
   //std::cout << " time to off ";
    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
      if (it->GetType() != go_LightsOff) continue;

      auto lightcomp = it->GetHdl<LightComp>();
      if (lightcomp.IsValid())
      {

        ASC_LIGHTS.GetLightData(lightcomp).SetType(-1);
        //offlight



      }
    }
  }
}    

void EventTrigger::LightOffBehaviour(GOHandle other)
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);
  if (player != nullptr)
  {
    // std::cout << " Player touching event trigger \n";
   //std::cout << " time to off ";
    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
      if (it->GetType() != go_LightsOff) continue;

      auto lightcomp = it->GetHdl<LightComp>();
      if (lightcomp.IsValid())
      {
        ASC_LIGHTS.GetLightData(lightcomp).SetType(1);
        //onlights
      }
    }
  }
}

void EventTrigger::NarrativeTriggerBehaviour(GOHandle other )
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);
 
  if (player != nullptr)
  {
    ObjectiveManager* objMan=  ASC_GETSCRIPT_PTR(ObjectiveManager, objectiveManager);
    objMan->StartNarrtive(narrativeIndex);
   // objectiveManager;
  

  }
}
void EventTrigger::SinkTrigger(GOHandle other)
{
  WaterContainer* water = ASC_GETSCRIPT_PTR(WaterContainer, other);
  if (water != nullptr)
  {
    std::cout << " starting water for :" << other->name << std::endl;
    water->CollectWater();
   /* ObjectiveManager* objMan = ASC_GETSCRIPT_PTR(ObjectiveManager, objectiveManager);
    objMan->StartNarrtive(narrativeIndex);*/
    // objectiveManager;


  }
}
void EventTrigger:: PromptTrigger(GOHandle other )
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);

  if (player != nullptr)
  {
    std::cout << "test\n";
    //pass to UImananger
    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_UPDATE_MISSION, narrativeIndex));
    // objectiveManager;


  }
  
}
void EventTrigger::StartFireTrigger(GOHandle other)
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);

  if (player != nullptr)
  {
    BaseTrigger* objMan = ASC_GETSCRIPT_PTR(BaseTrigger, gameobjectToUse);
    objMan->ActivateFire();
    ObjectiveManager* o = ASC_GETSCRIPT_PTR(ObjectiveManager, objectiveManager);
    o->PlayFireBgm();
    // objectiveManager;
   
  }
}

void EventTrigger::OnCollisionEnter(GOHandle other)
{
  //std::cout << "test\n";

  switch (missionType)
  {
  case 1://light_ALL turn off 
  {
    LightOnBehaviour(other);
    break;
  }
  case 2://light_ALL turn on
  {
    LightOffBehaviour(other);
    break;
  }
  case 3://trigger narrative 
  {
    NarrativeTriggerBehaviour(other);

    break;
  }
  case 4://sink
  {
    //SinkTrigger(other);
    break; 
  }
  case 5://prompt
  {
     PromptTrigger(other);
    break;
  }
  case 6://start stove
  {
    StartFireTrigger(other);
    break;
  }
  }
  
  

 
}
void EventTrigger::OnCollisionStay(GOHandle other)
{
 // PlayerController *player = ASC_GETSCRIPT_PTR(PlayerController,other);
  Interact* interact = ASC_GETSCRIPT_INTERACT(other);
 


  //roomba 
   

}
void EventTrigger::OnCollisionExit(GOHandle other)
{
  std::cout << "exit\n";
 /* PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);

  if (player != nullptr)
  {
    if (u.IsValid())
    {
      u->Enabled = false ;
    }
  }*/
  if (happenOnceOnly)
  {
    np->obj_trigger->GeomCollisionSetEnable(false);
   //   np->obj_trigger = false;
  }
}


void EventTrigger::OffALLLightsEvent()
{
  
}



