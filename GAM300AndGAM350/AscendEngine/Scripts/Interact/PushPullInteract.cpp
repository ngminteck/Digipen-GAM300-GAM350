#include "pch.h"


/* 





 NOT USE 















*/









PushPullInteract::PushPullInteract()
{
	ASC_SETUP_SCRIPT(PushPullInteract);
}
void PushPullInteract::InteractAction()
{
	Interact* i = ASC_GETSCRIPT_INTERACT(TargetInteract);
	i->activated = true;
}
void PushPullInteract::SetupTarget(std::string name)
{
	if (name != "")
	{
		TargetInteract = ASC_FACTORY.GetGameObjectByName(name);
		if (TargetInteract.IsValid())
		{
			Interact* i = ASC_GETSCRIPT_INTERACT(TargetInteract);
			i->activated = false;
		}
	}
}
void PushPullInteract::OnStart()
{


}

void PushPullInteract::Update(float dt)
{
  std::cout << "test\n";



}
void PushPullInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Filename", path);
	if(TargetInteract.IsValid())
	sr.Write("Target", TargetInteract->name);
	sr.Write("Activated", activated);

}

void PushPullInteract::Deserialize(Deserializer& d)
{
	d.Read("Filename", path);
	std::string name = "";
	d.Read("Target", name);
	SetupTarget(name);
	d.Read("Activated", activated);
	RefreshHintFile();
}
void PushPullInteract::RefreshHintFile()
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
void PushPullInteract::Reflect(const Reflector& r)
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

}

void PushPullInteract::OnDestroy()
{

}

void PushPullInteract::OnCollisionEnter(GOHandle other)
{
}

void PushPullInteract::OnCollisionStay(GOHandle other)
{

}

void PushPullInteract::OnCollisionExit(GOHandle other)
{
}

void PushPullInteract::HandleMessage(MsgData_Base* m)
{
}