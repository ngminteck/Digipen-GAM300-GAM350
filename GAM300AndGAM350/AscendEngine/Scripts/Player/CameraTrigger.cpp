#include "pch.h"
CameraTrigger::CameraTrigger() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(CameraTrigger))
{

}
CameraTrigger::~CameraTrigger()
{
	if (c)
		delete c;
}

void CameraTrigger::Initialize()
{
	Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
	if (w != nullptr && w->waypoints.size() >= 4)
	{
		c = new Bezier();
		c->set_steps(20);
		for (int i = 0; i < w->waypoints.size(); i++)
		{
			c->add_way_point(w->waypoints[i]);
		}

	}

	if (NameOfEndingObj != "")
	{
		EndingObj = ASC_FACTORY.GetGameObjectByName(NameOfEndingObj.c_str());
	}

}
void CameraTrigger::FormCurve()
{
	if (c == nullptr)
		c = new Bezier();
	else
		c->clear();
	Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
	if (w != nullptr && w->waypoints.size() >= 4)
	{
		for (int i = 0; i < w->waypoints.size(); i++)
		{
			c->add_way_point(w->waypoints[i]);
		}

	}

}
void CameraTrigger::OnStart()
{
	ReadFromCameraFile();
	actionindex = 0;
	activate = true;
	Waypointer * w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
	if(w!= nullptr)
		FormCurve();

	if (NameOfEndingObj != "")
	{
		EndingObj = ASC_FACTORY.GetGameObjectByName(NameOfEndingObj.c_str());
	}
	//np->obj_phy->SetHaveBody(true);

}
void CameraTrigger::StartMovements()
{
	PlayerController* pp = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
	pp->canMove = false;
	pp->freezeCam = true;
	pos = ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<TransformComp>()->pos;
	run = true;
	runTime = 0.0f;
	waitTime = 999.0f;
	intPos = pp->player_cam.GetPos();
	intRot = pp->player_cam.GetForward();
	actionindex = 0;
	fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");
	pp->GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->SetRigidBodyEnable(false);
	pp->GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->BodySetGravityEnable(false);
	fadeTime = 2.0f;
	DoTheFade = true;
}
Vec3 CameraTrigger::CamLERP(Vec3 a, Vec3 b, float t) {
	float ax = a.x, ay = a.y, az = a.z;
	return Vec3(ax + t * (b.x - ax),
		ay + t * (b.y - ay),
		az + t * (b.z - az)
	);
}
bool CameraTrigger::Fading(const Vec3& pos , float dt)
{
	PlayerController* pp = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
	if (!toggle)
	{
		if (fadeTime > 0.0)
		{
			fadeTime -= dt;
			fade->GetHdl<GraphicsComp>()->SetTransparencyVal((1 - (fadeTime / 2)));

		}
		else
		{
			ASC_FACTORY.GetGameObjectByName("Dogshit")->GetHdl<GraphicsComp>()->Enabled = true;
			pp->SetCutsceneRot();
			ASC_FACTORY.GetGameObjectByName("Dogshit");
			pp->player_cam.SetPosition(pos);
			if(actionList[actionindex].viewType == LOOKAT)
				pp->player_cam.LookAt(actionList[actionindex].focusObj->GetHdl<TransformComp>()->pos);
			else
				pp->player_cam.LookAt(pos+actionList[actionindex].posToRot);
			intRot = pp->player_cam.GetForward();
			pp->player_cam.SetFOV(pp->defaultFOV);
			Director* dd = ASC_GETSCRIPT_PTR(Director, goHdl);
			if(g.IsValid() && dd==nullptr)
			g->SetVisible(false);
			fadeTime = 2.0f;
			toggle = true;

		}
	}
	else
	{
		if (fadeTime > 0.0)
		{
			if (actionList[actionindex].viewType == LOOKAT)
			{
				pp->player_cam.LookAt(CamLERP(pp->player_cam.GetPos() + intRot, actionList[actionindex].focusObj->GetHdl<TransformComp>()->pos, ratio));
			}
			fadeTime -= dt;
			fade->GetHdl<GraphicsComp>()->SetTransparencyVal((fadeTime / 2));
		}
		else
		{
			DoTheFade = false;
			toggle = false;
			return true;

		}
	}
	return false;
}

