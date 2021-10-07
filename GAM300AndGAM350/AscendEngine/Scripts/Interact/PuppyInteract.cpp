#include "pch.h"

PuppyInteract::PuppyInteract()
{
    ASC_SETUP_SCRIPT(PuppyInteract);
}
void PuppyInteract::InteractAction()
{
    activated = false;
    g->isHighlighted = false;
    showDialog = true;
    PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
    p->canMove = false;
    usedMe = true;
    Vec3 puppyheightCam = p->player_cam.GetPos();
    puppyheightCam.y = t->pos.y;
    Vec3 forward = puppyheightCam - t->pos;
    Vec3 right = forward.Cross(Vec3::YAXIS);
    t->rotation = QuaternionFromMatrix(Mat4::LookAt({}, forward, right));
    
    ASC_SOUNDMAN.FadeTracks(p->GetGOHandle()->GetHdl<SoundComp>(),ASC_SOUNDMAN.currentBGM, "BGM_TalkingMusic.ogg");

    pup = GetGOHandle();
    //pup = ASC_FACTORY.GetGameObjectByName(dialogsFile);
    if(dialogsFile != "PuppyDialog_Charley_1"
        && dialogsFile != "PuppyDialog_Harley_1"
        && dialogsFile != "PuppyDialog_Harley_2")
        ASC_AMAN.ChangeModel(pup->name, "wagging", 4);
    else
        ASC_AMAN.ChangeModel(pup->name, "ears", 3);
    // increase when taken
    // puppyCollected++;
    
    // display right number
    // for (int i = 0; i <= 4; i++)
    // {
    //    Number[i]->GetHdl<GraphicsComp>()->SetVisible(false);
    // }

    //// Number[puppyCollected]->GetHdl<GraphicsComp>()->SetVisible(true);
    //if (linkToOtherMission)
    //{
    //  ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
    //}
    //else
    //{
    // // ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
    //}
}
void PuppyInteract::OnStart()
{
    ASC_INPUT.DisplayCursor(false);
    player = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
    dialogText = ASC_FACTORY.GetGameObjectByName("Dialog");
    Fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");
    dialogBG = ASC_FACTORY.GetGameObjectByName("dialogBg");
    dialogDog = ASC_FACTORY.GetGameObjectByName("dialogDog");
    dialogDogname = ASC_FACTORY.GetGameObjectByName("dialogDogname");
    dialogMouse = ASC_FACTORY.GetGameObjectByName("dialogMouse");
    
   
    if (!Fade.IsValid())
        ASC_FACTORY.CreateGOfromPB("FadeScreen");
    if (!dialogText.IsValid())
    {
        ASC_FACTORY.CreateGOfromPB("Dialog");
    }
    showDialog = false;
    DialogFinished = false;
    positionToMove = Vec3::ZERO;
    ASC_NEWPHYSICSMANAGER.GenerateBoundingBox(np->obj_phy, ASC_GETCOMP_PTR(TransformComp, goHdl), g->modelfile);
    
    barkTime = 4.0f;
    currentIndex = 0;
    if (positions.size() > 0)
        t->pos = OrigPos = ASC_FACTORY.GetGameObjectByName(positions[currentIndex])->GetHdl<TransformComp>()->pos;
    // set all to false except 0
    else
        OrigPos = t->pos;
    if (textures.size() > 0)
    {
        g->material.AlbedoMap = textures[currentIndex];
    }
    RefreshDialogList();
    RefreshDialogFile();
    
    //for (int i = 0; i <= 4; i++)
    //{
    //    std::string num = std::to_string(i);
    //    Number[i] = ASC_FACTORY.GetGameObjectByName("PuppyNumber" + num);
    //    Number[i]->GetHdl<GraphicsComp>()->SetVisible(false);
    //}
    //Number[puppyCollected]->GetHdl<GraphicsComp>()->SetVisible(true);
    if (disableAtStart)
    {
      GetGOHandle()->SetEnable(false);
    }
    if(dialogBG.IsValid())
      dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);


    if (dialogDog.IsValid())
        dialogDog->GetHdl<GraphicsComp>()->SetVisible(false);

    if (dialogDogname.IsValid())
        dialogDogname->GetHdl<GraphicsComp>()->SetVisible(false);
    if (!ASC_ENGINE.GetUsingMouse())
    {
        dialogMouse->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_Controller_LeftClick_albedo";
        dialogMouse->GetHdl<GraphicsComp>()->SetColor3(Vec4{ 1.0f,1.0f,1.0f,0.0f });
    }
    
}

