#include "pch.h"

WaterContainer::WaterContainer()
 : ScriptCommonIF(ASC_GETSCRIPTNAME(WaterContainer))

{
}

WaterContainer::~WaterContainer()
{
}

void WaterContainer::OnStart()
{
    //auto duration = ASC_SOUNDMAN.GetTrackLength(nameToSoundFileName[0]);
    //fillRate = 1.0f / duration;
    //SetWayPoints();
    //obj= ASC_FACTORY.GetGameObjectByName(targetObj);
    //Handle < NewPhysicsComp> phy =obj->GetHdl<NewPhysicsComp>();
    //if (!ASC_SOUNDMAN.IsPlaying(s, "SFX_DragObject.ogg"))
    //  ASC_LOOP("SFX_DragObject.ogg");
    ////stop the puppy from moving and being reactable 
    //if (phy.IsValid())
    //{
    //  phy->obj_phy->BodySetGravityEnable(false);
    // // phy->obj_phy->
    //  phy->obj_phy->GeomCollisionSetEnable(false);
    // // phy->obj_phy->setcoll
    //  //phy->obj_phy->SetHaveBody(false);
    // // phy->obj_phy->BodySetKinematic();
    //}
    //np->obj_phy->SetHaveBody(false);
    //sizeOfWaypoint = 2;
    //listOfWaypoint.resize(sizeOfWaypoint);
    //listOfWaypointTempName.resize(sizeOfWaypoint);
    //listOfWaypoint[0] = ASC_FACTORY.GetGameObjectByName("pos1");
    //listOfWaypoint[1] = ASC_FACTORY.GetGameObjectByName("pos2");
}
void WaterContainer::ToogleWaterContainerBehaviour()
{
  switch (missionType)
  {
  case 1:// sink
  {
    pe->isLooping =! pe->isLooping;
    //pe->Enabled = !pe->Enabled;
    isOn = !isOn;
    if (!isOn)
    {
      ASC_SOUNDMAN.Stop(s);
    }
    else if (!nameToSoundFileName.empty())
    {
      ASC_PLAY(nameToSoundFileName[0]);
      auto duration = ASC_SOUNDMAN.GetTrackLength(nameToSoundFileName[0]);   
      duration -= 2.0f;
      fillRate = (maxScale / (duration / (1.0f/60.0f)  ));
      timer = duration;
    }
    
    break;
  }
  case 2:// water bucket 
  {
    break;
  }
  default:
    break;
  }
 
}
void WaterContainer::Update(float dt)
{
 // std::cout << t->scale.y << std::endl;
  if (missionType == 1)
  {
    //when there is a pluger there 
    if (activate)
    {
      
      /*  auto name = obj->name;*/
      t->scale.y -= (!isOn) ? 0 : -fillRate;
      if (isOn && t->scale.y >= maxScale)
      {
        t->scale.y = maxScale;
        
      }
      else
      {
        t->dirty = true;
      }
    }
    else // not plug water wont fill 
    {
      if (!isOn)
      {
        t->scale.y = 0.0f;
        t->dirty = true;
      }
      else
      {
        t->scale.y = 0.4f;
        t->dirty = true; 

      }
    
    }
    if (timer <= 0)
    {
      isOn = false; 
      pe->isLooping = false;
    }
    else  {
      timer -= dt;
    }

  }
  
}

void WaterContainer::OnDestroy()
{

}

void WaterContainer::Serialize(Serializer& sr, LogicHandle idx)
{

 
  sr.Write("targetName", targetObj);
  sr.Write("MissionType", missionType);
  ScriptCommonIF::Serialize(sr, idx);


}
void WaterContainer::Deserialize(Deserializer& d)
{
    //std::vector<std::string> names;

    d.Read("targetName", targetObj);
    d.Read("MissionType", missionType);

   // SetWayPoints();
    //listOfWaypoint.resize(sizeOfWaypoint);
   // listOfWaypointTempName.resize(sizeOfWaypoint);
    ScriptCommonIF::Deserialize(d);
  
}
void WaterContainer::Reflect(const Reflector& r)
{
    //int tmpWaypointSize = sizeOfWaypoint;
    r.Reflect("targetName", targetObj);
    ImGui::SameLine();
    ImGui::Text(missionNames[missionType].c_str());
    if (r.Reflect("MissionType", missionNames, &index1))
    {
      missionType = index1;
    }
    ScriptCommonIF::Reflect(r);
}

void WaterContainer::OnCollisionEnter(GOHandle other)
{
  switch (missionType)
  {

  case 0: // none
  {
    break;
  }

  case 1://sink
  {
    //if has been plug else we wont allow you to collect the water 
    if (activate)
    {
      if (t->scale.y >= 1.0f)
      {
        //sink is looking to fill water bucket
        WaterContainer* bucket = ASC_GETSCRIPT_PTR(WaterContainer, other);

        //hit the player 
        if (bucket != nullptr)
        {
          bucket->CollectWater();
        
        }
      }
    }
    else
    {
      std::cout << " cant collect\n";
    }
    break;
  }
  case 2://water bucket
  {
    //looking for oven to dumb water on to 
    //WaterContainer* sink = ASC_GETSCRIPT_PTR(WaterContainer, other);

    ////hit the player 
    //if (sink != nullptr)
    //{
    //  revertDirection = !revertDirection;
    //  changeDirection = true;
    //}

    break;
  }
    

  }
}
void WaterContainer::OnCollisionStay(GOHandle other)
{

}
void WaterContainer::OnCollisionExit(GOHandle other)
{

}

void WaterContainer::CollectWater()
{
  if (!hasWater)
  {
    hasWater = true;
    ASC_PLAY("SFX_ScoopWater.wav");
    ChildIterate ci = ASC_TRANSMAN.GetChildIter(t->parentinghdl);

    while (ci.Next().IsValid())
    {
      GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());

      nextChild->GetHdl<ParticleEmitter>()->Emit();
      std::cout << "pour\n";
    }
  }


  if (pe.IsValid())
  {
    //std::cout << GetGOHandle()->name << " : " << hasWater << std::endl;
   //td::cout << " water has collected\n";
  //  pe->isLooping = true;
  }
}
void WaterContainer::DumbWater()
{
  hasWater = false;

  std::cout << GetGOHandle()->name << " :";

    ChildIterate ci = ASC_TRANSMAN.GetChildIter(t->parentinghdl);

    while (ci.Next().IsValid())
    {
      GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());
     
      nextChild->GetHdl<ParticleEmitter>()->Emit();
      std::cout << "pour\n";
    }
 
 
}

void WaterContainer::HandleMessage(MsgData_Base* m)
{

}