bool CameraTrigger::Fading(bool& b, float dt)
{
	PlayerController* pp = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
	if (!toggle)
	{
		if (fadeTime > 0.0)
		{
			fadeTime -= dt;
			fade->GetHdl<GraphicsComp>()->SetTransparencyVal((1 - (fadeTime / 2)));
			return false;
		}
		else
		{

			fadeTime = 2.0f;
			pp->freezeCam = false;
			pp->canMove = true;
			pp->player_cam.SetFOV(90);
			pp->GetGOHandle()->GetHdl<TransformComp>()->pos = pos;
			pp->GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->SetRigidBodyEnable(true);
			pp->GetGOHandle()->GetHdl<NewPhysicsComp>()->obj_phy->BodySetGravityEnable(true);

			//t->pos.y = -2000;
			np->obj_trigger->GeomCollisionSetEnable(false);
			t->dirty = true;
			pp->player_cam.SetPosition(pp->GetGOHandle()->GetHdl<TransformComp>()->pos);
			if (EndingObj.IsValid())
			{
				pp->player_cam.LookAt(EndingObj->GetHdl<TransformComp>()->pos);
			}
			ASC_FACTORY.GetGameObjectByName("Dogshit")->GetHdl<GraphicsComp>()->Enabled = false;
			GOHandle ppp = ASC_FACTORY.GetGameObjectByName("LivingRoomPuppy");
			if (ppp.IsValid())
			{
				ppp->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
			}
			GOHandle ppl = ASC_FACTORY.GetGameObjectByName("LivingRoomPuppyReal");
			{
				if (ppl.IsValid())
				{
					ppl->GetHdl<NewPhysicsComp>()->obj_phy->GeomCollisionSetEnable(true);
					ppl->GetHdl<NewPhysicsComp>()->obj_phy->SetHaveBody(true);
					ppl->GetHdl<NewPhysicsComp>()->obj_phy->SetRigidBodyEnable(true);
					ppl->GetHdl<NewPhysicsComp>()->obj_phy->BodySetGravityEnable(true);
					ppl->isEnable = true;
					ppl->GetHdl<GraphicsComp>()->Enabled = true;
				}
			}
			toggle = true;
			return false;
		}
	}
	else
	{
		if (fadeTime > 0.0)
		{
			fadeTime -= dt;
			fade->GetHdl<GraphicsComp>()->SetTransparencyVal((fadeTime / 2));
			return false;
		}
		else
		{

			//pp->freezeCam = false;
			DoTheFade = false;
			toggle = false;
			return true;
		}
	}

}
void CameraTrigger::Update(float dt)
{

	if (run)
	{
		PlayerController* pp = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);

		if (!waiting && actionindex <actionList.size())
		{
			switch (actionList[actionindex].camAct)
			{
			case TELEPORT:
				if (DoTheFade)
				{
					if (Fading(actionList[actionindex].posToMove, dt))
					{
						runTime += dt;
						if (actionList[actionindex]._waitTime != 0.0f)
						{
							ratio = (runTime / actionList[actionindex]._waitTime);
						}
						intPos = actionList[actionindex].posToMove;
					}
					else
					{

					}
				}
				else
				{
					pp->player_cam.SetPosition(actionList[actionindex].posToMove);
					runTime += dt;
					ratio = (runTime / actionList[actionindex]._waitTime);
					intPos = actionList[actionindex].posToMove;
				}
				break;
			case MOVETO:
				runTime += dt;
				ratio = (runTime / actionList[actionindex]._time);
				pp->player_cam.SetPosition(CamLERP(intPos,actionList[actionindex].posToMove , ratio));

				break;
			case WAYPOINT:
				runTime += dt;
				{		
					ratio = (runTime / actionList[actionindex]._time);
					float wRatio;
					Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
					if (w != nullptr)
					{
						if (!useCurve)
						{
							if (timeForEachWaypoint == 0.0)
							{
								timeForEachWaypoint = (actionList[actionindex]._time / w->waypoints.size());
							}
							wRatio = (runTime / ((wayPointerIndex + 1) * timeForEachWaypoint));
							if (wRatio > 1.0f)
							{
								int idd = 0;
								while (wRatio > 1.0f)
								{
									wRatio -= 1.0f;
									idd++;
								}
								wayPointerIndex++;
								intPos = t->pos;
							}
							pp->player_cam.SetPosition(CamLERP(intPos, w->waypoints[wayPointerIndex], wRatio));
						}
						else
						{
							if (timeForEachNode == 0.0)
							{
								timeForEachNode = (actionList[actionindex]._time / c->_nodes.size());
							}
							wRatio = (runTime / ((nodePointerIndex + 1) * timeForEachNode));
							if (wRatio > 1.0f)
							{
								nodePointerIndex++;

								std::cout << "moving to " << c->_nodes[nodePointerIndex].x << " " << c->_nodes[nodePointerIndex].y << " " << c->_nodes[nodePointerIndex].z << std::endl;
								intPos = t->pos;
								wRatio = (runTime / ((nodePointerIndex + 1) * timeForEachNode));
							}
							Vec3 tp = CamLERP(intPos, c->_nodes[nodePointerIndex], wRatio);
							std::cout << "moving to " << tp.x << " " << tp.y << " " << tp.z << std::endl;
							pp->player_cam.SetPosition(CamLERP(intPos, c->_nodes[nodePointerIndex], wRatio));
						}

					}
				}
				break;
			default:
				break;
			}
			switch (actionList[actionindex].viewType)
			{
			case LOOKAT:
				
				if (!DoTheFade)
				{
					 Vec3 newFor = (actionList[actionindex].focusObj->GetHdl<TransformComp>()->pos - pp->player_cam.GetPos()).Normalize();
					pp->player_cam.LookAt(CamLERP(pp->player_cam.GetPos() + intRot, pp->player_cam.GetPos() + newFor, ratio));
				}
				break;
			case GETROTATION:
				if (!DoTheFade)
				{
					if (actionList[actionindex].camAct == WAYPOINT)
					{
						Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
						float wRatio;
						if (w != nullptr)
						{
							if (timeForEachWaypoint == 0.0)
							{
								timeForEachWaypoint = (actionList[actionindex]._time / w->CameraLookats.size());
							}
							wRatio = (runTime / ((wayPointerIndex + 1) * timeForEachWaypoint));
							if (ratio > 1.0f)
							{
								if (useCurve)
									wayPointerIndex++;
								intPos = t->pos;
								wRatio = (runTime / ((wayPointerIndex + 1) * timeForEachWaypoint));
								
							}
							pp->player_cam.LookAt(CamLERP(intRot, w->CameraLookats[wayPointerIndex], wRatio));

						}
					}
					else
					{
						if(actionList[actionindex].camAct!= TELEPORT)
							pp->player_cam.LookAt(CamLERP(pp->player_cam.GetPos() + intRot, pp->player_cam.GetPos() + actionList[actionindex].posToRot, ratio));
					}
				}
				break;
			default:
				break;
			}
			if (ratio >= 1.0f)
			{
				intPos = pp->player_cam.GetPos();
				intRot = pp->player_cam.GetForward();
				if (!DoTheFade)
				{
					waiting = true;
					waitTime = 0.0f;
				}
			}

		}
		if (waiting)
		{
			//Keep Looking at the object if we are focusing
			if (actionList[actionindex].viewType == LOOKAT)
			{
				pp->player_cam.LookAt(actionList[actionindex].focusObj->GetHdl<TransformComp>()->pos);
			}

			waitTime += dt;
			waitRatio = (waitTime / actionList[actionindex]._waitTime);
			if (waitRatio >= 1.0f)
			{
				actionindex++;
				if (actionList[actionindex].camAct == TELEPORT)
				{
					DoTheFade = true;
				}
				if (actionList[actionindex].camAct == WAYPOINT)
				{
					Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);

				}
				waiting = false;
				runTime = 0.0f;
				ratio = 0.0f;
				fadeTime = 2.0f;
				intPos = pp->player_cam.GetPos();
				intRot = pp->player_cam.GetForward();
			}
		}
		if (actionindex == actionList.size())
		{
			if (Fading(run, dt))
			{
				DoTheFade = false;
				run = false;
				actionindex = 0;


			}
		}
	}

}
void CameraTrigger::OnDestroy()
{}

