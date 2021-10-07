#include "pch.h"

std::vector< const char *  > activateType
{
  "NONE",
  "LIGHT",
  "VENT",
  "OVEN",
  "Tap"
};

ActivateInteract::ActivateInteract()
{
	ASC_SETUP_SCRIPT(ActivateInteract);
}
// when button is switch on 
void ActivateInteract::InteractAction()
{
	//Interact* i = ASC_GETSCRIPT_INTERACT(TargetInteract);
	//i->activated = true;
 

  //if ( != "")
  {
    //light
    if (type == 1)
    {
      for (auto& currObj : ObjsToControl)
      {
        auto obj = currObj;
        if (obj.IsValid())
        {
          auto  light = ASC_LIGHTS.GetLightData(obj->GetHdl<LightComp>());
          if (light.GetType() == 1)
          {
            //off light
            ASC_LIGHTS.GetLightData(obj->GetHdl<LightComp>()).SetType(-1);
            for (auto& curr : objs)
            {
              auto em = curr->GetHdl<ParticleEmitter>();
              if (em.IsValid())
              {
                em->isLooping = false;
              }
            }
          }
          else
          {
            //on light 
            ASC_LIGHTS.GetLightData(obj->GetHdl<LightComp>()).SetType(1);
            for (auto& curr : objs)
            {
              auto em = curr->GetHdl<ParticleEmitter>();
              if (em.IsValid())
              {
                em->isLooping = true;
                //em->
              }
            }
          }
          ASC_PLAY("SFX_Switch.ogg");
        }
      }
    }
    //vent
    else if (type == 2)
    {
      ASC_PLAY("SFX_Switch.ogg");
      for (auto& currObj : ObjsToControl)
      {
        auto obj = currObj;
        if (obj.IsValid() && obj->name =="MissionSteam")
        {
          obj->GetHdl<NewPhysicsComp>()->obj_phy->SetHaveBody(false);
          obj->GetHdl<NewPhysicsComp>()->obj_phy->GeomCollisionSetEnable(false);

         // obj->SetEnable(false);
          obj->GetHdl<ParticleEmitter>()->Enabled = false;
  
        }
        if (obj.IsValid() && obj->name == "Stove_Vent")
        {
          obj->GetHdl<ParticleEmitter>()->isLooping = true;
          ASC_PLAY_FROMGO("SFX_AirVent.ogg",obj,1 );
        }
        //vent
     
          timer = ASC_SOUNDMAN.GetTrackLength("SFX_AirVent.ogg");
        
        //play vent Sux sound
      /*  if (!nameToSoundFileName.empty())
        {
          ASC_SOUNDMAN.GetTrackLength(nameToSoundFileName[0]);
        }*/
      }
    }
    //oven
    else if (type == 3)
    {
      for (auto& currObj : ObjsToControl)
      {
        auto obj = currObj;
        if (obj.IsValid())
        {
          
          obj->isEnable =! obj->isEnable;
        
        }
        
      }
    }
    // Tap ( the sink tap)
    else if (type == 4)
    {
      isOn = !isOn;
      for (auto& currObj : ObjsToControl)
      {
        auto obj = currObj;
        if (obj.IsValid())
        {
          WaterContainer* sink = ASC_GETSCRIPT_PTR(WaterContainer, obj);
          sink->ToogleWaterContainerBehaviour();      
        }

      }
    }
    //NONE
    else
    {
      activated = false;
      isTurnOn = true;
      g->isHighlighted = false;
    }
  }

  //of course this wont work if its not a mission 
  //going to tell the objective mananger that one button is done 
  ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));

}
void ActivateInteract::ForceRegStuff()
{
  auto obj = this->GetGOHandle();
  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE, obj, GOTYPE::go_player));
  if (linkToOtherMission)
  {
  }
    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_LINK_TO_AS_ONE_MISSION, GetGOHandle()));

}
void ActivateInteract::OnStart()
{
 // t->dirty = true;
  // connect 
  for (auto& curr : objParticleToTurnOn)
  {
    auto obj = ASC_FACTORY.GetGameObjectByName(curr);
    if (obj.IsValid())
    {
      objs.push_back(obj);
    }
  }
  for (auto& curr : nameOfObjs )
  {
    auto obj = ASC_FACTORY.GetGameObjectByName(curr);
    if(obj.IsValid())
    ObjsToControl.push_back(obj);
  }
  //vent
  if (type == 2)
  {
    timer = ASC_SOUNDMAN.GetTrackLength("SFX_AirVent.ogg");
  }
}

