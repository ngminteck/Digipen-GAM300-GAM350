#include "pch.h"

PuzzleKey::PuzzleKey() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(PuzzleKey))
{

}
PuzzleKey::~PuzzleKey() {}
void PuzzleKey::OnStart()
{

	Initialize();
}
void PuzzleKey::Initialize()
{
	if (ParentPuzzleObject.IsValid())
	{
		PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, ParentPuzzleObject);
		if (puz != nullptr)
		{
			myIndex = puz->GetKeyIndex(goHdl);
		}
		DetectScript();
		UpdateVariables();
	}

}
void PuzzleKey::Update(float dt)
{
	if (!ParentPuzzleObject.IsValid()||in.size()==0)
		Initialize();
	if(use == -1)
		DetectScript();
	if (in.size() > 0 && in[use]->usedMe)
	{
		if (use >= 0 && use < Interacts::DoNotPassThis)
		{
			PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, ParentPuzzleObject);
			switch (use)
			{
			case 0:
				if (static_cast<MoveInteract*>(in[use])->StaticObject)
				{
					if (static_cast<MoveInteract*>(in[use])->Distance == static_cast<MoveInteract*>(in[use])->CurrDistance)
					{
						puz->InputKey(myIndex, true);
					}
					else
					{
						puz->InputKey(myIndex, false);
					}
				}
				break;
			
			default:
				break;
			}

		}
	}


}
void PuzzleKey::OnDestroy() {}

void PuzzleKey::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("use", use);
	sr.Write("localPos", localPos);
	sr.Write("OneTime", oneTime);
}
void PuzzleKey::Deserialize(Deserializer& d)
{
	d.Read("use", use);
	d.Read("localPos", localPos);
	d.Read("OneTime", oneTime);

}
void PuzzleKey::Reflect(const Reflector& r)
{
	ImGui::Text("0 - MoveInteract\n1 - PuppyInteract\n2 - PickupInteract\n3 - ItemInteract\n4 - DoorInteract\n5 - ActivateInteract");
	ImGui::Text("If not specified, put -1");
	ImGui::InputInt("Type to Use", &use);
	ImGui::Text("Detect Script will detect the first Interact script");
	if(ImGui::Button("Detect Interact Script", ImVec2(300, 30)))
	{
		DetectScript();
		
	}
	if (use != 0)
	{
		ImGui::Text("Key will override the Interact script's variables");
	}


	if (ParentPuzzleObject.IsValid())
	{
		PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, ParentPuzzleObject);
		switch (use)
		{
		case -1:
			break;
			//MoveInteract
		case 0:
			ImGui::Checkbox("Static Object", &check);
			if (check)
			{
				ImGui::Checkbox("Forward", &check2);
				if (check2)
					ImGui::SliderFloat("Distance", &var, 0.0f, 100.0f);
				else
					ImGui::SliderFloat("Distance", &var, -100.0f, 0.0f);
			}
			break;
			//PuppyInteract
		case 1:
			break;
			//PickupInteract
		case 2:
			ImGui::Checkbox("OneTime?", &oneTime);
			//static_cast<PickupInteract*>(in[use])->placeLoc = puz->Objects[puz->GetKeyIndex(goHdl)].posToBe;
			break;
			//ItemInteract
		case 3:
			break;
		default:
			break;
		}
		if (in.size())
		{
			UpdateVariables();
		}
		else
		{
			DetectScript();
		}
	}

	ImGui::SliderFloat("LocalPos.x", &localPos.x, -100, 100);
	ImGui::SliderFloat("LocalPos.y", &localPos.y, -100, 100);
	ImGui::SliderFloat("LocalPos.z", &localPos.z, -100, 100);
}
void PuzzleKey::HandleMessage(MsgData_Base* m) {}
bool PuzzleKey::DetectScript(int ignore)
{
	in.clear();
	use = -1;
	MoveInteract* mI = ASC_GETSCRIPT_PTR(MoveInteract, goHdl);
	PuppyInteract* pI = ASC_GETSCRIPT_PTR(PuppyInteract, goHdl);
	PickupInteract* pkI = ASC_GETSCRIPT_PTR(PickupInteract, goHdl);
	ItemInteract* iI = ASC_GETSCRIPT_PTR(ItemInteract, goHdl);
	DoorInteract* dI = ASC_GETSCRIPT_PTR(DoorInteract, goHdl);
	ActivateInteract* aI = ASC_GETSCRIPT_PTR(ActivateInteract, goHdl);
	in.push_back(mI);
	in.push_back(pI);
	in.push_back(pkI);
	in.push_back(iI);
	in.push_back(dI);
	in.push_back(aI);
	if (ignore != -1)
	{
		in[ignore]->activated = false;
	}
	if (use == -1)
	{
		for (int i = 0; i < Interacts::DoNotPassThis; i++)
		{
			if (in[i] != nullptr)
			{
				if (i != ignore)
				{
					use = i;
					in[i]->activated = true;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < Interacts::DoNotPassThis; i++)
		{
			if (in[i] != nullptr)
			{
				if (i == use)
				{
					in[i]->activated = true;
				}
				else
					in[i]->activated = false;
			}
		}
	}
	
	return use == -1 ? false : true;
}

void PuzzleKey::UpdateVariables()
{
	PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, ParentPuzzleObject);
	if (in.size() == 0)
	{
		//DetectScript();
		if (myIndex == -1)
		{
			myIndex = puz->GetKeyIndex(goHdl);
		}
	}
	
	switch (use)
	{
		//MoveInteract
	case 0:
		if (static_cast<MoveInteract*>(in[use])->StaticObject)
		{
			static_cast<MoveInteract*>(in[use])->Distance = var;
		}
		break;
		//PuppyInteract
	case 1:
		break;
		//PickupInteract
	case 2:
		static_cast<PickupInteract*>(in[use])->placeLoc = puz->Objects[myIndex].posToBe;
		static_cast<PickupInteract*>(in[use])->placeRot = puz->Objects[myIndex].rotToBe;
			break;
		//ItemInteract
	case 3:

		break;
	default:
		break;
	}
	

}
void  PuzzleKey::HandleKeyboard(float) {}
void PuzzleKey::HandleController(float) {}