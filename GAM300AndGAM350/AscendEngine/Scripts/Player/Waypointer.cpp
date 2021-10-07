#include "pch.h"
Waypointer::Waypointer() :
    ScriptCommonIF(ASC_GETSCRIPTNAME(Waypointer)) {}
Waypointer::~Waypointer() {}

void Waypointer::OnStart()
{
	ReadfromWaypointFile();
}
void Waypointer::Update(float dt)
{}
void Waypointer::OnDestroy()
{}

void Waypointer::Serialize(Serializer& sr, LogicHandle idx)
{
    sr.Write(GetName());
	sr.Write("IsScent", scent);
	sr.Write("useCam", useCam);

}
void Waypointer::WritetoWaypointFile()
{

	std::ofstream posFile;
	posFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".way", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < waypoints.size(); i++)
	{

		posFile << waypoints[i].x << " " << waypoints[i].y << " " << waypoints[i].z << " " << '\n';
		if (useCam)
		{
			posFile << CameraLookats[i].x << " " << CameraLookats[i].y << " " << CameraLookats[i].z << " " << '\n';
		}

	}
	posFile.close();

}
void Waypointer::ReadfromWaypointFile()
{
	std::ifstream posFile;
	std::string line;
	char delim = ' ';
	std::vector<float> vals;
	size_t pos;
	float token;
	int i = 0;
	waypoints.clear();
	CameraLookats.clear();
	std::string n = ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".way";
	posFile.open(n);
	if (posFile.is_open())
	{
		// Get the position
		while (posFile)
		{
			std::getline(posFile, line);
			vals.clear();
			
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
				waypoints.push_back(point);
				//There are rotations
				if (useCam)
				{
					std::getline(posFile, line);
					vals.clear();

					while ((pos = line.find(delim)) != std::string::npos) {
						token = std::stof(line.substr(0, pos));
						vals.push_back(token);
						line.erase(0, pos + 1);
					}
					if (vals.size() > 0)
					{
						Vec3 points;
						points.x = vals[0];
						points.y = vals[1];
						points.z = vals[2];
						CameraLookats.push_back(points);
					}
				}
				i++;
			}
		}
	}


}
void Waypointer::Deserialize(Deserializer& d)
{
	d.Read("IsScent", scent);
	d.Read("useCam", useCam);
	ReadfromWaypointFile();
}
void Waypointer::Reflect(const Reflector& r) {
   
	if (goHdl.IsValid())
	{
		CameraTrigger* ct = ASC_GETSCRIPT_PTR(CameraTrigger, goHdl);
		if (ct != nullptr)
		{
			useCam = true;
			ImGui::Text("Using Editor Camera");
			cam = &ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);
		}
		static int location;
		if (ImGui::Button("Add Waypoint", ImVec2(200, 30)))
		{
			AddWaypoint();
		}

		for (int i = 0; i < waypoints.size(); i++)
		{
			std::string text = std::to_string(waypoints[i].x) + " " + std::to_string(waypoints[i].y) + " " + std::to_string(waypoints[i].z);
			ImGui::Text(&text[0]);

		}
		if (waypoints.size() > 0)
		{
			ImGui::SliderInt("Index", &location, 0, static_cast<int>(waypoints.size()) - 1);

			if (ImGui::Button("Remove waypoint at index", ImVec2(200, 30)))
			{
				auto it = waypoints.begin();
				waypoints.erase(it + location);
				if (location >= waypoints.size())
				{
					location = waypoints.size() == 0 ? 0 : waypoints.size() - 1;
				}
			}
			if (ImGui::Button("Move to waypoint at index", ImVec2(200, 30)))
			{
				if (useCam)
					cam->SetPosition(waypoints[location]);
				else
					t->pos = waypoints[location];
			}
			if (ImGui::Button("Move to next waypoint", ImVec2(200, 30)))
			{
				location = location + 1 >= waypoints.size() ? location = 0 : location + 1;
				if (useCam)
					cam->SetPosition(waypoints[location]);
				else
					t->pos = waypoints[location];
			}
			if (ImGui::Button("Clear all", ImVec2(200, 30)))
			{
				waypoints.clear();
				CameraLookats.clear();
			}
		}
		if (ImGui::Button("Save Waypoints To File", ImVec2(200, 30)))
		{
			WritetoWaypointFile();
		}
		if (ImGui::Button("Read Waypoints From File", ImVec2(200, 30)))
		{
			ReadfromWaypointFile();
		}

		ImGui::Checkbox("Is Scent?", &scent);
	}
}
void Waypointer::HandleMessage(MsgData_Base* m) {}

void Waypointer::HandleKeyboard(float) {}
void Waypointer::HandleController(float) {}

void Waypointer::AddWaypoint()
{
	Vec3 waypoint;
	Vec3 look;
	if (useCam)
	{
		waypoint = cam->GetPos();
		look = cam->GetForward();
		CameraLookats.push_back(look);
	}
	else
	{
		waypoint = t->pos;
	}
    waypoints.push_back(waypoint);



}