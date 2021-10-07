#include "pch.h"





BaseTrigger::BaseTrigger() :
    ScriptCommonIF(ASC_GETSCRIPTNAME(BaseTrigger))
{
 //ASC_SCRIPT_REGISTER_MSG( MsgType::mTYPE_MISSION_END, HandleMessage);

}
void BaseTrigger::ForceRegStuff()
{
  auto obj = this->GetGOHandle();
  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE, obj, GOTYPE::go_player));
  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE_IGNORERAY, obj, GOTYPE::go_player));

  if (linkToOtherMission)
  {

  }
  ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_LINK_TO_AS_ONE_MISSION, GetGOHandle()));
  //if(link)
}

void BaseTrigger::Update(float dt)
{
 // assert(false);
 // std::cout << " hi\n";


}

void BaseTrigger:: OnStart()
{
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

  //roomba stuff
  if (missionType == 1)
  {

  }

  //?
  if (missionType == 2)
  {

  }
  //Sink
  if (missionType == 3)
  {

  }
  //oven 
  if ( missionType == 4)
  {
    //steam need to be off 
    auto steam = ASC_FACTORY.GetGameObjectByName(targetedObj);
    if (steam.IsValid())
    {
      ASC_FACTORY.GetGameObjectByName(targetedObj)->SetEnable ( false);
     // steam->isEnable = false;
    }
   

  }
}

void BaseTrigger:: OnDestroy()
{

}

void BaseTrigger::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write("linkToOtherMission", linkToOtherMission);
  sr.Write("TargetObject", targetedObj);
  sr.Write("MissionNum", missionNum);
  sr.Write("MissionLvl", misionlvl);
  sr.Write("amtOfTimes", amtOfTimes);
  sr.Write("disableAtStart", disableAtStart);
  sr.Write("targetToEnable", targetToEnable);


  sr.Write("MissionType", missionType);
 /* sr.Write("Sound", nameToSoundFileName);*/
  ScriptCommonIF::Serialize(sr, idx);
}
void BaseTrigger::Deserialize(Deserializer& d)
{
  d.Read("linkToOtherMission", linkToOtherMission);
  d.Read("TargetObject", targetedObj);
  d.Read("MissionNum", missionNum);
  d.Read("MissionLvl", misionlvl);
  d.Read("amtOfTimes", amtOfTimes);
  d.Read("disableAtStart", disableAtStart);
  d.Read("targetToEnable", targetToEnable);
  


  d.Read("MissionType", missionType);
  /*d.Read("Sound", nameToSoundFileName);*/
 // intdex1 missionType;
 /* auto obj = this->GetGOHandle();
  if (targetedObj != "")
  {
    ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_NAME, obj, targetedObj));
  }*/
  ScriptCommonIF::Deserialize(d);
}
void BaseTrigger::Reflect(const Reflector& r)
{
  r.Reflect("disableAtStart", disableAtStart);

  ImGui::Separator();
  r.Reflect("TargetObject", targetedObj);
  r.Reflect("targetToEnable", targetToEnable);
  r.Reflect("linkToOtherMission", linkToOtherMission);
  r.Reflect("MissionNum", missionNum);
  r.Reflect("MissionLvl", misionlvl);

  r.Reflect("amtOfTimes", amtOfTimes);
 

 // r.Reflect("li")

  ImGui::Separator();

  if (ImGui::Combo("##TriggerStuff_1", &index1, triggerMissionNames.data(), (uint32_t)triggerMissionNames.size()))
  {
    missionType = index1;
  }
  ImGui::Text(std::to_string(missionType).c_str());
  ImGui::Separator();
 // auto soundList =ASC_SOUNDMAN.GetAllTrack();
 // r.Reflect("Sound", nameToSoundFileName, ASC_SOUNDMAN.GetAllTrack(),false);::
  ScriptCommonIF::Reflect(r);
}

void BaseTrigger::HandleMsgObjectiveDone(MsgData_Base* m)
{
  switch (m->type)
  {
    case mTYPE_GAME_PLAYER_ENABLE_CONTROLS:
    {
      break;
    }
  }
}
void BaseTrigger::ObjectPlaceCorrectly()
{
  ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle() ));
  if (pe.IsValid())
  {
    pe->Emit();
  }
  if (s.IsValid())
  {
    if (!nameToSoundFileName.empty())
    {

      ASC_PLAY(nameToSoundFileName[0]);
    }
  }
}

void BaseTrigger::HandleMessage(MsgData_Base* m)
{
  
}


