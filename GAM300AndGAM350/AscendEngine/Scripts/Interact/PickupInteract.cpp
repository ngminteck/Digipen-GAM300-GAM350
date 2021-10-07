#include "pch.h"
PickupInteract::PickupInteract()
{
	ASC_SETUP_SCRIPT(PickupInteract);
}
void PickupInteract::InteractAction()
{
	if (activated)
	{
		if (!usedMe)
		{
			if (prompt)
			{
				if (ASC_ENGINE.GetUsingMouse())
					py->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_Drop_albedo";
				else
					py->prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_Controller_Drop_albedo";
				py->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(1.0f);
			}
			if (InteractSoundOn != "")
			{
				ASC_PLAY(InteractSoundOn.c_str());
			}
			py->holdingItem = true;
			usedMe = true;
			PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
			//Remove keys from keyhole
			if (pz != nullptr && pz->inKeyHole)
			{
				pz->inKeyHole = false;
				PuzzleLock* pl = ASC_GETSCRIPT_PTR(PuzzleLock, pz->ParentPuzzleObject);
				if (pl != nullptr)
					pl->Objects[pl->GetKeyIndex(pz->GetGOHandle())].inplace = false;
				if (otherObj.IsValid())
				{
					PuzzleKeyHole* pH = ASC_GETSCRIPT_PTR(PuzzleKeyHole, otherObj);
					if (pH != nullptr)
					{
						pH->currentInsertedKey.AssignNewInValid();
					}
				}
			}
			///
			s = py->player_cam.GetPos();
			d = t->pos;
			origRot = QuaternionToEuler(t->rotation);
			t->pos = player->GetHdl<TransformComp>()->pos;
			np->obj_phy->GeomCollisionSetEnable(false);
			//np->obj_trigger->GeomCollisionSetEnable(false);
			np->obj_phy->BodySetGravityEnable(false);
			np->obj_phy->SetRigidBodyEnable(false);
			player->GetHdl<NewPhysicsComp>()->obj_phy->ignore_collision_list.push_back(t->GetGOHandle()->name);
			player->GetHdl<NewPhysicsComp>()->obj_floor_ray->ignore_collision_list.push_back(t->GetGOHandle()->name);
			if (pe.IsValid())
			{
				pe->Enabled = false;
			}

		}
		else
		{
			PuzzleKeyHole* pH = nullptr;
			FakePlacement* fP = nullptr;
			BaseTrigger* bT = nullptr;
			PuzzleKey* pK = nullptr;
			if (otherObj.IsValid())
			{
				pH = ASC_GETSCRIPT_PTR(PuzzleKeyHole, otherObj);
				fP = ASC_GETSCRIPT_PTR(FakePlacement, otherObj);
				bT = ASC_GETSCRIPT_PTR(BaseTrigger, otherObj);
				pK = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
			}

			if (inTrigger)
			{
				if (InteractTriggerSound != "")
				{
					ASC_PLAY(InteractTriggerSound.c_str());
				}
				//PuzzleKeyHole* pH = ASC_GETSCRIPT_PTR(PuzzleKeyHole, otherObj);

				std::cout << otherObj->name << std::endl;
				if (pH != nullptr)
				{
					if (!pH->currentInsertedKey.IsValid())
					{
						PlaceItem(pH->Keypos, pH->KeyRot);
						PuzzleKey* oth = ASC_GETSCRIPT_PTR(PuzzleKey, pH->ExpectedKey);
						pK->localPos = oth->localPos;
						pH->currentInsertedKey = goHdl;
						usedMe = false;
						py->holdingItem = false;
					}
				}
				else if (fP != nullptr)
				{
					//FakePlacement* fP = ASC_GETSCRIPT_PTR(FakePlacement, otherObj);
					if (fP != nullptr)
					{
						if (fP->correctObject.IsValid())
						{
							if (fP->correctObject->name.c_str() == goHdl->name.c_str() && nameofSinkBody == "")
							{
								bT->ObjectPlaceCorrectly();
								PlaceItem(placeLoc, placeRot);
								np->obj_trigger->GeomCollisionSetEnable(false);
								usedMe = false;
								py->holdingItem = false;
								fP->fakeObject->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);

								ASC_PLAY("SFX_CorrectAction.ogg");
							}
							else if (nameofSinkBody == "")
							{
								fP->ShowText("I think this belongs somewhere else..");
							}
              // this else if statement should only be called by the plug(for sink)
              if ( (! CheckTheSink()) && nameofSinkBody == "MissionWaterBody")
              {
                bT->ObjectPlaceCorrectly();
                PlaceItem(placeLoc, placeRot);

                np->obj_trigger->GeomCollisionSetEnable(false);
                usedMe = false;
                py->holdingItem = false;
                fP->fakeObject->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                ReadyTheSink();
              }
              else if  (nameofSinkBody == "MissionWaterBody")
              {
                fP->ShowText("I can't see where is the hole.");
              }
						}
					}
				}
				else if (bT != nullptr) // only has trigger for some reason
				{
     
            bT->ObjectPlaceCorrectly();
            PlaceItem(placeLoc, placeRot);
            np->obj_trigger->GeomCollisionSetEnable(false);
            usedMe = false;
            py->holdingItem = false;

				}
			}
			else
			{
				usedMe = false;
				py->holdingItem = false;
				DropPos = t->pos;
				t->rotation = py->GetGOHandle()->GetHdl<TransformComp>()->rotation;
				t->rotation *= EulerToQuaternion(GrabRot);
				np->obj_phy->GeomCollisionSetEnable(true);
				np->obj_phy->BodySetGravityEnable(true);
				np->obj_phy->SetRigidBodyEnable(true);
				g->isHighlighted = false;
				if (InteractSoundOff != "")
				{
					ASC_PLAY(InteractSoundOff.c_str());
				}


			}
			if (!py->holdingItem)//player drop 
			{
				if (prompt)
					py->prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
				g->isHighlighted = false;
				if (player->GetHdl<NewPhysicsComp>()->obj_phy->ignore_collision_list.size() > 0)
					player->GetHdl<NewPhysicsComp>()->obj_phy->ignore_collision_list.pop_back();
				//if (player->GetHdl<NewPhysicsComp>()->obj_floor_ray->ignore_collision_list.size() > 0)
				//{
				//	player->GetHdl<NewPhysicsComp>()->obj_floor_ray->ignore_collision_list.pop_back();
				//}
				//  np->obj_phy->ignore_collision_list.pop_back();
				np->obj_trigger->GeomCollisionSetEnable(true);
				py->lastInteract.AssignNewInValid();
			}
		}
	}
}
void PickupInteract::OnStart()
{
	player = ASC_FACTORY.GetObjectHandleList(go_player)[0];
	DropPos = t->pos;
	py = ASC_GETSCRIPT_PTR(PlayerController, player);
	ReadFromPosFile();
	//clue = ASC_FACTORY.GetGameObjectByName("ClueSmell");
	//dest = ASC_FACTORY.GetGameObjectByName("DestinationSmell");


	//clue->GetHdl<ParticleEmitter>()->pauseSimulation = true;
	//dest->GetHdl<ParticleEmitter>()->pauseSimulation = true;
	
}
void PickupInteract::Update(float dt)
{
	if (!usedMe && activated)
	{
		//if(clue.IsValid() && useParticle)
		//clue->GetHdl<TransformComp>()->pos = t->pos;
		t->pos.x = DropPos.x;
		t->pos.z = DropPos.z;
		

	}

		UpdateDest(dt);
		UpdateClue(dt);

}
void PickupInteract::AssignSmells()
{
	clue = ASC_FACTORY.GetGameObjectByName("ClueSmell");
	dest = ASC_FACTORY.GetGameObjectByName("DestinationSmell");

}
void PickupInteract::UpdateDest(float dt)
{
	if(dest.IsValid())
{
	if (dest->GetHdl<ParticleEmitter>()->countdown < 0.0)
	{
		dest->GetHdl<ParticleEmitter>()->maxLifetime = dest->GetHdl<ParticleEmitter>()->minLifetime = 0;
		dest.AssignNewInValid();
	}
	else
	{
		std::cout << dest->GetHdl<ParticleEmitter>()->countdown << std::endl;
		dest->GetHdl<TransformComp>()->pos = placeLoc;
		dest->GetHdl<ParticleEmitter>()->countdown -= dt;
	}
}
}
void PickupInteract::UpdateClue(float dt)
{
	//if (clue.IsValid())
	//{
	//	if (clue->GetHdl<ParticleEmitter>()->countdown < 0.0)
	//	{
	//		clue->GetHdl<ParticleEmitter>()->maxLifetime = clue->GetHdl<ParticleEmitter>()->minLifetime = 0;
	//		clue.AssignNewInValid();
	//	}
	//	else
	//	{
	//		//std::cout << clue->GetHdl<ParticleEmitter>()->countdown << std::endl;
	//		clue->GetHdl<TransformComp>()->pos = t->pos;
	//		clue->GetHdl<ParticleEmitter>()->countdown -= dt;
	//	}
	//}
}
void PickupInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Filename", path);
	sr.Write("Range", range);
	sr.Write("GrabRot", GrabRot);
	sr.Write("UseParticle", useParticle);
  sr.Write("isItforCollectingWater", isItforCollectingWater);
  sr.Write("nameofSinkBody", nameofSinkBody);

  Interact::Serialize(sr, idx);
}

