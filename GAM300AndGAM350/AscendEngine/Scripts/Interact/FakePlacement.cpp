#include "pch.h"

FakePlacement::FakePlacement() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(FakePlacement))
{}
FakePlacement::~FakePlacement()
{}
void FakePlacement::OnStart()
{
	fakeObject = ASC_FACTORY.GetGameObjectByName("PlacementObj");
	fakeObject->GetHdl<GraphicsComp>()->SetTransparencyVal(0);
	correctObject = ASC_FACTORY.GetGameObjectByName(correctObjectName);
	placeText = ASC_FACTORY.GetGameObjectByName("FakeText");
	dialogBG = ASC_FACTORY.GetGameObjectByName("dialogBg");
	show = false;

}
void FakePlacement::Update(float dt)
{
	if (show)
	{
		placeText->GetHdl<UITextComp>()->Enabled = true;
		time -= dt;
		float ratio = time / 2.0f;
		placeText->GetHdl<UITextComp>()->tint.w = ratio;
		dialogBG->GetHdl<GraphicsComp>()->tint.w = ratio;
		if (ratio <= 0.0f)
		{
			show = false;
			placeText->GetHdl<UITextComp>()->Enabled = false;

			dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
		}
	}
}

void FakePlacement::ShowText(std::string s)
{
	show = true;
	time = 2.0f;
	placeText->GetHdl<UITextComp>()->offset.x = -(s.length() * 0.047f);
	placeText->GetHdl<UITextComp>()->SetText(s);
	dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
}
void FakePlacement::OnDestroy()
{}
void FakePlacement::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("CorrectObject", correctObjectName);

  ScriptCommonIF::Serialize(sr, idx);
}
void FakePlacement::Deserialize(Deserializer& d)
{
	d.Read("CorrectObject", correctObjectName);
  ScriptCommonIF::Deserialize(d);
}
void FakePlacement::Reflect(const Reflector& r)
{
	ImGui::InputText("Object Name", &temp[0], 128);
	if (ImGui::Button("Set Correct Object", ImVec2(300, 30)))
	{
		correctObject = ASC_FACTORY.GetGameObjectByName(temp.c_str());
		if (correctObject.IsValid())
		{
			correctObjectName = correctObject->name;
		}
	}
	if (correctObject.IsValid())
	{
		ImGui::Text(correctObject->name.c_str());
	}
  ScriptCommonIF::Reflect(r);
	
}
void FakePlacement::HandleMessage(MsgData_Base* m)
{}
void FakePlacement::OnCollisionEnter(GOHandle other)
{
	PickupInteract* pp = ASC_GETSCRIPT_PTR(PickupInteract, other);
	//std::cout << other->name << std::endl;
	if(pp!=nullptr && correctObject.IsValid())
	{

		PickupInteract * pc  = ASC_GETSCRIPT_PTR(PickupInteract, correctObject);
		fakeObject->GetHdl<GraphicsComp>()->modelfile = other->GetHdl<GraphicsComp>()->modelfile;
		
		if (other != correctObject)
		{
			fakeObject->GetHdl<GraphicsComp>()->SetColor3(Vec4(1, 0, 0, alpha));
		}
		else
		{
			fakeObject->GetHdl<GraphicsComp>()->SetColor3(Vec4(0, 1, 0, alpha));

		}
		fakeObject->GetHdl<GraphicsComp>()->SetTransparencyVal(alpha);
		//fakeObject->GetHdl<GraphicsComp>()->dirty
		fakeObject->GetHdl<TransformComp>()->pos = pc->placeLoc;
		fakeObject->GetHdl<TransformComp>()->rotation = EulerToQuaternion(pc->placeRot);
		fakeObject->GetHdl<TransformComp>()->dirty = true;
	}
}
void FakePlacement::OnCollisionExit(GOHandle other)
{
	fakeObject->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
}