void ActivateInteract::Update(float dt)
{
  if (type == 2)
  {
    if (timer <= 0)
    {
      for (auto& currObj : ObjsToControl)
      {
        auto obj = currObj;

        if (obj.IsValid() && obj->name == "Stove_Vent")
        {
          obj->GetHdl<ParticleEmitter>()->isLooping = false;
        }
      }
    }
    else
    {
      timer -= dt;
    }
  }
}
void ActivateInteract::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write("targetObj" ,targetObj );
	sr.Write(GetName());
	sr.Write("Filename", path);
	//if(TargetInteract.IsValid())
	//  sr.Write("Target", TargetInteract->name);
	sr.Write("Activated", activated);
  //mission stat stuff 
  sr.Write("MissionNum", missionNum);
  sr.Write("MissionLvl", misionlvl);
  sr.Write("linkToOtherMission", linkToOtherMission);
  sr.Write("range", range);

  sr.Write("type", type);


  sr.Write("NameToOff", objParticleToTurnOn);
  sr.Write("NameOfLight", nameOfObjs);

  Interact::Serialize(sr, idx);

}

void ActivateInteract::Deserialize(Deserializer& d)
{
  d.Read("targetObj", targetObj);
  d.Read("type", type);
	d.Read("Filename", path);
	std::string name = "";
	//d.Read("Target", name);
	//  SetupTarget(name);
	d.Read("Activated", activated);
  
  d.Read("MissionNum", missionNum);

  d.Read("MissionLvl", misionlvl);
  d.Read("linkToOtherMission", linkToOtherMission);
  d.Read("range", range);

  d.Read("NameToOff", objParticleToTurnOn);
  d.Read("NameOfLight", nameOfObjs);

	RefreshHintFile();

  Interact::Deserialize(d);
  
}
void ActivateInteract::RefreshHintFile()
{
	std::ifstream hintFile;
	std::string test = ASC_TEXT_FILE;// + path + ".txt";
	test += path.c_str();
	test += ".txt";
	hintFile.open(test);
	// Execute a loop until EOF (End of File) 
	while (hintFile)
	{
		std::string line;
		// Read a Line from File 
		std::getline(hintFile, line);
        if (line != "")
		    hint.push_back(line);
	}
	hintFile.close();
}
void ActivateInteract::Reflect(const Reflector& r)
{
  //to test on and off without player being there. 
  if (ImGui::Button("TestActivateInteraction"))
  {
    InteractAction();
  }
  static int  index22 = 0;
  if (ImGui::Combo("##ActivateInteract", &index22, activateType.data(), (uint32_t)activateType.size()))
  {
    type = index22;
  }
  ImGui::Text(std::to_string(type).c_str());

  ImGui::Text("object To disable/enable");
  r.Reflect("NameoflightToOff", nameOfObjs);
  //r.Reflect("targetObj", targetObj);
  ImGui::Separator();
  if (type == 1)
  {
  /*  std::vector<std::string> allobj;
    for (auto obj : ASC_FACTORY.GetGameObjectList())
    {
      allobj.push_back(obj.name);
    }*/
  
    ImGui::Text("Object particle to control");
    r.Reflect("ObjName", objParticleToTurnOn );
    ImGui::Separator();
  }
  r.Reflect("linkToOtherMission", linkToOtherMission);
  //ImGui::InputInt("MissionNum",&missionNum);
  r.Reflect("MissionNum", missionNum);
  r.Reflect("MissionLvl", misionlvl);

  ImGui::Separator();
	static std::vector<const char*> interactObjects;
	auto objlist = ASC_FACTORY.GetGameObjectList();
	for(int i = 0; i< objlist.size(); i++)
	{
		
		if (objlist[i].GetHdl<LogicComp>().IsValid())
		{
			Interact* itt = ASC_GETSCRIPT_INTERACT(objlist[i].GetSelfHandle());

			if (itt != nullptr)
			{
				interactObjects.push_back(objlist[i].name.c_str());
			}
		}

	}
	static int index = -1;
	static std::string selected_obj;

	if (ImGui::Combo("##objchoice", &index, interactObjects.data(), (uint32_t)interactObjects.size()))
	{
		selected_obj = interactObjects[index];
	}

	ImGui::InputFloat("Range", &range);
	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}

  Interact::Reflect(r);

}

void ActivateInteract::OnDestroy()
{

}

void ActivateInteract::OnCollisionEnter(GOHandle other)
{
}

void ActivateInteract::OnCollisionStay(GOHandle other)
{

}

void ActivateInteract::OnCollisionExit(GOHandle other)
{
}

void ActivateInteract::HandleMessage(MsgData_Base* m)
{
}