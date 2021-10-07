#include "pch.h"

PuzzleKeyHole::PuzzleKeyHole() : ScriptCommonIF(ASC_GETSCRIPTNAME(PuzzleKeyHole))
{

}

PuzzleKeyHole::~PuzzleKeyHole()
{

}

void PuzzleKeyHole::OnStart()
{

}
void PuzzleKeyHole::Update(float dt)
{
	if (!parentPuzzle.IsValid())
	{
		Init();
		
	}
	else
	{

	}

}
void PuzzleKeyHole::Init()
{
	//Init parent
	parentPuzzle = ASC_FACTORY.GetGameObjectByName(parPuzName);
	//Init Key
	ExpectedKey = ASC_FACTORY.GetGameObjectByName(ExpectedKeyName);

	if (parentPuzzle.IsValid() && ExpectedKey.IsValid())
	{
		PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, parentPuzzle);
		PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, ExpectedKey);
		if (puz != nullptr && pz!=nullptr)
		{
			Keypos = puz->Objects[pz->myIndex].posToBe;
			KeyRot = puz->Objects[pz->myIndex].rotToBe;

		}
	}

}
void PuzzleKeyHole::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("ExpectedKeyName", ExpectedKeyName);
	sr.Write("PuzzleParent", parPuzName);

}

void PuzzleKeyHole::Deserialize(Deserializer& d)
{
	d.Read("ExpectedKeyName", ExpectedKeyName);
	d.Read("PuzzleParent", parPuzName);
}
void PuzzleKeyHole::Reflect(const Reflector& r)
{
	ImGui::InputText("Parent Puzzle Object", &parPuzName[0], 128);
	ImGui::InputText("Expected Puzzle Key", &ExpectedKeyName[0], 128);
}
void PuzzleKeyHole::GetKeyLocation()
{


}
void PuzzleKeyHole::GetKeyRotation()
{


}
void PuzzleKeyHole::OnDestroy()
{

}

void PuzzleKeyHole::HandleMessage(MsgData_Base* m)
{
}