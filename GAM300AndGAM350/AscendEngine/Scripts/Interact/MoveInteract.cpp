#include "pch.h"
MoveInteract::MoveInteract()
{
	ASC_SETUP_SCRIPT(MoveInteract);
}
void MoveInteract::InteractAction()
{
	if(p->lastInteract->name == goHdl->name)
	{ 
   // std::cout << " here\n";
    if (!p->movingItem)
	  {
     // std::cout << " 1\n";
		  p->movingItem = true;
		 // np->obj_phy->SetHaveBody(true);
		  np->obj_phy->BodySetLinearDamping(0.5f);

		  p->push = Pushable;
		  p->pull = Pullable;
		  if (lockedPositions)
		  {
			  ResetWinds();
			  for (int i = 0; i < 4; i++)
			  {
				  winds[i] += t->pos;
			  }
			  //SetPlayerpos
			  SetPlayerPos();
       
		  }
		  ASC_PLAY("SFX_Grab.ogg");
		  if (prompt)
		  {
			  if(ASC_ENGINE.GetUsingMouse())
				  p->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_move_albedo";
			  else
				  p->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_Controller_move_albedo";
			  p->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(1.0f);
		  }
	}
	else
	{
		if (prompt)
		{
			p->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
		}
		p->movingItem = false;
		//np->obj_phy->SetHaveBody(false);
		np->obj_phy->BodySetDampingDefaults();
		p->push = false;
		p->pull = false;
		if (StaticObject)
		{
			if (side <= 1)
			{
				if (Forward)
				{
					t->pos.x = (CurrDistance >= Distance) ? OriginalPos.x + Distance : t->pos.x;
				}
				else
				{
					t->pos.x = (CurrDistance <= Distance) ? OriginalPos.x + Distance : t->pos.x;
				}
			}
			else
			{
				if (Forward)
				{
					t->pos.z = (CurrDistance >= Distance) ? OriginalPos.z + Distance : t->pos.z;
				}
				else
				{
					t->pos.z = (CurrDistance <= Distance) ? OriginalPos.z + Distance : t->pos.z;
				}
			}
		}
		np->obj_phy->BodySetLinearVel(Vec3::ZERO);
	}
	}
}
void MoveInteract::ResetWinds()
{
	dVector3 v;
	np->obj_phy->GeomBoxGetLengths(v);
	winds[0] = Vec3(static_cast<float>(v[0]), 0, 0);
	winds[1] = Vec3(static_cast<float>(-v[0]), 0, 0);
	winds[2] = Vec3(0, 0, static_cast<float>(v[2]));
	winds[3] = Vec3(0, 0, static_cast<int>(-v[2]));

}
void MoveInteract::SetPlayerPos()
{
	player = ASC_FACTORY.GetObjectHandleList(go_player)[0];
	Vec3 pos = player->GetHdl<TransformComp>()->pos;
	int shortest = 0;
	float temp = (pos - winds[0]).MagnitudeSq();
	for (int i = 1; i < 4; i++)
	{
		if ((pos - winds[i]).MagnitudeSq() < temp)
		{
			temp =( pos - winds[i]).MagnitudeSq();
			shortest = i;
		}
	}
	if (!StaticObject)
		player->GetHdl<TransformComp>()->pos = winds[shortest];
	else
		player->GetHdl<TransformComp>()->pos = winds[side];
	//Vec3 Dir = Vec3(t->pos.x, t->pos.y, t->pos.z);
	//p->player_cam.LookAt(t->pos);
	

}
void MoveInteract::OnStart()
{
	player = ASC_FACTORY.GetObjectHandleList(go_player)[0];
	p = ASC_GETSCRIPT_PTR(PlayerController, player);
	winds.push_back(Vec3(1, 0, 0));
	winds.push_back(Vec3(-1, 0, 0));
	winds.push_back(Vec3(0, 0, 1));
	winds.push_back(Vec3(0, 0, -1));
	OriginalPos = t->pos;

	//np->obj_phy->SetHaveBody(false);
	//np->obj_phy->is
	

}

