#include "pch.h"

PuzzleLock::PuzzleLock() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(PuzzleLock))
{}
PuzzleLock::~PuzzleLock() {}
void PuzzleLock::OnStart()
{
	ReadfromKeysFile();
	AssignAllObjects();
	g->Enabled = true;
	maxVal = 0;
	for (int i = 0; i < Order.size(); i++)
	{
		maxVal += Order[i];
	}
}

void PuzzleLock::Initialize()
{
	ReadfromKeysFile();
	AssignAllObjects();
}
void PuzzleLock::Update(float dt) 
{
	if (!unlocked)
	{

		if (CheckAllReady())
		{
			if (!useOrdering)
			{
				if (CheckCombination())
				{
					if (UnlockSound != "")
					{
						ASC_PLAY(UnlockSound.c_str());
					}
					std::cout << "unlocked" << std::endl;
					unlocked = true;
					Director* d = ASC_GETSCRIPT_PTR(Director, goHdl);
					for (int i = 0; i < Objects.size(); i++)
					{

						PlayerController* pp = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
						if (pp->lastInteract.IsValid())
						{

							Interact* inter = ASC_GETSCRIPT_INTERACT(pp->lastInteract);
							if (inter != nullptr)
							{
								inter->InteractAction();
								inter->activated = false;
							}
						}
						PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, Objects[i].obj);
						if (pz->inKeyHole)
						{
							ASC_TRANSMAN.AttachChild(t, Objects[i].obj->GetHdl<TransformComp>());
							Objects[i].obj->GetHdl<TransformComp>()->localpos = pz->localPos;
							Objects[i].obj->GetHdl<TransformComp>()->localrotation = Objects[i].obj->GetHdl<TransformComp>()->rotation;
							Objects[i].obj->GetHdl<TransformComp>()->localscale = Objects[i].obj->GetHdl<TransformComp>()->scale;
							Interact* is = ASC_GETSCRIPT_INTERACT(Objects[i].obj);
							is->activated = false;
						}
					}
					if (d != nullptr)
					{
						d->direct = true;
					}
				}
			}
			else
			{
			//	if (CheckOrder())
			//	{


			//	}
			}
			//Do something
		}
	}
	if (useOrdering)
	{
		if (wrongTime > 0.0f)
		{
			wrongTime -= dt;
			for (int i = 0; i < Objects.size(); i++)
			{
				if(Objects[i].order !=-2)
				Objects[i].obj->GetHdl<GraphicsComp>()->SetColor3(Vec4(1.0f, 0.0f, 0.0f, 1.0f));
				Objects[i].obj->GetHdl<GraphicsComp>()->isHighlighted = false;
				Interact* in = ASC_GETSCRIPT_INTERACT(Objects[i].obj)
				if(in!=nullptr)
				{
					in->activated = false;
				}
			}

		}
		else if(wrongTime <0.0 && wrongTime != -100.0f)
		{
			for (int i = 0; i < Objects.size(); i++)
			{
				Objects[i].obj->GetHdl<GraphicsComp>()->SetColor3(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
				ItemInteract* in = ASC_GETSCRIPT_PTR(ItemInteract,Objects[i].obj)
					if (in != nullptr)
					{
						in->activated = true;
					}
			}
			wrongTime = -100.0f;
		}
	}
	g->SetTransparencyVal(255);
}
void PuzzleLock::OnDestroy() {}
void PuzzleLock::AssignAllObjects()
{
	if (Objects.size())
	{
		for (int i = 0; i < Objects.size(); i++)
		{
			if (Objects[i].obj.IsValid())
			{
				PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, Objects[i].obj);
				if (pz)
				{
					pz->ParentPuzzleObject = goHdl;
				}
				else
				{
					std::cout << "Object has no PuzzleKey" << std::endl;
				}
			}
		}
	}
}
void PuzzleLock::Serialize(Serializer& sr, LogicHandle idx) 
{
	sr.Write(GetName());
	sr.Write("useInPlace", useInplace);
	sr.Write("useOrdering", useOrdering);
	sr.Write("maxVal", maxVal);
	sr.Write("OrderCorrectSound", PuzzleOrderCorrect);
	sr.Write("OrderWrongSound", PuzzleOrderWrong);
	sr.Write("UnlockSound", UnlockSound);
}
void PuzzleLock::Deserialize(Deserializer& d) 
{
	d.Read("useOrdering", useOrdering);


	d.Read("useInPlace", useInplace);
	d.Read("maxVal", maxVal);
	d.Read("OrderCorrectSound", PuzzleOrderCorrect);
	d.Read("OrderWrongSound", PuzzleOrderWrong);
	d.Read("UnlockSound", UnlockSound);
}
void PuzzleLock::Reflect(const Reflector& r) 
{
	ImGui::InputText("OrderCorrectSound", &PuzzleOrderCorrect[0], 256);
	ImGui::InputText("OrderWrongSound", &PuzzleOrderWrong[0], 256);
	ImGui::InputText("UnlockSound", &UnlockSound[0], 256);
	static std::string ObjN;
	if (useOrdering && Order.size() != Objects.size())
	{
		for (int i = 0; i < Objects.size(); i++)
		{
			Order.push_back(0);
		}
	}
	if(ImGui::Button("Add new puzzle Component", ImVec2(200, 30)))
	{
		Objects.push_back(Key());
		Combination.push_back(bool());
		Order.push_back(0);
	}
	if (Objects.size() > 0)
	{
		ImGui::SliderInt("Index", &index, 0, static_cast<int>(Objects.size())-1);
		if(Objects[index].obj.IsValid())
			ObjN = Objects[index].obj->name;
		ImGui::InputText("Name of Object", &ObjN[0], 128);
		if (ImGui::Button("Set Objective Position and Set GameObject", ImVec2(300, 30)))
		{
			Objects[index].obj = ASC_FACTORY.GetGameObjectByName(ObjN.c_str());
			if (Objects[index].obj.IsValid())
			{
				Objects[index].posToBe = Objects[index].obj->GetHdl<TransformComp>()->pos;
				Objects[index].rotToBe = QuaternionToEuler(Objects[index].obj->GetHdl<TransformComp>()->rotation);
				if (useOrdering)
					Order[index] = Objects[index].order;
			}
			else
			{
				ImGui::Text("Object does not exist, check the name");
			}

		}
		if (Objects[index].obj.IsValid())
		{
			std::string text = std::to_string(Objects[index].posToBe.x) + " " + std::to_string(Objects[index].posToBe.y) + " " + std::to_string(Objects[index].posToBe.z);
			std::string rot = std::to_string(Objects[index].rotToBe.x) + " " + std::to_string(Objects[index].rotToBe.y) + " " + std::to_string(Objects[index].rotToBe.z);
			ImGui::Text(&text[0]);
			ImGui::Text(&rot[0]);
		}
	

		ImGui::Checkbox("Is In Pos", &Objects[index].inplace);
		if (useOrdering)
		{
			ImGui::Text("Order To Unlock (UNIQUE NUMBERS ONLY)");
			for (int i = 0; i < Objects.size(); i++)
			{
			
				if (Objects[i].obj.IsValid())
					ImGui::InputInt(Objects[i].obj->name.c_str(), &Objects[i].order);
				Order[i] = Objects[i].order;
			}
		}
		else
		{
			ImGui::Text("Combination To Unlock");
			for (int i = 0; i < Objects.size(); i++)
			{
				bool b = Combination[i];
				if (Objects[i].obj.IsValid())
					ImGui::Checkbox(Objects[i].obj->name.c_str(), &b);
				Combination[i] = b;
			}
		}
		if (ImGui::Button("Save All Keys to File", ImVec2(300, 30)))
		{
			WritetoKeysFile();
		}


	}

	if (ImGui::Button("Read Keys File", ImVec2(300, 30)))
	{
		ReadfromKeysFile();
	}
	if (ImGui::Button("Link All Keys", ImVec2(300,30)))
	{
		AssignAllObjects();
		//ASC_SCRIPTMAN.AddScript(Objects[index].obj->GetHdl<LogicComp>(), "SI_PuzzleKey");
	}
	

	ImGui::Checkbox("Use In place?", &useInplace);
	ImGui::Checkbox("Use ordering?", &useOrdering);
}
void PuzzleLock::HandleMessage(MsgData_Base* m) {}