void PuppyInteract::Update(float dt)
{
  if (ASC_KEYDOWN(ASC_KEY_L) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::X))
  {
    InteractAction();

  }

    t->pos.x = OrigPos.x;
    t->pos.z = OrigPos.z;

    if (usedMe)
    {
        if (!showDialog)
        {
            dialogText->GetHdl<UITextComp>()->Enabled = false;
            dialogText->GetHdl<UITextComp>()->tint.w = 255.0f;
            dialogText->GetHdl<UITextComp>()->offset.x = 0.0f;
            //dialogText->GetHdl<UITextComp>()->scale = 0.8f;
            if (dialogBG.IsValid())
              dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
            if(dialogDog.IsValid())
                dialogDog->GetHdl<GraphicsComp>()->SetVisible(false);

            if (dialogDogname.IsValid())
                dialogDogname->GetHdl<GraphicsComp>()->SetVisible(false);
            if (dialogMouse.IsValid())
                dialogMouse->GetHdl<GraphicsComp>()->SetVisible(false);
        }
        else
        {
            dialogText->GetHdl<UITextComp>()->Enabled = true;
            
            if (dialogBG.IsValid())
              dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
            if (dialogMouse.IsValid())
                dialogMouse->GetHdl<GraphicsComp>()->SetVisible(true);
        }
        if (showDialog)
        {
           
           // p->player_cam.LookAt(t->pos);
            // skipping dialog
            /*if (player->IsInteractButtonPress() && numLines < dialog.size())
            {
            
              //numLines++;
              time = 2.0f;
              if (numLines >= dialog.size())
              {
                showDialog = false;
                numLines = 0;
                time = 0.0;
                DialogFinished = true;
                fadeTime = 2.0;
                ASC_SOUNDMAN.Stop(s);
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk1.ogg");
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk2.ogg");
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk3.ogg");
                ASC_SOUNDMAN.Stop(s);
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk1.ogg");
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk2.ogg");
                ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk3.ogg");

                dialogText->GetHdl<UITextComp>()->SetText("");
              }
              return;
            }*/
       
            if (player->IsInteractButtonPress() && numLines < dialog.size() ) //&& time<=0.0)              
            {
                dialogText->GetHdl<UITextComp>()->SetText(dialog[numLines].second);
                //dialogText->GetHdl<UITextComp>()->offset.x = -((dialog[numLines].second.length() - 1) / 10.5f);
               
                if (!dialog[numLines].first)
                {

                    dialogText->GetHdl<UITextComp>()->offset.x = 0.35f - (dialog[numLines].second.length() * 0.05f);
                    ASC_SOUNDMAN.Stop(s);
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk1.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk2.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk3.ogg");
                    std::string STP = "SFX_PuppyTalk" + std::to_string((rand() % 3) + 1) + ".ogg"; 
                    np->obj_phy->BodyAddForce(Vec3(0, 3000, 0));
                    ASC_PLAY(STP);
                    dialogText->GetHdl<UITextComp>()->tint.r = 0.94901961f;
                    dialogText->GetHdl<UITextComp>()->tint.g = 0.96470588f;
                    dialogText->GetHdl<UITextComp>()->tint.b = 0.81568629f;
                    //to avoid going out of subscript if one guy forget to do something
                    (numLines < nameToSoundFileName.size()) ? ASC_PLAY(nameToSoundFileName[numLines],GetGOHandle()) :false ;
                    
                    dialogDog->GetHdl<GraphicsComp>()->SetVisible(true);
                    dialogDogname->GetHdl<GraphicsComp>()->SetVisible(true);
                   
                }
                else
                {

                    dialogText->GetHdl<UITextComp>()->offset.x =  - (dialog[numLines].second.length() * 0.047f);
                    ASC_SOUNDMAN.Stop(s);
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk1.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk2.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk3.ogg");
                    std::string STP = "SFX_DogTalk" + std::to_string((rand() % 3) + 1) + ".ogg";
                    ASC_PLAY_FROMGO(STP, ASC_FACTORY.GetObjectHandleList(go_player)[0], 0);
                    dialogText->GetHdl<UITextComp>()->tint.r = 1.0f;
                    dialogText->GetHdl<UITextComp>()->tint.g = 0.96470588f;
                    dialogText->GetHdl<UITextComp>()->tint.b = 0.9254902f;
                    (numLines < nameToSoundFileName.size()) ? ASC_PLAY(nameToSoundFileName[numLines], GetGOHandle()) : false;
                    
                    dialogDog->GetHdl<GraphicsComp>()->SetVisible(false);
                    dialogDogname->GetHdl<GraphicsComp>()->SetVisible(false);
                    

                }
                dialogText->GetHdl<UITextComp>()->tint.w = 255.0f;
                if (numLines == dialog.size() - 1)
                {
                   time= (numLines < nameToSoundFileName.size()) ? ASC_SOUNDMAN.GetTrackLength(nameToSoundFileName[numLines]) : 4.0f;
                    
                   
                }
                numLines++;
                //Timer
               
            }
            else if(player->IsInteractButtonPress() && numLines == dialog.size())
            {
                
                    ASC_SOUNDMAN.Stop(s);
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk1.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk2.ogg");
                    ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), "SFX_DogTalk3.ogg");
                    showDialog = false;
                    numLines = 0;
                    DialogFinished = true;
                    fadeTime = 2.0;
                    dialogText->GetHdl<UITextComp>()->SetText("");

                    dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
                    dialogDog->GetHdl<GraphicsComp>()->SetVisible(false);
                    dialogDogname->GetHdl<GraphicsComp>()->SetVisible(false);
                    dialogMouse->GetHdl<GraphicsComp>()->SetVisible(false);
                    ASC_SOUNDMAN.FadeTracks(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(),"BGM_TalkingMusic.ogg", ASC_SOUNDMAN.currentBGM);
                    
                }
                if (numLines < nameToSoundFileName.size())
                {
                 // voiceStop !=  ASC_SOUNDMAN.IsPlaying(s, nameToSoundFileName[numLines]);

                }

            
        }
        if (DialogFinished)
        {
            if(endDog)
            {
                
                ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
            }
            else
            {
                DirectorTrigger* dir = ASC_GETSCRIPT_PTR(DirectorTrigger, goHdl);
                CameraTrigger* ct = ASC_GETSCRIPT_PTR(CameraTrigger, goHdl);
                if (ct != nullptr)
                {

                    fadeout = false;
                    fadeTime = 0.0f;
                    DialogFinished = false;
                    activated = false;
                    moving = false;
                    usedMe = false;
                    if (dialogsFile == "PuppyDialog_Barley_1")
                        ASC_AMAN.ChangeModel(pup->name, "ears", 3);
                    // Number[puppyCollected]->GetHdl<GraphicsComp>()->SetVisible(true);
                    if (linkToOtherMission)
                    {
                        ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
                    }
                    ct->StartMovements();

                }
                else
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
                            currentIndex++;
                            //Last position
                            if (currentIndex >= static_cast<int>(positions.size()))
                            {
                                g->SetVisible(false);
                                np->obj_phy->GeomCollisionSetEnable(false);
                                np->obj_trigger->GeomCollisionSetEnable(false);
                                activated = false;
                            }
                            //next position and Dialog
                            else
                            {
                                t->pos = OrigPos = ASC_FACTORY.GetGameObjectByName(positions[currentIndex])->GetHdl<TransformComp>()->pos;
                                RefreshDialogFile();
                                PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
                                Vec3 puppyheightCam = p->player_cam.GetPos();
                                puppyheightCam.y = t->pos.y;
                                Vec3 forward = puppyheightCam - t->pos;
                                Vec3 right = forward.Cross(Vec3::YAXIS);
                                t->rotation = QuaternionFromMatrix(Mat4::LookAt({}, forward, right));
                                g->materialList[0].AlbedoMap = textures[currentIndex];
                                g->SetVisible(false);
                                g->SetVisible(true);

                            }
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
                        else// when fade done
                        {
                            fadeout = false;
                            DialogFinished = false;
                            fadeTime = 0.0;
                            moving = false;
                            PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
                            p->canMove = true;
                            usedMe = false;
                            //activated = false;

                            // Number[puppyCollected]->GetHdl<GraphicsComp>()->SetVisible(true);
                            if (linkToOtherMission)
                            {
                                ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
                            }
                            else
                            {
                                // ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_PART_DONE, GetGOHandle()));
                            }
                        }
                    }
                }
                if (dir != nullptr)
                {
                    dir->StartDirector();
                }
            }
        }

    }
    else
    {
        if (barkTime <= 0.0 && activated)
        {
            barkTime = 15.0f;
            std::string STP = "SFX_PuppyBark" + std::to_string(currentIndex+1) + ".ogg";
           // std::cout << goHdl->name;
            if (s.IsValid() && !ASC_SOUNDMAN.IsPlaying(s, STP))
            {

                ASC_PLAY(STP);
            }
           // std::cout << goHdl->name << std::endl;
            emitParticle = true; 
        }
        else
        {
            //std::cout << ASC_SOUNDMAN.IsPlaying(s, "SFX_PuppyBark1.ogg") << std::endl;
            barkTime -= dt;
        }
    }
    if (emitParticle)
    {
    //  std::cout << barkInterval / emitTimes << " ";
    //  std::cout << (int)barkTime % emitTimes << std::endl;
      if ( emitTimes == (int)barkTime% (emitTimes+1))
      {
        if (pe.IsValid())
        {
          pe->Emit();
        }
        emitedAtm++;
      }
      if (emitedAtm >= emitTimes)
      {
        emitParticle = false;
        emitedAtm = 0;
      }
    }


}
void PuppyInteract::Serialize(Serializer& sr, LogicHandle idx)
{
    sr.Write("linkToOtherMission", linkToOtherMission);
    sr.Write("MissionNum", missionNum);
    sr.Write("MissionLvl", misionlvl);
    sr.Write("targetToEnable", targetToEnable);
  /*  ImGui::Separator();*/
    sr.Write(GetName());
    sr.Write("Filename", path);
    sr.Write("dialogFile", dialogsFile);
    sr.Write("positionsFile", positionsFile);
    sr.Write("Activated", activated);
    sr.Write("Range", range);
    sr.Write("Textures", textureFile);
    sr.Write("AmtTime", amtOfTimes);

    sr.Write("disableAtStart", disableAtStart);
    sr.Write("endDog", endDog);
    Interact::Serialize(sr, idx);
    //

}

