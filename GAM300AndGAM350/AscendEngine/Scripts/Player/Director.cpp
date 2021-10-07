#include "pch.h"


Director::Director() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(Director))
{}
Director::~Director() {}

//Variables
void Director::OnStart() {
	direct = false;
	index = 0;
	time = 0;
	OrigRot = intQuat = t->rotation;
	OrigScale = intScale = t->scale;
	PositionToStart = intPos = t->pos;
	std::cout << goHdl->name << " is "  << g->Enabled << std::endl;
	origVis = g->Enabled;
	np->obj_phy->BodySetGravityEnable(false);
	ReadFromDirectorFile();
}
Quaternion QSLERP(const Quaternion& a, const Quaternion& b, const float t)
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
Vec3 LERP( Vec3 a, Vec3 b, float t) {
	float ax = a.x, ay = a.y, az = a.z;
	return Vec3(ax + t * (b.x - ax),
		ay + t * (b.y - ay),
		az + t * (b.z - az)
	);
}
void Director::Update(float dt)
{

	if (direct)
	{
		
		time += dt;
		if (actionList.size() > 0)
		{
			ratio = (time / actionList[index].time);
			switch (actionList[index].m)
			{
			case Translate:
				if (ratio > 1.0f)
				{
					index++;
					time = 0.0f;
					intPos = t->pos;
					np->obj_phy->GeomCollisionSetEnable(false);
					np->obj_phy->SetHaveBody(false);
					np->obj_phy->BodySetGravityEnable(false);
										PuppyInteract * p = ASC_GETSCRIPT_PTR(PuppyInteract, goHdl);
					if (p != nullptr)
					{
						p->OrigPos.x = t->pos.x;
						p->OrigPos.z = t->pos.z;
					}
					if (Pause)
					{
						np->obj_phy->SetHaveBody(false);
						direct = false;
					}
				}
				else
				{
					np->obj_phy->SetHaveBody(true);
					t->pos = LERP(intPos, actionList[index].Val, ratio);
					PuppyInteract * p = ASC_GETSCRIPT_PTR(PuppyInteract, goHdl);
					if (p != nullptr)
					{
						p->OrigPos.x = t->pos.x;
						p->OrigPos.z = t->pos.z;
					}
					t->dirty = true;
					np->obj_phy->GeomSetPosition(t->pos);

				}
				break;
			case Rotate:
				if (ratio > 1.0f)
				{
					index++;
					time = 0.0f;
					intQuat = t->rotation;
					if (Pause)
					{
						np->obj_phy->SetHaveBody(false);
						direct = false;
					}
				}
				else
				{
					t->rotation = QSLERP(intQuat, EulerToQuaternion(actionList[index].Val), ratio);
				}
				break;
			case Scale:
				if (ratio > 1.0f)
				{
					index++;
					time = 0.0f;
					intScale = t->scale;
					if (Pause)
					{
						np->obj_phy->SetHaveBody(false);
						direct = false;
					}
				}
				else
				{
					t->scale = LERP(intScale, actionList[index].Val, ratio);
				}
				break;
			default:
				break;
			}
			t->dirty = true;

			if (index == actionList.size())
			{


				np->obj_phy->GeomCollisionSetEnable(enableCollisionAfter);
				np->obj_phy->SetHaveBody(enableCollisionAfter);
				np->obj_phy->BodySetGravityEnable(false);
				np->obj_phy->GeomSetPosition(t->pos);
				//PuppyInteract* p = ASC_GETSCRIPT_PTR(PuppyInteract, goHdl);

				direct = false;
				g->Enabled = origVis;
				index = 0;
			}
		}
	}

}
void Director::OnDestroy() {}

