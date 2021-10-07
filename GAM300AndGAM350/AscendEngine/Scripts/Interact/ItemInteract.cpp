#include "pch.h"
ItemInteract::ItemInteract()
{
	ASC_SETUP_SCRIPT(ItemInteract);
}
void ItemInteract::InteractAction()
{
	if (InteractSoundOn != "")
	{
		ASC_PLAY(InteractSoundOn.c_str());
	}
	PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
	if (pz != nullptr)
	{
		PuzzleLock* pl = ASC_GETSCRIPT_PTR(PuzzleLock, pz->ParentPuzzleObject);

		if (pz->orderInput == -1)
		{
			pz->orderInput = pl->orderToBeUsed;
			pl->orderToBeUsed++;
			g->SetColor3(Vec4(0.0f, 1.0f, 0.0f, 1.0f));
			pl->CheckOrder();

		}
		else
		{

			pl->ResetKeys();
		}

	}

	//std::cout << "I am an item" << std::endl;
}
void ItemInteract::OnStart()
{}
void ItemInteract::Update(float dt)
{
	/*if (s.IsValid())
	{
		std::cout << ASC_SOUNDMAN.GetTrackLength("BGM_InGame.ogg") << std::endl;
	}*/
	/*goHdl->name;
	if (!activated)
		std::cout << "deactivated " << goHdl->name <<  std::endl;*/
}
void ItemInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Range", range);
	sr.Write("Filename", path);
	//sr.Write("Hint", hint);

  Interact::Serialize(sr, idx);
}

void ItemInteract::Deserialize(Deserializer& d)
{


	d.Read("Range", range);
	d.Read("Filename", path);
	RefreshHintFile();

  Interact::Deserialize(d);
}
void ItemInteract::RefreshHintFile()
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
void ItemInteract::Reflect(const Reflector& r)
{
	ImGui::InputFloat("Range", &range);
	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}

	//ImGui::Separator();
	//ImGui::Text("0 - NONE\n1 - LEFT\n2 - RIGHT\n3 - SWIPE\n4 - ALL(ONLY LEFT & RIGHT)\n");
	//ImGui::SliderInt("##prompt", &prompt, 0, Prompts::ALL);
  Interact::Reflect(r);

}

void ItemInteract::OnDestroy()
{

}

void ItemInteract::OnCollisionEnter(GOHandle other)
{
}

void ItemInteract::OnCollisionStay(GOHandle other)
{

}

void ItemInteract::OnCollisionExit(GOHandle other)
{
}

void ItemInteract::HandleMessage(MsgData_Base* m)
{
}