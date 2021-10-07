#include "pch.h"
SwivelInteract::SwivelInteract()
{
	ASC_SETUP_SCRIPT(SwivelInteract);
}
void SwivelInteract::InteractAction()
{
	if (!usedMe)
	{
		p->Swivel = true;
		p->canMove = false;
		usedMe = true;
	}
		//std::cout << goHdl->name << std::endl;
}
void SwivelInteract::Initialize()
{
	if (activateObject != "")
	{
		target = ASC_FACTORY.GetGameObjectByName(activateObject.c_str());
	}

}
void SwivelInteract::OnStart()
{
  t->dirty = true;
	player = ASC_FACTORY.GetObjectHandleList(go_player)[0];
	p = ASC_GETSCRIPT_PTR(PlayerController, player);
	Rotate = false;
	OrigPos = t->pos;
	target = ASC_FACTORY.GetGameObjectByName(activateObject.c_str());
	if (target.IsValid())
	{
		activateScript = ASC_GETSCRIPT_INTERACT(target);
		activateScript->activated = false;
	}
}
Quaternion SLERP(const Quaternion& a, const Quaternion& b, const float t)
{
	Quaternion r;
	float t_ = 1 - t;
	float Wa, Wb;
	float theta = acos(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
	float sn = sin(theta);
	Wa = sin(t_ * theta) / sn;
	Wb = sin(t * theta) / sn;
	r.x = Wa * a.x + Wb * b.x;
	r.y = Wa * a.y + Wb * b.y;
	r.z = Wa * a.z + Wb * b.z;
	r.w = Wa * a.w + Wb * b.w;
	r.Normalize();
	return r;
}
void SwivelInteract::Update(float dt)
{
	t->pos = OrigPos;
		if(usedMe)
		{
			
        t->dirty = true;
				if (p->currDelta.MagnitudeSq() > 0.5f && !Rotate)
				{
					Rotate = true;
					RotationMag = p->currDelta.MagnitudeSq();
					p->Swivel = false;
					p->canMove = true;
					difference = ResultRot * OrigRot.Inverse();
					time = 1.05f;
					ResultRot = ResultRot.Normalize();
					OrigRot = OrigRot.Normalize();
					std::cout << goHdl->name << std::endl;


				}


				if (Rotate)
				{
					if (prompt)
					{
						p->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
					}

					if (InteractTriggerSound != "")
					{
						if (!ASC_SOUNDMAN.IsPlaying(s, InteractTriggerSound.c_str()))
							ASC_PLAY(InteractTriggerSound.c_str());
					}
					t->pos = OrigPos;
					t->rotation = SLERP(ResultRot, OrigRot, time);
					time -= dt;


				}
				else
				{
					if (prompt)
					{
						if (ASC_ENGINE.GetUsingMouse())
							p->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_Swivel_albedo";
						else
							p->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_Controller_Swivel_albedo";
						p->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(1.0f);
					}
				}
				if (time < 0.0)
				{
					Rotate = false;

					np->obj_phy->SetRigidBodyEnable(false);
					np->obj_phy->GeomCollisionSetEnable(false);
					//np->obj_phy->BodySetKinematic();
					np->obj_phy->SetHaveBody(false);

					if (target.IsValid())
					{
						activateScript->activated = true;
					}
					usedMe = false;
					activated = false;
				}


	}

}

void SwivelInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Range", range);
	sr.Write("Filename", path);
	sr.Write("ResultRot", Vec3(ResultRot.x, ResultRot.y, ResultRot.z));
	sr.Write("ResultQuat", ResultRot.w);
	sr.Write("OrigRot", Vec3(OrigRot.x, OrigRot.y, OrigRot.z));
	sr.Write("OrigQuat", OrigRot.w);
	sr.Write("activateObject", activateObject);
	//sr.Write("Hint", hint);
  Interact::Serialize(sr, idx);
}

void SwivelInteract::Deserialize(Deserializer& d)
{

	d.Read("Range", range);
	d.Read("Filename", path);
	RefreshHintFile();
	Vec3 Quat;
	float w;
	d.Read("ResultRot", Quat);
	d.Read("ResultQuat",w);
	ResultRot.x = Quat.x;
	ResultRot.y = Quat.y;
	ResultRot.z = Quat.z;
	ResultRot.w = w;
	d.Read("OrigRot", Quat);
	d.Read("OrigQuat", w);
	OrigRot.x = Quat.x;
	OrigRot.y = Quat.y;
	OrigRot.z = Quat.z;
	OrigRot.w = w;
	d.Read("activateObject", activateObject);
  Interact::Deserialize(d);
}
void SwivelInteract::RefreshHintFile()
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
void SwivelInteract::Reflect(const Reflector& r)
{
	ImGui::InputFloat("Range", &range);
	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}
	if (ImGui::Button("Capture Original Quaternion", ImVec2(200, 50)))
	{
		OrigRot = t->rotation;
	}
	if (ImGui::Button("Capture Result Quaternion", ImVec2(200, 50)))
	{
		ResultRot = t->rotation;
	}
	if (ImGui::Button("Set Original Quaternion", ImVec2(200, 50)))
	{
		t->rotation = OrigRot;
	}
	if (ImGui::Button("Set Result Quaternion", ImVec2(200, 50)))
	{
		t->rotation = ResultRot;
	}
	ImGui::InputText("ActivateObject", &activateObject[0], 256);
	if (ImGui::Button("Set Activate object", ImVec2(200, 30)))
	{
		target = ASC_FACTORY.GetGameObjectByName(activateObject.c_str());
	}
	if (target.IsValid())
	{
		ImGui::Text(&target->name[0]);
	}
  Interact::Reflect(r);
}

void SwivelInteract::OnDestroy()
{

}

void SwivelInteract::OnCollisionEnter(GOHandle other)
{
}

void SwivelInteract::OnCollisionStay(GOHandle other)
{

}

void SwivelInteract::OnCollisionExit(GOHandle other)
{
}

void SwivelInteract::HandleMessage(MsgData_Base* m)
{
}