void BaseTrigger::OnCollisionEnter(GOHandle other)
{
  std::cout << " enter\n";
   PlayerController *player = ASC_GETSCRIPT_PTR(PlayerController,other);
   Interact* interact = ASC_GETSCRIPT_INTERACT( other);
   FakePlacement* placement = ASC_GETSCRIPT_PTR(FakePlacement, goHdl);


 /*  if(player != nullptr)
   {
     if (u.IsValid())
     {
       u->Enabled = true;
     }
   }  */
   //interact behaviour 
 //  std::cout << other->name << " entering" << std::endl;
   if (interact != nullptr && ( missionType ==0  || missionType ==3 ))
   {
       interact->OnCollisionEnter(goHdl);
       if (placement != nullptr)
       {

           placement->OnCollisionEnter(other);

       }
     //check if you belong ot the right spot 

     auto trans = other->GetHdl<TransformComp>();
     auto npc = other->GetHdl<NewPhysicsComp>();
     //trans->pos = t->pos;
    // npc->obj_phy->SetHaveBody(false);
  /*   std::cout << trans->pos.x << std::endl;;
     std::cout << t->pos.x << std::endl; */
  
    // std::cout << "test\n";

     //ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle() ));
    // std::cout << interact->type << std::endl;
    //  ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_ENTER, it->first->npc->GetGOHandle(), it2->first->npc->GetGOHandle()));
    // if (pe.IsValid())
    // {
    //   pe->Emit();
    // }
     if (s.IsValid())
     {
       if (!nameToSoundFileName.empty())
       {
        
          ASC_PLAY(nameToSoundFileName[0]);
       }
     }
   }
   if (interact != nullptr && missionType == 1)
   {
     RoombaEnd();
     ////check if you belong ot the right spot 
     //if (ASC_KEYTRIGGERED(ASC_KEY_E))
     //{
     //    
     //      // RoombaEnd();

     //  
     //}

   }
   //oven 
   if (interact != nullptr && missionType == 4)
   {
     WaterContainer* water = ASC_GETSCRIPT_PTR(WaterContainer, other);
     if (water != nullptr && water->hasWater)
     {
       if (GetGOHandle()->isEnable)
       {
         water->DumbWater();
         auto steam = ASC_FACTORY.GetGameObjectByName(targetedObj);
         if (steam.IsValid())
         {
           steam->SetEnable(true);
         }
         //pull out the fire 
         pe->isLooping = false;
         t->pos = { -9999,-9999,-999 };
         GetGOHandle()->isEnable = false;
         ASC_SOUNDMAN.Stop(s);
         ASC_PLAY_FROMGO("SFX_Extinguish.ogg", steam, 1);
         GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_trigger->GeomCollisionSetEnable(false);
         GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->GeomCollisionSetEnable(false);
         GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->SetHaveBody(false);


       }
     }
    // interact- ;
   }
}
void BaseTrigger::OnCollisionStay(GOHandle other)
{
 // PlayerController *player = ASC_GETSCRIPT_PTR(PlayerController,other);
  Interact* interact = ASC_GETSCRIPT_INTERACT(other);
  std::cout << other->name << " staying" << std::endl;
  if (interact != nullptr)
  {
      interact->OnCollisionStay(goHdl);
  }

  //roomba 
   

}
void BaseTrigger::OnCollisionExit(GOHandle other)
{
    FakePlacement* placement = ASC_GETSCRIPT_PTR(FakePlacement, goHdl);
    std::cout << other->name << " exit" << std::endl;
    if (placement != nullptr)
    {

        placement->OnCollisionExit(goHdl);

    }
    Interact* interact = ASC_GETSCRIPT_INTERACT(other);
    if(interact != nullptr)
    interact->OnCollisionExit(goHdl);
 // std::cout << "exit\n";
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);

  if (player != nullptr)
  {
    if (u.IsValid())
    {
      u->Enabled = false ;
    }
  }
}


void BaseTrigger::RoombaEnd( )
{
  //check if you belong ot the right spot 

 /* auto trans = other->GetHdl<TransformComp>();
  auto npc = other->GetHdl<NewPhysicsComp>();*/
 // trans->pos = t->pos;
  //npc->obj_phy->SetHaveBody(false);
  //std::cout << "test\n";
    if (targetedObj != "")
    {
        RoombaController* rom = ASC_GETSCRIPT_PTR(RoombaController, ASC_FACTORY.GetGameObjectByName(targetedObj));
        rom->SetRoombaActive(true);
    }
  //rom->SetActive(false);
 // ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
  // std::cout << interact->type << std::endl;
  // ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_ENTER, it->first->npc->GetGOHandle(), it2->first->npc->GetGOHandle()));
}

void BaseTrigger::ActivateFire()
{
  pe->Enabled = true;
  ASC_LOOP("SFX_StoveBurning.ogg");
}