void CameraTrigger::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("NameOfEndingObject", NameOfEndingObj);
}
void CameraTrigger::Deserialize(Deserializer& d)
{
	d.Read("NameOfEndingObject", NameOfEndingObj);

}
void CameraTrigger::Reflect(const Reflector& r) 
{

	/*ImGui::Text("Current Starting Values");
	std::string tt;
	tt ="Position: " + std::to_string(t->pos.x) + " " + std::to_string(t->pos.y) + " " + std::to_string(t->pos.z);
	ImGui::Text(&tt[0]);
	tt = "Rotation In Quaternion: " + std::to_string(t->rotation.x) + " " + std::to_string(t->rotation.y) + " " + std::to_string(t->rotation.z) + " " + std::to_string(t->rotation.w);
	ImGui::Text(&tt[0]);
	tt = "Scale: " + std::to_string(t->scale.x) + " " + std::to_string(t->scale.y) + " " + std::to_string(t->scale.z);
	ImGui::Text(&tt[0]);*/
	Camera & eCam  = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);

	ImGui::InputText("Ending Object Name", &NameOfEndingObj[0], 128);

	if (ImGui::Button("Add an action", ImVec2(300, 30)))
	{
		AddCameraAction();
	}
	//QuaternionFromMatrix(eCam.GetV());
	if (actionList.size())
	{
			ImGui::SliderInt("Action" + index, &index, 0, static_cast<int>(actionList.size()) - 1);
			ImGui::SliderInt("Type of Camera Movement", &editorVal, 0, static_cast<int>(CameraMovements::DONOTGOPASTTHISPOINT) );
			std::string text;
			Vec3 vec;
			//Set values according to key
			//tt = actionList[index]._time;
			//wt = actionList[index]._waitTime;
			if (actionList[index].focusObj.IsValid())
			{
				inputName = actionList[index].focusObj->name;
			}


			//std::string t;
			switch (editorVal)
			{
				//Teleport to
			case 0:
				ImGui::Text("Teleport to\n");
				text = std::to_string(eCam.GetPos().x) + " " + std::to_string(eCam.GetPos().y) + " " + std::to_string(eCam.GetPos().z);
				vec = eCam.GetPos();
				ImGui::Text(&text[0]);
				ImGui::SliderInt("Type of Camera View", &viewVal, 0, static_cast<int>(CameraViews::GETROTATION));
				if(!viewVal)
				{
					ImGui::Text("Looking at\n");
					ImGui::InputText("Object name", &inputName[0], 128);
				}
				else
				{
					ImGui::Text("With the current Rotation\n");
				}
				ImGui::Text("For\n");
				ImGui::DragFloat("Time(in Seconds): ", &wt);
				ImGui::Text("seconds");
				break;
			case 1:
				ImGui::Text("Move to\n");
				text = std::to_string(eCam.GetPos().x) + " " + std::to_string(eCam.GetPos().y) + " " + std::to_string(eCam.GetPos().z);
				vec = eCam.GetPos();
				ImGui::Text(&text[0]);
				ImGui::SliderInt("Type of Camera View", &viewVal, 0, static_cast<int>(CameraViews::GETROTATION));
				if (!viewVal)
				{
					ImGui::Text("Looking at\n");
					ImGui::InputText("Object name", &inputName[0], 128);
				}
				else
				{
					ImGui::Text("With the current Rotation\n");
				}
				ImGui::Text("in\n");
				ImGui::DragFloat("Time(in Seconds)", &tt);
				ImGui::Text("seconds\n");
				ImGui::Text("and wait for\n");
				ImGui::DragFloat("", &wt);
				ImGui::Text("seconds\n");
				break;
			case WAYPOINT:
				ImGui::Text("Follow Waypoints from Waypointer Script\n");
				ImGui::SliderInt("Type of Camera View", &viewVal, 0, static_cast<int>(CameraViews::GETROTATION));
				if (!viewVal)
				{
					ImGui::Text("Looking at\n");
					ImGui::InputText("Object name", &inputName[0], 128);
				}
				else
				{
					ImGui::Text("With the WayPoint Rotation\n");
				}
				ImGui::Text("in\n");
				ImGui::DragFloat("Time(in Seconds)", &tt);
				ImGui::Text("seconds\n");
				ImGui::Text("and wait for\n");
				ImGui::DragFloat("", &wt);
				ImGui::Text("seconds\n");
				break;
			default:
				break;
			}
		//	
		//	ImGui::Text("In\n");
		//	ImGui::InputFloat("Seconds", &time);

			if (ImGui::Button("Lock in values", ImVec2(300, 20)))
			{
				switch (editorVal)
				{
				case 0:
					if (viewVal == 0)
					{
						actionList[index].focusObj = ASC_FACTORY.GetGameObjectByName(inputName.c_str());
						if (actionList[index].focusObj.IsValid())
						{
							actionList[index].posToMove = vec;
							actionList[index]._waitTime = wt;
						}
					}
					else
					{
						actionList[index].posToMove = vec;
						actionList[index].posToRot = eCam.GetForward();
						actionList[index]._waitTime = wt;
					}
					break;
				case 1:
					if (viewVal == 0)
					{
						actionList[index].focusObj = ASC_FACTORY.GetGameObjectByName(inputName.c_str());
						if (actionList[index].focusObj.IsValid())
						{
							actionList[index].posToMove = vec;
							actionList[index]._time = tt;
							actionList[index]._waitTime = wt;
						}
					}
					else
					{
						actionList[index].posToMove = vec;
						actionList[index].posToRot = eCam.GetForward();
						actionList[index]._waitTime = wt;
						actionList[index]._time = tt;
					}
					break;
				default:
					break;
				}
				actionList[index].camAct = static_cast<CameraMovements>(editorVal);
				actionList[index].viewType = static_cast<CameraViews>(viewVal);
		//		actionList[index].Val = vec;
			}
			if (ImGui::Button("Save actions to File", ImVec2(300, 30)))
			{
				WriteToCameraFile();
			}

	}

	//}
	if (ImGui::Button("Read actions from File", ImVec2(300, 30)))
	{
		ReadFromCameraFile();
	}

	if (actionList.size())
	{
		std::string mov = "Move Camera to Action " + index; 
		mov += " result";
		if (ImGui::Button(mov.c_str(), ImVec2(300,20)))
		{
			switch (actionList[index].camAct)
			{
			case TELEPORT:
			case MOVETO:
				eCam.SetPosition(actionList[index].posToMove);
				break;
			case WAYPOINT:
				Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);
				if(w!=nullptr)
				eCam.SetPosition(w->waypoints[w->waypoints.size()]);
			break;
			}
			switch (actionList[index].viewType)
			{
			case LOOKAT:
				if (!actionList[index].focusObj.IsValid())
				{
					actionList[index].focusObj = ASC_FACTORY.GetGameObjectByName(actionList[index].focusObj->name.c_str());
				}
				eCam.LookAt(actionList[index].focusObj->GetHdl<TransformComp>()->pos);
				break;
			case GETROTATION:
				eCam.LookAt(eCam.GetPos() + actionList[index].posToRot);
			}

		}
		if (actionList[actionindex].camAct == WAYPOINT)
		{
			if(ImGui::Button("Form The Curve", ImVec2(300, 20)))
			{
				FormCurve();
			}
			if (c != nullptr)
			{
				ImGui::Text("Curve Stats");
				ImGui::Separator();
				ImGui::Text("Number Of Points :");
				ImGui::Text(std::to_string(c->_nodes.size()).c_str());
			}
		}
		ImGui::SliderInt("Saved Action" + actionindex, &actionindex, 0, static_cast<int>(actionList.size()) - 1);
			std::string ttt;
	
				switch (actionList[actionindex].camAct)
				{
				case TELEPORT:
					ttt = "Teleport to ";
					break;
				case MOVETO:
					ttt = "Move to ";

					break;
				default:
					break;
				}

				ttt += std::to_string(actionList[actionindex].posToMove.x) + " " + std::to_string(actionList[actionindex].posToMove.y) + " " + std::to_string(actionList[actionindex].posToMove.z);
				if (actionList[actionindex].camAct)
				{
					ttt += " in ";
					ttt += std::to_string(actionList[actionindex]._time) + " seconds";
				}
				switch (actionList[actionindex].viewType)
				{
				case LOOKAT:
					ttt += " while looking at \n";
					if (actionList[actionindex].focusObj.IsValid())
						ttt += actionList[actionindex].focusObj->name;
					else
						ttt += "There is an error finding object";

					break;
				case GETROTATION:
					ttt += " with rotation towards\n";
					ttt += std::to_string(actionList[actionindex].posToRot.x) + " " + std::to_string(actionList[actionindex].posToRot.y) + " " + std::to_string(actionList[actionindex].posToRot.z);
					break;
				default:
					break;
				}

				ttt += ", wait for " + std::to_string(actionList[actionindex]._waitTime) + " seconds";
				ImGui::Text(&ttt[0]);
		




}



}
void CameraTrigger::HandleMessage(MsgData_Base* m)
{}
void CameraTrigger::OnCollisionEnter(GOHandle other)
{
	if (other->name == "DogPlayer")
	{
		//pos = other->GetHdl<TransformComp>()->pos;
		StartMovements();
		if (!g.IsValid())
		{
			t->pos.y = -2000.0f;
		}
	}

}
void CameraTrigger::WriteToCameraFile()
{
	std::ofstream keysFile;
	keysFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".camTrig", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < actionList.size(); i++)
	{
			keysFile << actionList[i].camAct << '\n';
			keysFile << actionList[i].viewType << '\n';
			switch (actionList[i].camAct)
			{
			case TELEPORT:
				break;
			case MOVETO:
			case WAYPOINT:
			keysFile << actionList[i]._time << '\n';
				break;
			default:
				break;
			}
			switch (actionList[i].viewType)
			{
			case LOOKAT:
				keysFile << actionList[i].focusObj->name << '\n';
			break;
			case GETROTATION:
				keysFile << actionList[i].posToRot.x << " " << actionList[i].posToRot.y << " " << actionList[i].posToRot.z << " " << '\n';
				break;
			default:
				break;
			}

			keysFile << actionList[i].posToMove.x << " " << actionList[i].posToMove.y << " "  <<actionList[i].posToMove.z <<" " <<'\n';
			keysFile << actionList[i]._waitTime << '\n';
	}

	keysFile.close();

}