void PickupInteract::Deserialize(Deserializer& d)
{
	d.Read("Filename", path);
	d.Read("Range", range);
	d.Read("GrabRot", GrabRot);
	d.Read("UseParticle", useParticle);

	ReadFromPosFile();
  d.Read("isItforCollectingWater", isItforCollectingWater);
  d.Read("nameofSinkBody",nameofSinkBody);

  Interact::Deserialize(d);
}

void PickupInteract::Reflect(const Reflector& r)
{

	ImGui::InputFloat("Range", &range);
	ImGui::InputText("Hint File", &path[0], 128);
	ImGui::Text("Current Hint File");
	ImGui::Text(&path[0], ".txt");

	if (goHdl.IsValid())
	{
		if (placeLoc != Vec3::ZERO)
		{
			std::string text = std::to_string(placeLoc.x) + " " + std::to_string(placeLoc.y) + " " + std::to_string(placeLoc.z);
			ImGui::Text(&text[0]);
			ImGui::Checkbox("Use Particles for clue?", &useParticle);
		}
		if (placeRot != Vec3::ZERO)
		{
			std::string text = std::to_string(placeRot.x) + " " + std::to_string(placeRot.y) + " " + std::to_string(placeRot.z);
			ImGui::Text(&text[0]);
		}
		if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
		{
			RefreshHintFile();
		}
		if (ImGui::Button("Write To Pos File", ImVec2(200, 50)))
		{
			WriteToPosFile(true);
		}
		PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
		if (pz == nullptr)
		{
			if (ImGui::Button("Read From Pos File", ImVec2(200, 50)))
			{
				ReadFromPosFile();
			}
		}
		if (ImGui::Button("Set Placement Pos/Rot", ImVec2(200, 30)))
		{
			PlaceItem(placeLoc,placeRot);
		}
		if (ImGui::Button("Set Grab Rot", ImVec2(200, 30)))
		{
			GrabRot = QuaternionToEuler(t->rotation);
		}
    ImGui::Separator();
    ImGui::Text("water jar");
    r.Reflect("isItforCollectingWater", isItforCollectingWater);
    ImGui::Text("Sink's body name");
    r.Reflect("nameofSinkBody", nameofSinkBody);
    ImGui::Separator();
    Interact::Reflect(r);
	}
}
void PickupInteract::RefreshHintFile()
{
	if (path != "")
	{
		std::ifstream hintFile;
		hintFile.open(ASC_TEXT_FILE + path + ".txt");


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
}

void PickupInteract::WriteToPosFile(bool res)
{
	    placeRot = QuaternionToEuler(t->rotation);
		placeLoc = t->pos;
		std::ofstream posFile;
		posFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name  + ".pos", std::ofstream::out | std::ofstream::trunc);
		posFile << placeLoc.x << " " << placeLoc.y << " " << placeLoc.z << " " << '\n';
		posFile << placeRot.x << " " << placeRot.y << " " << placeRot.z << " " << '\n';
		posFile.close();

}
void PickupInteract::UseParticles(float countdown, bool destroy)
{
	if (useParticle)
	{
		myParticle = true;
		//if (clue.IsValid())
		//{
		//	clue->GetHdl<ParticleEmitter>()->pauseSimulation = false;
		//	clue->GetHdl<TransformComp>()->pos = t->pos;
		//	clue->GetHdl<ParticleEmitter>()->Emit();
		//	clue->GetHdl<ParticleEmitter>()->StartCountdown(countdown, destroy);
		//	clue->GetHdl<ParticleEmitter>()->maxLifetime = clue->GetHdl<ParticleEmitter>()->minLifetime = 1.0f;
		//	//clue->GetHdl<ParticleEmitter>()->c
		//}
		if (dest.IsValid())
		{
			dest->GetHdl<ParticleEmitter>()->pauseSimulation = false;
			dest->GetHdl<TransformComp>()->pos = placeLoc;
			dest->GetHdl<ParticleEmitter>()->Emit();
			dest->GetHdl<ParticleEmitter>()->StartCountdown(countdown + 3.0f, destroy);
			dest->GetHdl<ParticleEmitter>()->maxLifetime = dest->GetHdl<ParticleEmitter>()->minLifetime = 1.0f;
		}
	}

}
void PickupInteract::ReadFromPosFile()
{
	std::ifstream posFile;
	std::string line;
	char delim = ' ';
	std::vector<float> vals;
	size_t pos;
	float token;
	posFile.open(ASC_POS_FILE + ASC_ENGINE.CurrentScene() + '_' + GetGOHandle()->name + ".pos");
	if (posFile.is_open())
	{
		// Get the position
		std::getline(posFile, line);
		while ((pos = line.find(delim)) != std::string::npos) {
			token = std::stof(line.substr(0, pos));
			vals.push_back(token);
			line.erase(0, pos + 1);
		}
		placeLoc.x = vals[0];
		placeLoc.y = vals[1];
		placeLoc.z = vals[2];

		// Get the rotation
		vals.clear();
		std::getline(posFile, line);
		while ((pos = line.find(delim)) != std::string::npos) {
			token = std::stof(line.substr(0, pos));
			vals.push_back(token);
			line.erase(0, pos + 1);
		}
		placeRot.x = vals[0];
		placeRot.y = vals[1];
		placeRot.z = vals[2];
	}
	

}
void PickupInteract::PlaceItem(const Vec3& pos, const Vec3& rot)
{
	DropPos = t->pos = pos;
	t->rotation = EulerToQuaternion(rot);
	np->obj_phy->BodySetLinearVel(Vec3::ZERO);
	PuzzleKey* pz = ASC_GETSCRIPT_PTR(PuzzleKey, goHdl);
	if (pz != nullptr)
	{

		PuzzleLock* pl = ASC_GETSCRIPT_PTR(PuzzleLock, pz->ParentPuzzleObject);
		if (pl != nullptr)
		{
			if (pos == placeLoc)
			{
				pl->Objects[pl->GetKeyIndex(pz->GetGOHandle())].inplace = true;
				if(!pz->oneTime)
				pz->DetectScript(2);
			}
			if (pz->oneTime)
			{
				activated = false;
				np->obj_trigger->SetHaveBody(false);
				np->obj_trigger->GeomCollisionSetEnable(false);
			}
			pz->inKeyHole = true;
			np->obj_phy->GeomCollisionSetEnable(false);
			np->obj_phy->BodySetGravityEnable(false);
			
		}
	}
	else
		np->obj_phy->SetHaveBody(false);
}
void PickupInteract::OnDestroy()
{

}