void PuppyInteract::Deserialize(Deserializer& d)
{
    d.Read("linkToOtherMission", linkToOtherMission);
    d.Read("MissionNum", missionNum);
    d.Read("MissionLvl", misionlvl);
    d.Read("targetToEnable", targetToEnable);

    d.Read("Filename", path);
    d.Read("dialogFile", dialogsFile);
    d.Read("positionsFile", positionsFile);
    d.Read("Activated", activated);
    d.Read("Range", range);

    d.Read("AmtTimes", amtOfTimes);
    d.Read("disableAtStart", disableAtStart);
    d.Read("AmtTime", amtOfTimes);
    d.Read("Textures", textureFile);

    d.Read("endDog", endDog);
    RefreshHintFile();
   // RefreshDialogList();
    RefreshPositionFile();
    RefreshDialogFile();
    RefreshTextures();

    Interact::Deserialize(d);
}

void PuppyInteract::RefreshTextures()
{
    textures.clear();
    std::ifstream cFile;
    std::string test = ASC_TEXT_FILE;// + path + ".txt";
    if (textureFile != "")
    {
        test += textureFile.c_str();
        test += ".txt";
        cFile.open(test);
        // Execute a loop until EOF (End of File) 
        while (cFile)
        {
   
            std::string line;
            std::getline(cFile, line);
            if (line != "")
            {
                textures.push_back(line);
            }
        }
        cFile.close();
    }
}
void PuppyInteract::RefreshHintFile()
{
    hint.clear();
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

void PuppyInteract::RefreshPositionFile()
{
    positions.clear();
    std::ifstream position;
    std::string test = ASC_TEXT_FILE;// + path + ".txt";
    test += positionsFile.c_str();
    test += ".txt";
    position.open(test);
    // Execute a loop until EOF (End of File) 
    while (position)
    {
        std::string line;
        // Read a Line from File 
        std::getline(position, line);
        if (line != "")
        positions.push_back(line);
    }
    position.close();

}
void PuppyInteract::RefreshDialogList()
{
    dialogs.clear();
    std::ifstream d;
    std::string test = ASC_TEXT_FILE;// + path + ".txt";
    test += dialogsFile.c_str();
    test += ".txt";
    d.open(test);
    // Execute a loop until EOF (End of File) 
    while (d)
    {
        std::string line;
        // Read a Line from File 
        std::getline(d, line);
        if (line != "")
        dialogs.push_back(line);
    }
    d.close();

}
void PuppyInteract::RefreshDialogFile()
{
    dialog.clear();
    std::ifstream dialogFile;
    std::string test = ASC_TEXT_FILE;// + path + ".txt";
   // if (dialogs.size() > 0)
  //  {
       // dialogPath = dialogs[currentIndex];
        test += dialogsFile.c_str();
        test += ".txt";
        dialogFile.open(test);
        // Execute a loop until EOF (End of File) 
        while (dialogFile)
        {
            std::string line;
            // Read a Line from File 
            std::getline(dialogFile, line);
            if (line == "Me")
            {
                std::getline(dialogFile, line);
                dialog.push_back(std::pair<bool, std::string>(true, line));
            }
            else if (line == "fucker")
            {
                std::getline(dialogFile, line);
                dialog.push_back(std::pair<bool, std::string>(false, line));
            }

        }
        dialogFile.close();
   // }
}
void PuppyInteract::Reflect(const Reflector& r)
{
    r.Reflect("disableAtStart", disableAtStart);
    ImGui::Separator();
    r.Reflect("linkToOtherMission", linkToOtherMission);
    r.Reflect("MissionNum", missionNum);
    r.Reflect("MissionLvl", misionlvl);
    r.Reflect("AmtTime", amtOfTimes);
    r.Reflect("targetToEnable", targetToEnable);
    ImGui::Separator();
    ImGui::InputText("Hint File", &path[0],128);
    ImGui::Text("Current Hint File");
    ImGui::Text(&path[0], ".txt");
    if (ImGui::Button("Refresh Hint File", ImVec2(200, 50)))
    {
        RefreshHintFile();
    }

    ImGui::InputText("Positions File", &positionsFile[0], 128);
    ImGui::Text("Current Positions File");
    ImGui::Text(&positionsFile[0], ".txt");
    if (ImGui::Button("Refresh Positions File", ImVec2(200, 50)))
    {
        RefreshPositionFile();
    }

    ImGui::InputText("Dialogs File", &dialogsFile[0], 128);
    ImGui::Text("Current Dialogs File");
    ImGui::Text(&dialogsFile[0], ".txt");
    if (ImGui::Button("Refresh Dialogs File", ImVec2(200, 50)))
    {
        RefreshDialogList();
    }

    ImGui::InputText("Colours File", &textureFile[0], 128);
    ImGui::Text("Current Color File");
    ImGui::Text(&textureFile[0], ".txt");
    if (ImGui::Button("Refresh Color File", ImVec2(200, 50)))
    {
        RefreshTextures();
    }
    ImGui::InputFloat("Range", &range);
    ImGui::Checkbox("Last Dog?", &endDog);
    Interact::Reflect(r);

}

void PuppyInteract::OnDestroy()
{

}

void PuppyInteract::OnCollisionEnter(GOHandle other)
{
}

void PuppyInteract::OnCollisionStay(GOHandle other)
{

}

void PuppyInteract::OnCollisionExit(GOHandle other)
{
}

void PuppyInteract::HandleMessage(MsgData_Base* m)
{
}