void  PuzzleLock::HandleKeyboard(float) {}
void PuzzleLock::HandleController(float) {}

void PuzzleLock::WritetoKeysFile() 
{

	std::ofstream keysFile;
	keysFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".puz", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < Objects.size(); i++)
	{
		keysFile << Objects[i].obj->name << '\n';
		keysFile << Objects[i].posToBe.x << " " << Objects[i].posToBe.y << " " << Objects[i].posToBe.z << " " << '\n';
		keysFile << Objects[i].inplace << '\n';
		keysFile << Objects[i].rotToBe.x << " " << Objects[i].rotToBe.y << " " << Objects[i].rotToBe.z << " " << '\n';
		if(useOrdering)
			keysFile << Objects[i].order << '\n';

	}
	keysFile.close();
	if (!useOrdering)
	{
		keysFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".combi", std::ofstream::out | std::ofstream::trunc);
		for (int i = 0; i < Objects.size(); i++)
		{
			keysFile << Combination[i] << '\n';

		}
	}
	else
	{
		keysFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".ordering", std::ofstream::out | std::ofstream::trunc);
		for (int i = 0; i < Objects.size(); i++)
		{
			keysFile << Order[i] << '\n';

		}
	}
	keysFile.close();
}
void PuzzleLock::ReadfromKeysFile() 
{
	std::ifstream keysFile;
	std::string line;
	std::string objN;
	char delim = ' ';
	std::vector<float> vals;
	size_t pos;
	float token;
	int i = 0;
	index = 0;
	Objects.clear();
	Combination.clear();
	Order.clear();
	KeyInput.clear();
	std::string n = ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".puz";
	keysFile.open(n);
	if (keysFile.is_open())
	{
		// Get the position
		while (keysFile)
		{
			//Get Object name
			std::getline(keysFile, objN);
			std::getline(keysFile, line);
			vals.clear();
			//Get in place position
			while ((pos = line.find(delim)) != std::string::npos) {
				token = std::stof(line.substr(0, pos));
				vals.push_back(token);
				line.erase(0, pos + 1);
			}
			if (vals.size() > 0)
			{
				Vec3 point;
				point.x = vals[0];
				point.y = vals[1];
				point.z = vals[2];
				//if there is a position, object definitely exists
				Objects.push_back(Key());
				if (!useOrdering)
					Combination.push_back(bool());
				else
					Order.push_back(0);
				KeyInput.push_back(false);
				Objects[index].obj = ASC_FACTORY.GetGameObjectByName(objN.c_str());
				if (!Objects[index].obj.IsValid())
				{
					std::cout << "Cant find obj" << std::endl;
				}
				Objects[index].posToBe = point;
				std::getline(keysFile, line);
				bool b = stoi(line);
				Objects[index].inplace = b;

			}

			//Get rotation
			std::getline(keysFile, line);
			vals.clear();
			//Get in place position
			while ((pos = line.find(delim)) != std::string::npos) {
				token = std::stof(line.substr(0, pos));
				vals.push_back(token);
				line.erase(0, pos + 1);
			}
			if (vals.size() > 0)
			{
				Vec3 point;
				point.x = vals[0];
				point.y = vals[1];
				point.z = vals[2];
				//if there is a position, object definitely exists
				Objects[index].rotToBe = point;
				//Get the order
				if (useOrdering)
				{
					std::getline(keysFile, line);
					Objects[index].order = std::stoi(line);
				}
				index++;
			}



		}
		index = 0;
	}
	keysFile.close();

	if(useOrdering)
	{ 
		n = ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".ordering";
		keysFile.open(n);
		int pp = 0;
		while (keysFile)
		{
			line = "";
			std::getline(keysFile, line);
			if (line != "")
				Order[pp] = stoi(line);
			pp++;
		}
	}
	else
	{
		n = ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".combi";
		keysFile.open(n);
		int pp = 0;
		while (keysFile)
		{
			line = "";
			std::getline(keysFile, line);
			if (line != "")
				Combination[pp] = stoi(line);
			pp++;
		}
	}

}


