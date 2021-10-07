#include "pch.h"
DoorInteract::DoorInteract()
{
	ASC_SETUP_SCRIPT(DoorInteract);
}
int DoorInteract::Controls()
{
	
	return 0;
}
void DoorInteract::InteractAction()
{
  if (!lock)
  {


    usedMe = true;
    //std::cout << "I am a door" << std::endl;
    moving = true;
    fadeTime = 2.0;
    fadeout = false;
    PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
    p->canMove = false;
    activated = false;
    int i = ((rand() % 69) + 1);
    if (i == 69)
      ASC_PLAY("SFX_ChineseUnlock.ogg");
    else
      ASC_PLAY("SFX_DoorTrans.ogg");
  }
  else
  {
    //lock

    //change this to be message send to the player and player should handle it or 
    //someone else should handle it 
     textObj = ASC_FACTORY.GetGameObjectByName("Dialog");
    if (textObj.IsValid())
    {
      textFade = true; 
      textObj->GetHdl<UITextComp>()->Enabled = true;
      textObj->GetHdl<UITextComp>()->tint.w = 1.0f;
      textObj->GetHdl<UITextComp>()->SetText(textToDisplayForLock);

	  textObj->GetHdl<UITextComp>()->offset.x = -(textToDisplayForLock.length() * 0.047f);
    }

	dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
  }
}
void DoorInteract::OnStart()
{
	Fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");

	dialogBG = ASC_FACTORY.GetGameObjectByName("dialogBg");
	if (!Fade.IsValid())
		assert("Cannot find fadescreen"); 
	SetupSpawnPoints();

}

void DoorInteract::SetupSpawnPoints()
{
	if(pos1!= "")
	position1 = ASC_FACTORY.GetGameObjectByName(pos1);
	if(pos2!="")
	position2 = ASC_FACTORY.GetGameObjectByName(pos2);
	

}
void DoorInteract::TextUpdate(float dt)
{
  if (textFade)
  {
    if (textObj.IsValid() && dialogBG.IsValid())
    {
     
      if (textObj->GetHdl<UITextComp>()->tint.w > 0)
      {
        textObj->GetHdl<UITextComp>()->tint.w -= dt;
        dialogBG->GetHdl<GraphicsComp>()->tint.w -= dt;
		
      }
      else
      {
        textFade = false;

		dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
      }

    }
    //textFadeTime -= dt;
  }
}
void DoorInteract::Update(float dt)
{
	if (moving)
	{
		//fading in
		if (!fadeout)
		{
			if (fadeTime > 0.0)
			{
				fadeTime -= dt;
				Fade->GetHdl<GraphicsComp>()->SetTransparencyVal((1 - (fadeTime / 2)));
			}
			
			else
			{
				fadeout = true;
				fadeTime = 2.0;

				PlayerController * p =  ASC_GETSCRIPT_PTR(PlayerController,ASC_FACTORY.GetObjectHandleList(go_player)[0]);
				p->canMove = true;
				if (PositionOne)
					ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<TransformComp>()->pos = position2->GetHdl<TransformComp>()->pos;
				else
					ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<TransformComp>()->pos = position1->GetHdl<TransformComp>()->pos;
				PositionOne = !PositionOne;
				
			}
		}
		//fading out
		else
		{
			if (fadeTime > 0.0)
			{
				fadeTime -= dt;
				Fade->GetHdl<GraphicsComp>()->SetTransparencyVal((fadeTime / 2));
			}
			else
			{
				fadeout = false;
				fadeTime = 0.0;
				moving = false;
				PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
				p->canMove = true;
				usedMe = false;
				activated = true;
			}
		}
	}

	
  TextUpdate(dt);
}
void DoorInteract::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Filename", path);
	sr.Write("Pos1", pos1);
	sr.Write("Pos2", pos2);
	sr.Write("Range", range);
  sr.Write("lock",lock );

  sr.Write("doorMsg", textToDisplayForLock);


  Interact::Serialize(sr, idx);
}

void DoorInteract::Deserialize(Deserializer& d)
{
	d.Read("Filename", path);
	d.Read("Pos1", pos1);
	d.Read("Pos2", pos2);
	d.Read("Range", range);
  d.Read("lock", lock);

  d.Read("doorMsg", textToDisplayForLock);
  Interact::Deserialize(d);
	RefreshHintFile();
}
void DoorInteract::RefreshHintFile()
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
			if(line != "")
				hint.push_back(line);
		}
		hintFile.close();
}
void DoorInteract::Reflect(const Reflector& r)
{

  r.Reflect("displayMsg",textToDisplayForLock);
  r.Reflect("lock", lock);
  ImGui::Separator();
  //ImGui::Text(hint.c_str(), ImVec2(500, 100));
	ImGui::InputText("Filename", &path[0],128);
	ImGui::Text("Current File");
	ImGui::Text(&path[0],".txt");
	if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
	{
		RefreshHintFile();
	}
	ImGui::InputText("Spawn Pos 1", &pos1[0], 128);
	ImGui::InputText("Spawn Pos 2", &pos2[0], 128);
	ImGui::InputFloat("Range", &range);
	ImGui::Separator();
	
  Interact::Reflect(r);

}

void DoorInteract::OnDestroy()
{

}

void DoorInteract::OnCollisionEnter(GOHandle other)
{
}

void DoorInteract::OnCollisionStay(GOHandle other)
{

}

void DoorInteract::OnCollisionExit(GOHandle other)
{
}

void DoorInteract::HandleMessage(MsgData_Base* m)
{
}