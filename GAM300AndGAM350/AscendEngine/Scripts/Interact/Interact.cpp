#include "pch.h"


Interact::Interact() :
    ScriptCommonIF(ASC_GETSCRIPTNAME(Interact))
{
 /* auto obj= this->GetGOHandle();
  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE,obj , GOTYPE::go_player));*/
}

void Interact::ForceRegStuff()
{
 
  auto obj= this->GetGOHandle();
  ASC_MSGSYS.SendMsg(Msg_Force_Reg_Collide(mTYPE_BY_GO_TYPE,obj , GOTYPE::go_player));
  if (linkToOtherMission)
  {
    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_LINK_TO_AS_ONE_MISSION, obj));
  }
  else
  {
    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_REG_TO_AS_ONE_MISSION, obj));
  }

}

void Interact::Reflect(const Reflector& r)
{

    ImGui::InputText("InteractOn", &InteractSoundOn[0], 256);
    ImGui::InputText("InteractOff", &InteractSoundOff[0], 256);
    ImGui::InputText("InteractTrigger", &InteractTriggerSound[0], 256);
  ImGui::Separator();
  ImGui::Text("Base Interact stuff");
  ImGui::Text("0 - NONE\n1 - LEFT\n2 - RIGHT\n3 - SWIPE\n4 - ALL(ONLY LEFT & RIGHT)\n");
  ImGui::SliderInt("##prompt", &prompt, 0, Prompts::ALL);


  ImGui::Separator();
  ImGui::Text("Base Script Stuff");
  ScriptCommonIF::Reflect(r);

}

void Interact::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write("Prompt", prompt);
  sr.Write("InteractOn", InteractSoundOn);
  sr.Write("InteractOff", InteractSoundOff);
  sr.Write("InteractTrigger", InteractTriggerSound);
  ScriptCommonIF::Serialize(sr, idx);
}
void Interact::Deserialize(Deserializer& d)
{
  d.Read("Prompt", prompt);
  d.Read("InteractOn", InteractSoundOn);
  d.Read("InteractOff", InteractSoundOff);
  d.Read("InteractTrigger", InteractTriggerSound);
  ScriptCommonIF::Deserialize(d);
}