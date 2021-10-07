#include "pch.h"

Scent::Scent() : ScriptCommonIF(ASC_GETSCRIPTNAME(Scent)), w(nullptr), c(nullptr)
{
}
Scent::~Scent()
{
	if (c)
	{
		delete c;
	}
}

void Scent::OnStart()
{
	
	for (int i = 0; i < 10; i++)
	{
		Trail.push_back(ASC_FACTORY.CreateGOfromPB("LeadingPart"));
		Trail[i]->GetHdl<ParticleEmitter>()->minLifetime = Trail[i]->GetHdl<ParticleEmitter>()->maxLifetime = 0.0f;
	}

	player = ASC_FACTORY.GetObjectHandleList(go_player)[0];

	index = 0;
	closestWaypoint = Vec3::ZERO;
	lastwp = 0;
	TrailInit = false;
	focused = false;
	///Test curve creation
	//w = ASC_GETSCRIPT_PTR( Waypointer, ASC_FACTORY.GetGameObjectByName("Waypointtt"));

}
void Scent::Update(float dt) 
{
	if (ASC_ENGINE.GetUsingMouse())
	{
		HandleKeyboard(dt);
	}
	else
	{
		HandleController(dt);
	}
	//Setup differences for comparison
	//if (w != nullptr && distDiff.size() == 0)
	//{
	//	for (int i = 0; i < w->waypoints.size(); i++)
	//	{
	//		distDiff.push_back((w->waypoints[w->waypoints.size() - 1] - w->waypoints[i]).MagnitudeSq());
	//	}
	//}
	//if(w != nullptr)
	//distFromObjective = (t->pos - w->waypoints[w->waypoints.size() - 1]).MagnitudeSq();

	////Check which is the closest waypoint
	//if (distDiff.size())
	//{
	//	for (int i = 0; i < distDiff.size(); i++)
	//	{
	//		//if the distance is longer than my current distance then the next waypoint is the one I want to go to
	//		if (distDiff[i] > distFromObjective)
	//		{
	//			closestWaypoint = w->waypoints[i];
	//			//if()
	//			break;
	//		}
	//		//if the distance is shorter than my current distance then I have already passed the waypoint
	//		else
	//			continue;
	//	}
	//}

	//Vec3 Dir = closestWaypoint - t->pos;
	
	if (w != nullptr && !TrailInit)
	{
		InitTrail();
	}

	//Dont do anything with curve is w is nullptr
	if (w == nullptr)
	{
		TrailInit = false;
	}

	if (TrailInit && c->_nodes.size() > 0)
	{
		if (c->_nodes.size() - index < 15 && lastwp < w->waypoints.size()-1)
		{
			FormTrail();
		}
		else if (c->_nodes.size() - index < 15 && lastwp == w->waypoints.size() - 1)
		{
			//End of the trail bitch
			w = nullptr;
			TrailInit = false;
			c->clear();
			index = 0;
			focused = false;
			SetTrailParticles(0.0f);
		}
		//check for the 10 closest points 
		if (TrailInit)
		{
			for (int i = 0; i < c->_nodes.size() - Trail.size(); i++)
			{
				if ((c->_nodes[i] - player->GetHdl<TransformComp>()->pos).MagnitudeSq() < (c->_nodes[i + 1] - player->GetHdl<TransformComp>()->pos).MagnitudeSq())
				{
					index = i;
					break;
				}
			}

			for (int j = index, i = 0; i < Trail.size(); j++, i++)
			{
				Trail[i]->GetHdl<TransformComp>()->pos = c->_nodes[j];
				Vec3 v = c->_nodes[j + 1] - c->_nodes[j];
				Trail[i]->GetHdl<ParticleEmitter>()->minVelocity = Trail[i]->GetHdl<ParticleEmitter>()->maxVelocity = Vec3(v.x * 10, 0.0f, v.z * 10);
			}
		}
		if (focused)
		{
			Camera& c = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
			//c.SetFOV(40);
			for (int i = 0; i < Trail.size(); i++)
			{
				Trail[i]->GetHdl<ParticleEmitter>()->minLifetime = Trail[i]->GetHdl<ParticleEmitter>()->maxLifetime = 1.0f;
				Trail[i]->GetHdl<ParticleEmitter>()->countdown = 3.0f;
			}
		}
		else
		{
			Camera& c = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
			//c.SetFOV(90);
			for (int i = 0; i < Trail.size(); i++)
			{
				if(Trail[i]->GetHdl<ParticleEmitter>()->countdown > 0.0f)
					Trail[i]->GetHdl<ParticleEmitter>()->countdown -= dt;
				else
					Trail[i]->GetHdl<ParticleEmitter>()->minLifetime = Trail[i]->GetHdl<ParticleEmitter>()->maxLifetime = 0.0f;
			}
		}
	}



}

void Scent::SetTrailParticles(float t)
{
	for (int i = 0; i < Trail.size(); i++)
	{
		Trail[i]->GetHdl<ParticleEmitter>()->minLifetime = Trail[i]->GetHdl<ParticleEmitter>()->maxLifetime = t;
	}
}
void Scent::InitTrail()
{
	if (w != nullptr && w->waypoints.size() >= 4)
	{
		c = new Bezier();
		for (int i = 0; i < 4; i++)
		{
			c->add_way_point(w->waypoints[i]);
			lastwp = i;
		}
		TrailInit = true;
	}

}
void Scent::FormTrail()
{
	c->clear();
	int pos;
	if (lastwp + 4 > w->waypoints.size() - 1)
	{
		pos = lastwp = w->waypoints.size() - 4;

	}
	else
		pos = lastwp;
	for (int i = lastwp; i < pos + 4; i++)
	{

		c->add_way_point(w->waypoints[i]);
		lastwp = i;
	}
	index = 0;


}
void Scent::OnDestroy() {

}
void Scent::ResetTrail()
{
	index = 0;
	if(c!= nullptr)
		c->clear();
	TrailInit = false;
	lastwp = 0;
	w = nullptr;
}


void Scent::Serialize(Serializer& sr, LogicHandle idx) {
	sr.Write(GetName());
	sr.Write("objective", objective);
}
void Scent::Deserialize(Deserializer& d) {

	d.Read("objective", objective);
}
void Scent::Reflect(const Reflector& r) 
{
	ImGui::InputText("Objective Name", &objective[0], 128);
}
void Scent::HandleMessage(MsgData_Base* m) {

}

void Scent::HandleKeyboard(float) {
	if (ASC_KEYTRIGGERED(ASC_BUTTON_RIGHTMOUSE))
	{
		if (w != nullptr)
		{
			focused = !focused;
		}

	}
}
void Scent::HandleController(float) {
	if (ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::Y))
	{
		if (w != nullptr)
		{
			focused = !focused;
		}

	}
}

void Scent::WritetoWaypointFile() {

}
void Scent::ReadfromWaypointFile() {

}