void CameraTrigger::ReadFromCameraFile()
{
	actionList.clear();
	int i = 0;

	std::ifstream keysFile;
	keysFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".camTrig");
	if (keysFile.is_open())
	{
		while (keysFile)
		{
			CameraMovements c;
			CameraViews v;
			float token;
			std::string delim = " ";
			size_t pos;
			Vec3 ps, rot;
			std::string line;
			std::vector<float> vals;
			//camAct
			std::getline(keysFile, line);
			if (line != "")
			{
				c = static_cast<CameraMovements>(stoi(line));
				//CamView
				std::getline(keysFile, line);
				v = static_cast<CameraViews>(stoi(line));
				actionList.push_back(Actions());
				actionList[i].camAct = c;
				actionList[i].viewType = v;
				switch (actionList[i].camAct)
				{
				case TELEPORT:
					break;
				case MOVETO:
				case WAYPOINT:
					std::getline(keysFile, line);
					actionList[i]._time = stof(line);
					break;
				default:
					break;
				}
				std::getline(keysFile, line);
				switch (actionList[i].viewType)
				{
				case LOOKAT:
					actionList[i].focusObj = ASC_FACTORY.GetGameObjectByName(line.c_str());
					break;
				case GETROTATION:
					while ((pos = line.find(delim)) != std::string::npos) {
						token = std::stof(line.substr(0, pos));
						vals.push_back(token);
						line.erase(0, pos + 1);
					}
					if (vals.size())
					{
						ps.x = vals[0];
						ps.y = vals[1];
						ps.z = vals[2];
						actionList[i].posToRot = ps;
					}
					break;
				default:
					break;
				}
				//position
				std::getline(keysFile, line);
				vals.clear();
				while ((pos = line.find(delim)) != std::string::npos) {
					token = std::stof(line.substr(0, pos));
					vals.push_back(token);
					line.erase(0, pos + 1);
				}
				if (vals.size())
				{
					rot.x = vals[0];
					rot.y = vals[1];
					rot.z = vals[2];
					actionList[i].posToMove = rot;
				}
				//wait time
				std::getline(keysFile, line);
				actionList[i]._waitTime = stof(line);
				i++;

			}
			
		}


	}

	keysFile.close();
}

void CameraTrigger::AddCameraAction(int c , int v , float t, bool wayPoint)
{
	Actions a(c, v, t, Vec3::ZERO, Vec3::ZERO);
	actionList.push_back(a);
}

void CameraTrigger::HandleKeyboard(float)
{}
void CameraTrigger::HandleController(float)
{}