void Director::Serialize(Serializer& sr, LogicHandle idx) {
	sr.Write(GetName());
	sr.Write("EnableCol", enableCollisionAfter);
}
void Director::Deserialize(Deserializer& d) {
	d.Read("EnableCol", enableCollisionAfter);
}
void Director::Reflect(const Reflector& r) {
	if (ImGui::Button("Set Starting Position/Rotation/Scale", ImVec2(300, 30)))
	{
		OrigRot = intQuat =  t->rotation;
		OrigScale = intScale = t->scale;
		PositionToStart = intPos =  t->pos;
	}

	/*ImGui::Text("Current Starting Values");
	std::string tt;
	tt ="Position: " + std::to_string(t->pos.x) + " " + std::to_string(t->pos.y) + " " + std::to_string(t->pos.z);
	ImGui::Text(&tt[0]);
	tt = "Rotation In Quaternion: " + std::to_string(t->rotation.x) + " " + std::to_string(t->rotation.y) + " " + std::to_string(t->rotation.z) + " " + std::to_string(t->rotation.w);
	ImGui::Text(&tt[0]);
	tt = "Scale: " + std::to_string(t->scale.x) + " " + std::to_string(t->scale.y) + " " + std::to_string(t->scale.z);
	ImGui::Text(&tt[0]);*/

	if(ImGui::Button("Add an action", ImVec2(300, 30)))
	{
		actionList.push_back(DirectorActions(0, Vec3::ZERO,0.0f));
		editorVal = 0;
	}
	if (actionList.size())
	{
		ImGui::SliderInt("Action" + index, &index, 0 , static_cast<int>(actionList.size())-1);
		ImGui::SliderInt("Type of Manipulation", &editorVal, 0, Manipulations::Scale);
		std::string text;
		Vec3 vec;
		//std::string t;
		switch (editorVal)
		{
		case 0:
			ImGui::Text("Translate object to\n");
			text = std::to_string(t->pos.x) + " " + std::to_string(t->pos.y) + " " + std::to_string(t->pos.z);	
			vec = t->pos;
			break;
		case 1:
			vec = QuaternionToEuler(t->rotation);
			ImGui::Text("Rotate object to\n");
			text = std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);

			//t = "Rotate";
			break;
		case 2:

			ImGui::Text("Scale object to\n");
			text = std::to_string(t->scale.x) + " " + std::to_string(t->scale.y) + " " + std::to_string(t->scale.z);
			vec = t->scale;
			//t = "Scale";
		default:
			break;
		}
			ImGui::Text(&text[0]);
			ImGui::Text("In\n");
			ImGui::InputFloat("Seconds", &time);

			if (ImGui::Button("Lock in values", ImVec2(300, 20)))
			{
				actionList[index].m = static_cast<Manipulations>(editorVal);
				actionList[index].time = time;
				actionList[index].Val = vec;
			}
			if (ImGui::Button("Save actions to File", ImVec2(300, 30)))
			{
				WriteToDirectorFile();
			}



	}
	if (ImGui::Button("Read actions from File", ImVec2(300, 30)))
	{
		ReadFromDirectorFile();
	}
	ImGui::Checkbox("Enable Collision After? ", &enableCollisionAfter);
	if (actionList.size())
	{
		for (int i = 0; i < actionList.size(); i++)
		{
			std::string ttt;
			switch (actionList[i].m)
			{
			case Translate:
				ttt = "Translate to";
				break;
			case Rotate:
				ttt = "Rotate to";
				break;
			case Scale:
				ttt = "Scale to";
				break;
			default:
				break;
			}

			ttt += std::to_string(actionList[i].Val.x) + " " + std::to_string(actionList[i].Val.y) + " " + std::to_string(actionList[i].Val.z);
			ttt += "in " + std::to_string(actionList[i].time);
			ImGui::Text(&ttt[0]);
		}

		if (ImGui::Button("Set Object to Position/Rotation/Scale in actionList", ImVec2(200,30)))
		{
			switch (actionList[index].m)
			{
			case Translate:
				t->pos = actionList[index].Val;
				break;
			case Rotate:
				t->rotation = EulerToQuaternion(actionList[index].Val);
				break;
			case Scale:
				t->scale = actionList[index].Val;
				break;
			default:
				break;
			}
		
		}
	}


}

void Director::ReadFromDirectorFile()
{

	std::ifstream keysFile;
	std::string line;
	char delim = ' ';
	std::vector<float> vals;
	size_t pos;
	float token, t;
	index = 0;
	actionList.clear();
	std::string n = ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".director";
	keysFile.open(n);
	if (keysFile.is_open())
	{
		// Get the position
		while (keysFile)
		{

			std::getline(keysFile, line);
			if (line != "")
			{
				int m = std::stoi(line);
				std::getline(keysFile, line);
				vals.clear();
				//Get position
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
					std::getline(keysFile, line);
					t = std::stof(line);
					actionList.push_back(DirectorActions(m, point, t));

				}
			}
		}
	}
	keysFile.close();
}
void Director::WriteToDirectorFile()
{
	std::ofstream actionsFile;
	actionsFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".director", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < actionList.size(); i++)
	{
		actionsFile << actionList[i].m << '\n';
		actionsFile << actionList[i].Val.x << " " << actionList[i].Val.y << " " << actionList[i].Val.z << " " << '\n';
		actionsFile << actionList[i].time << '\n';
	}
}
void Director::HandleMessage(MsgData_Base* m) {}

void Director::HandleKeyboard(float) {}
void Director::HandleController(float) {}