bool PuzzleLock::InputKey(const GOHandle& g)
{
	for (int i = 0; i < Objects.size(); i++)
	{
		if (Objects[i].obj == g)
		{
			KeyInput[i] = !KeyInput[i];
		}
	}
	return true;


}
bool PuzzleLock::InputKey(int index , bool b)
{
	KeyInput[index] =b;

	return true;


}
bool PuzzleLock::CheckAllReady() {

	if (useInplace)
		return true;

	for (int i = 0; i < Objects.size(); i++)
	{
		if (!Objects[i].inplace)
		{
			return false;
		}
	}
	return true;

}
bool PuzzleLock::CheckCombination() {

	for (int i = 0; i < Combination.size(); i++)
	{
		if (!useInplace)
		{
			if (KeyInput[i] != Combination[i])
			{
				return false;
			}
		}
		else
		{
			if (Combination[i])
			{
				if (Objects[i].inplace != Combination[i])
				{
					return false;
				}
			}
		}
	}
	std::cout << "unlocked" << std::endl;
	return true;

}
bool PuzzleLock::CheckOrder()
{
	if (CheckAllReady())
	{
		int cur = 0;
		int val = 0;
		for (int i = 0; i < Order.size(); i++)
		{
			PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, Objects[i].obj);
			if (pz->orderInput != Objects[i].order && pz->orderInput != -1)
			{
				cur += pz->orderInput;
				if (Objects[i].order == -2)
				{
					continue;
				}
				ResetKeys();
				wrongTime = 2.0f;
				if (PuzzleOrderWrong != "")
				{
					ASC_PLAY(PuzzleOrderWrong.c_str());
				}
				return false;
			}
			else
			{
				if (pz->orderInput == Objects[i].order || Objects[i].order == -2)
					cur += Objects[i].order;
				val = pz->orderInput;
			}

		}
		if (val == -1)
		{
			return false;
		}
		if (cur != maxVal)
		{
			return false;
		}
		//If reached here, means all are correct
		for (int i = 0; i < Objects.size(); i++)
		{
			Interact* is = ASC_GETSCRIPT_INTERACT(Objects[i].obj);
			if (is != nullptr)
			{
				is->activated = false;
			}

		}
		if (PuzzleOrderCorrect != "")
		{
			ASC_PLAY(PuzzleOrderCorrect.c_str());
		}
		unlocked = true;
		//Trigger Camera
		CameraTrigger* ct = ASC_GETSCRIPT_PTR(CameraTrigger, goHdl);
		if (ct != nullptr)
		{
			ct->StartMovements();
		}
		//Trigger Director
		DirectorTrigger* dir = ASC_GETSCRIPT_PTR(DirectorTrigger, goHdl);
		if (dir != nullptr)
		{
			dir->StartDirector();
		}
		g->SetTransparencyVal(255.0f);
	}
	else
	{
		ResetKeys();
	}

}
void PuzzleLock::ResetKeys()
{
	for (int i = 0; i < Order.size(); i++)
	{
		PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, Objects[i].obj);
		pz->orderInput = -1;
		Objects[i].obj->GetHdl<GraphicsComp>()->SetColor3(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	orderToBeUsed = 0;
}
int PuzzleLock::GetKeyIndex(const GOHandle& g)
{
	for (int i = 0; i < Objects.size(); i++)
	{
		if (Objects[i].obj == g)
		{
			return i;
		}
	}
	return -1;
}