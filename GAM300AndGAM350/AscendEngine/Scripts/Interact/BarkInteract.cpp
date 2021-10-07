#include "pch.h"

BarkInteract::BarkInteract()
{
	ASC_SETUP_SCRIPT(BarkInteract);
}
void BarkInteract::InteractAction()
{
	Interact* i = ASC_GETSCRIPT_INTERACT(TargetInteract);
	i->activated = true;
}
void BarkInteract::SetupTarget(std::string name)
{
	if (name != "")
	{
		TargetInteract = ASC_FACTORY.GetGameObjectByName(name);
		if (TargetInteract.IsValid())
		{
			Interact* i = ASC_GETSCRIPT_INTERACT(TargetInteract);
			i->activated = false;
			activated = false;
		}
	}
}
void BarkInteract::OnStart()
{


}

void BarkInteract::Update(float dt)
{
	if (!ASC_SOUNDMAN.IsPlaying(s, "SFX_Whine.ogg"))
	{
		ASC_PLAY("SFX_Whine.ogg");
	}



}
void BarkInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Filename", path);
	if(TargetInteract.IsValid())
	sr.Write("Target", TargetInteract->name);
	sr.Write("Activated", activated);
	sr.Write("Range", range);
  Interact::Serialize(sr, idx);
}

void BarkInteract::Deserialize(Deserializer& d)
{
	d.Read("Filename", path);
	std::string name = "";
	d.Read("Target", name);
	SetupTarget(name);
	d.Read("Activated", activated);
	d.Read("Range", range);
	RefreshHintFile();

  Interact::Deserialize(d);
}
void BarkInteract::RefreshHintFile()
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
void BarkInteract::Reflect(const Reflector& r)
{
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
	if (ImGui::Button("Set Target"))
	{
		TargetInteract = ASC_FACTORY.GetGameObjectByName(selected_obj);
	}

	interactObjects.clear();
	if (TargetInteract.IsValid())
	{
		ImGui::Text(&TargetInteract->name[0]);
	}

	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}

	ImGui::InputFloat("Range", &range);

  Interact::Reflect(r);
}

void BarkInteract::OnDestroy()
{

}

void BarkInteract::OnCollisionEnter(GOHandle other)
{
}

void BarkInteract::OnCollisionStay(GOHandle other)
{

}

void BarkInteract::OnCollisionExit(GOHandle other)
{
}

void BarkInteract::HandleMessage(MsgData_Base* m)
{
}