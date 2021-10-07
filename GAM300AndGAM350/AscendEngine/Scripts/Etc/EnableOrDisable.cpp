#include "pch.h"

std::vector< const char* > EnableDiasbleType
{
  "NONE",
  "PLAY_SOUND",
};
enum  TYPE_OF_BEHAVIOUR
{
  NONE,
  PLAY_SOUND,
};

EnableOrDisable::EnableOrDisable()
 : ScriptCommonIF(ASC_GETSCRIPTNAME(EnableOrDisable)), 
    checkWaypointOffset(0.7f),
    waypointIndex(0),
    revertDirection(false)
{
}
 int index2 = 0; 
EnableOrDisable::~EnableOrDisable()
{
}

void EnableOrDisable::OnStart()
{

}
void EnableOrDisable::SwitchTypeBehaviour()
{
  auto go = GetGOHandle();
  
  switch (type )
  {
  case PLAY_SOUND:
  {
    //auto test = ASC_FACTORY;

    go->AddComponent(SOUND, go);
  //  m_factory->AddComponent(*m_selected, GRAPHICS, PHYSICS);
    break;
  }
  case NONE:
  {
    break;
  }

  }
}
void EnableOrDisable::Update(float dt)
{
  switch (type)
  {
  case PLAY_SOUND:
  {
    if (!ASC_SOUNDMAN.IsPlaying(s, stringVal.c_str()))
      ASC_PLAY(stringVal.c_str());
    break;
  }
  case NONE:
  {
    break;
  }

  }
}

void EnableOrDisable::OnDestroy()
{

}

void EnableOrDisable::Serialize(Serializer& sr, LogicHandle idx)
{
  sr.Write("willActivate",willActivate );
  sr.Write("willDeActivate", willDeActivate);

}
void EnableOrDisable::Deserialize(Deserializer& d)
{
  d.Read("willActivate", willActivate);
  d.Read("willDeActivate",willDeActivate );
}
void EnableOrDisable::Reflect(const Reflector& r)
{
  r.Reflect("willActivate", willActivate);
  r.Reflect("willDeActivate", willDeActivate);


  if (ImGui::Combo("##TriggerStuff_2", &index2, EnableDiasbleType.data(), (uint32_t)EnableDiasbleType.size()))
  {
    type = index2;
    SwitchTypeBehaviour();
  }
  ImGui::Text(std::to_string(type).c_str());
  ImGui::Separator();

}

void EnableOrDisable::OnCollisionEnter(GOHandle other)
{

}
void EnableOrDisable::OnCollisionStay(GOHandle other)
{

}
void EnableOrDisable::OnCollisionExit(GOHandle other)
{

}

void EnableOrDisable::HandleMessage(MsgData_Base* m)
{

}