void MoveInteract::Update(float dt)
{
	if (StaticObject)
	{
		np->obj_phy->SetRigidBodyEnable(false);
		np->obj_phy->GeomSetPosition(t->pos);
	}
	PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
	if (pz != nullptr)
	{
		if (pz->ParentPuzzleObject.IsValid())
		{
			PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, pz->ParentPuzzleObject);
			myIndex = puz->GetKeyIndex(goHdl);
			OriginalPos = puz->Objects[myIndex].posToBe;
		}
	}
	if (activated)
	{
		if (p != nullptr)
		{
			if (p->movingItem)
			{
				if (p->lastInteract->name == goHdl->name)
				{

					if (p->finalForce.MagnitudeSq() > 0.0f)
					{

						if (StaticObject)
						{
							PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
							if (Forward)
							{
								p->push = (fabs(CurrDistance) > fabs(Distance)) ? false : true;
								p->pull = (fabs(CurrDistance) < 0.0f) || (CurrDistance < 0.0f) && (t->pos.z > OriginalPos.z) ? false : true;
								if (pz != nullptr)
								{
									PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, goHdl);
									if (fabs(CurrDistance) > fabs(Distance))
									{
										puz->InputKey(myIndex, true);

									}
									else
									{
										puz->InputKey(myIndex, false);
									}
								}
							}
							else
							{
								p->pull = (fabs(CurrDistance) > fabs(Distance)) ? false : true;
								p->push = (t->pos.z > OriginalPos.z) ? false : true;
								if (pz != nullptr)
								{
									PuzzleLock* puz = ASC_GETSCRIPT_PTR(PuzzleLock, pz->ParentPuzzleObject);
									if (fabs(CurrDistance) > fabs(Distance))
									{
										puz->InputKey(myIndex, true);
									}
									else
									{
										puz->InputKey(myIndex, false);
									}
								}
							}

							np->obj_phy->BodyAddForce(p->finalForce * (p->moveSpeed * 0.3f));
							np->obj_phy->SetRigidBodyEnable(true);
						}
						else
						{
							if (!ASC_SOUNDMAN.IsPlaying(s, "SFX_DragObject.ogg"))
								ASC_PLAY("SFX_DragObject.ogg");
							np->obj_phy->BodyAddForce(p->finalForce * (p->moveSpeed * 0.3f));

							if (pe.IsValid())
							{
								pe->Emit();
							}
							np->obj_phy->SetRigidBodyEnable(true);
						}
						CurrDistance = side <= 1 ? t->pos.x - OriginalPos.x : t->pos.z - OriginalPos.z;
					}
					else
					{

						ASC_SOUNDMAN.Stop(s, "SFX_DragObject.ogg");
					}


					//
					float midGround = fabs((player->GetHdl<TransformComp>()->pos.y - t->pos.y) * 0.5f);
					Vec3 Dir = Vec3(t->pos.x, midGround + t->pos.y, t->pos.z);
					p->player_cam.LookAt(Dir);
				}

			}
		}
	}

}
void MoveInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Filename", path);
	sr.Write("Range", range);
	sr.Write("Pushable", Pushable);
	sr.Write("Pullable", Pullable);
	sr.Write("LockedPos", lockedPositions);
	sr.Write("Static", StaticObject);
	sr.Write("Forward", Forward);
	sr.Write("Distance", Distance);
	sr.Write("CurrDistance", CurrDistance);
	sr.Write("side", side);

  Interact::Serialize(sr, idx);
}

void MoveInteract::Deserialize(Deserializer& d)
{
	d.Read("Filename", path);

	d.Read("Range", range);
	d.Read("Pushable", Pushable);
	d.Read("Pullable", Pullable);
	d.Read("LockedPos", lockedPositions);
	d.Read("Static", StaticObject);
	d.Read("Forward", Forward);
	d.Read("Distance", Distance);
	d.Read("CurrDistance", CurrDistance);
	d.Read("side", side);
	RefreshHintFile();
  Interact::Deserialize(d);
}
void MoveInteract::RefreshHintFile()
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
void MoveInteract::Reflect(const Reflector& r)
{
	ImGui::Checkbox("Pushable", &Pushable);
	ImGui::Checkbox("Pullable", &Pullable);
	ImGui::Checkbox("Locked Positions", &lockedPositions);
	ImGui::Checkbox("Static Object", &StaticObject);
	if (StaticObject)
	{
		ImGui::Checkbox("Forward", &Forward);
		if (Forward)
		{
			ImGui::SliderFloat("Distance", &Distance, 0.0f, 100.0f);
		}
		else
		{
			ImGui::SliderFloat("Distance", &Distance, -100.0f, 0.0f);
		}
		ImGui::SliderInt("Which Side", &side, 0, 3);
	}

	ImGui::InputFloat("Range", &range);
	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}

  Interact::Reflect(r);


}

void MoveInteract::OnDestroy()
{

}

void MoveInteract::OnCollisionEnter(GOHandle other)
{
}

void MoveInteract::OnCollisionStay(GOHandle other)
{

}

void MoveInteract::OnCollisionExit(GOHandle other)
{
}

void MoveInteract::HandleMessage(MsgData_Base* m)
{
}