void PickupInteract::OnCollisionEnter(GOHandle other)
{
	inTrigger = true;
	otherObj = other;

}

void PickupInteract::OnCollisionStay(GOHandle other)
{
	inTrigger = true;
	otherObj = other;
}

void PickupInteract::OnCollisionExit(GOHandle other)
{
	inTrigger = false;
	otherObj.AssignNewInValid();
}

void PickupInteract::HandleMessage(MsgData_Base* m)
{
}

void PickupInteract::CollectWater()
{
 /* hasWater = true;
  std::cout << " water has collected\n";
  if (pe.IsValid())
  {
    pe->isLooping = true;
  }*/
}
void PickupInteract::DumbWater()
{
  //hasWater = false;
  //if (pe.IsValid())
  //{
  //  pe->isLooping = false;
  //}
}
void PickupInteract::ReadyTheSink()
{
  auto obj  =  ASC_FACTORY.GetGameObjectByName( nameofSinkBody);
  if (obj.IsValid())
  {
    auto sink = ASC_GETSCRIPT_PTR(WaterContainer,obj);
    if (sink != nullptr)
    {
      sink->activate = true;
    }
  }

}
//cannot plug if the sink is on 
bool PickupInteract::CheckTheSink()
{
  auto obj = ASC_FACTORY.GetGameObjectByName(nameofSinkBody);
  //std::cout << obj->name << " vs " << nameofSinkBody << std::endl;
  if (obj.IsValid())
  {
   // std::cout << "im in \n";
    auto sink = ASC_GETSCRIPT_PTR(WaterContainer, obj);
    if (sink != nullptr)
    {
      std::cout << sink->isOn;
      return sink->isOn;
    }
  }
  return false;
}