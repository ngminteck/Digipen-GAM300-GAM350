/******************************************************************************/
/*!
\file   ObjectiveManager.cpp
\author Cai Wangyang
\par    email: wangyang.cai\@digipen.edu
\par    Digipen login: wangyang.cai
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"
#include "ObjectiveManager.h"

int currLvl = 0;
std::vector<std::string> levelname
{
  //"Lvl1_Realest",
  "Lv2_Realest",
  "Lv3_Realest",
  "MainMenu"
};
std::vector < std::string> BGMs
{
  "BGM_InGame.ogg",
  "BGM_level2.ogg",
  "BGM_level3.ogg",
};

//void ObjectiveManager::RegisterObjective()
//{
//
//}
float fadeTime = 2.0f;

void ObjectiveManager::GetObjects(const std::vector<GOTYPE>& vec)
{
  ////update the list of objects in objectiveMap
  //for (auto& type : vec)
  //{
  //  auto iter = objectiveMap.find(type);
  //  if (iter != objectiveMap.end())
  //  {
  //    ObjTypeItr& obj = (*iter).second;
  //    obj.ClearObj();
  //    ASC_FACTORY.GetObjectHandleList(obj, type);
  //  }
  //  else
  //    //not in objective list
  //    return;
  //}
}

ObjTypeItr & ObjectiveManager::GetObjectiveType(GOTYPE type)
{
  //ObjTypeItr& obj = objectiveMap[type];
  //ASC_FACTORY.GetObjectHandleList(obj, type);
  //return obj;


  return objectiveMap[type];
}

ObjectiveManager::ObjectiveManager()
  : ScriptCommonIF(ASC_GETSCRIPTNAME(ObjectiveManager))
{
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_REG_TO_AS_ONE_MISSION, HandleMessageRegObjects);
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_LINK_TO_AS_ONE_MISSION, HandleMessageRegObjects);

 // ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_MISSION_END, HandleMessageMissonComplete);
  ASC_SCRIPT_REGISTER_MSG(MsgType::mTYPE_MISSION_PART_DONE, HandleMessageMissonComplete);


}
void ObjectiveManager::HandleMessageRegObjects(MsgData_Base* b)
{
  Msg_Logic_Mission* realMsg = static_cast<Msg_Logic_Mission*> (b);
  auto infor = ASC_GETSCRIPT_INFOR_PTR(realMsg->gameObject);
  NewPhyHandle phyHandle = realMsg->gameObject->GetHdl<NewPhysicsComp>();
  //  static_cast<ObjInfor*>(b);
  switch (b->type)
  {
    case mTYPE_REG_TO_AS_ONE_MISSION:
    {
    //  if (infor->misionlvl == -1) return;
    //  //std::cout << infor->missionNum;
    //  LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);
    //  auto& missionData = levelData[lvl].find(infor->missionNum);
    //  //same mission number cannot exist 
    //  if (missionData == levelData[lvl].end())
    //  {
    //  
    //    auto group =  levelData[lvl][infor->missionNum];
    //    group.mapGameObj[ group.mapGameObj.size()].gameobj = realMsg->gameObject;      
    //  }
    //  else //same level was found 
    //  {
    //    //did not set any mission num yet so... just skip. 
    //    if (infor->missionNum != -1)
    //    {
    //      //same mission number cannot exist 
    //      std::cout << "same mission number cannot exist\n";
    //      assert(false);
    //      exit(0);
    //    }
    //  }

     break;
    }
    case mTYPE_LINK_TO_AS_ONE_MISSION :
    {
      if (infor->missionNum == -1) return;
      LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);

      //find the misisonData, if it doesnt exist it will auto create .
      auto& missionData= levelData[lvl][infor->missionNum];
      //add this object as ONE part of this mission number
      missionData.mapGameObj[ static_cast<int>( missionData.mapGameObj.size() )].gameobj = realMsg->gameObject;
      MissionData& group = levelData[static_cast<LEVELS>(0)][0];
      std::string text = "";
      //its assume you have many object in this misison 
      if (infor->amtOfTimes != -1)
      {
      
        text = std::to_string(group.numDone) + std::string(" / ") + std::to_string(infor->amtOfTimes);;

      }
      // it assume you have a single object needin to be interacted many times 
      else
      {
        text = std::to_string(group.numDone) + std::string(" / ") + std::to_string(group.mapGameObj.size());;
      }
      if (u.IsValid())
      {
        u->SetText(text);
      }

      break;
    }
  }
}

void ObjectiveManager::HandleMessageMissonComplete(MsgData_Base * b)
{
  Msg_Logic_Mission* realMsg = static_cast<Msg_Logic_Mission*> (b);
  auto infor = ASC_GETSCRIPT_INFOR_PTR(realMsg->gameObject);
 /* NewPhyHandle phyHandle = realMsg->gameObject->GetHdl<NewPhysicsComp>();*/
  switch (b->type)
  {
    case mTYPE_MISSION_PART_DONE:
    {
    
      if (infor->missionNum == -1) return;
      MissionData* obj = nullptr;
      if (SetAndCheckComplete(realMsg->gameObject,obj))
      {
        //pass to UImananger
        ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_MISSION_END,infor->missionNum));
        // current mission completed 
        //check if need to enable somemission 
        if (infor->targetToEnable!= -1)
        {
          LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);
       //   for (auto const& missonNum : infor->targetToEnable)
          {
            MissionData& group = levelData[lvl][infor->targetToEnable];
            for (auto& [num, curr] : group.mapGameObj)
            {
              curr.gameobj->SetEnable(true);
            }
          }
        }
        auto unlocks= unlockThese.find(infor->missionNum);
        //check to unlock something 
        if (unlocks != unlockThese.end())
        {
           auto obj = ASC_FACTORY.GetGameObjectByName(unlockThese[infor->missionNum]);
           if (obj.IsValid())
           {
              DoorInteract* inter = ASC_GETSCRIPT_PTR(DoorInteract,obj);
              if (inter != nullptr)
              {
                inter->lock = false;
              }
           }

        }

        //update text
        {
          LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);
          MissionData& group = levelData[lvl][infor->missionNum];

          int amtTimes = static_cast<int>( group.mapGameObj.size());
          if (infor->amtOfTimes != -1)
          {
            //todo
            amtTimes = infor->amtOfTimes;
          }
          std::string text = std::to_string(group.numDone) + std::string(" / ") + std::to_string(amtTimes);;
          //  text += " / " + group.mapGameObj.size();

          u->SetText(text);
        }
        //check if  all levels are completed 
        if (CheckIfAllMissionIsDone(realMsg->gameObject))
        {
        //hard code end the level 
          fadeTime = 2.0f;
          startFade = true;
       

          ////ASC_ENGINE.CurrentScene();
          //ASC_MSGSYS.SendMsg(Msg_Change_Scene(levelname[currLvl++]));
        }
        
      
      }
      else//not yet done 
      {
        LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);
        if (infor->missionNum == -1)return;
        MissionData& group = levelData[lvl][infor->missionNum];
        int amtTimes = static_cast<int>( group.mapGameObj.size() );
        if (infor->amtOfTimes != -1)
        {
          //todo
          amtTimes = infor->amtOfTimes;
        }
        std::string text = std::to_string(group.numDone) + std::string(" / ") + std::to_string(amtTimes);;
        u->SetText(text);
        

      }
       // infor
      break;
    }

   
  }
}

ObjectiveManager::~ObjectiveManager()
{
}

void ObjectiveManager::OnStart()
{   
  // reset this global value back to 0 
  if (currLvl >= levelname.size() || ASC_ENGINE.CurrentScene() == "MainMenu")
  {
    currLvl = 0;
  }

  player = ASC_FACTORY.GetObjectHandleList(go_player)[0]; 
#ifndef _SUBMISSION
  if (ASC_ENGINE.CurrentScene() == "Lv2_Realest")
      currLvl = 1;
  if (ASC_ENGINE.CurrentScene() == "Lv3_Realest")
     currLvl = 2;
#endif // _SUBMISSION


  //ASC_LOOP(BGMs[currLvl]);
  ASC_SOUNDMAN.Play(player->GetHdl<SoundComp>(), BGMs[currLvl], 0);
  ASC_SOUNDMAN.currentBGM = BGMs[currLvl];
  finalLvlName = "Lv3_Realest";
  //RegisterObjective();
  //GetObjects();

  Fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");
  if (!Fade.IsValid())
    assert("Cannot find fadescreen");
  //Must have narText
  NarrativeText = ASC_FACTORY.GetGameObjectByName("narText");
  dialogBG = ASC_FACTORY.GetGameObjectByName("dialogBg");
  RefreshNarrationFile();
  StartNarrtive(0);
  showHint = true;
  //  text += " / " + group.mapGameObj.size();

  if (u.IsValid())
  {
    u->Enabled = false;
  }
  if (g.IsValid())
  {
    g->posOffset.x = 0;
    g->Enabled = false;
    g->SetVisible(false);
  }

}
void ObjectiveManager::NarrativeUpdate( float dt )
{
  if (showNarration && NarrativeText.IsValid() && !narrativeLines.empty())
  {
    //std::cout << "hi" << std::endl;
    auto& currNar = narrativeLines[currNarrativeIndex];
    if (!showHint)
    {
      NarrativeText->GetHdl<UITextComp>()->Enabled = false;
      NarrativeText->GetHdl<UITextComp>()->tint.w = 255.0f;
      NarrativeText->GetHdl<UITextComp>()->offset.x = 0;
      NarrativeText->GetHdl<UITextComp>()->scale = 0.8f;
      //dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
      auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
      obj->canMove = true;
    }
    else
    {
      auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
      obj->canMove = false;
      NarrativeText->GetHdl<UITextComp>()->Enabled = true;

      //dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
    }
    if (currNar.size() > 0 && showHint)
    {
      auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
      //cannot skip
#ifndef _SUBMISSION
      if (obj->IsInteractButtonPress())
      {
        time = 0; 
      //  numLines++;
        if (numLines >= currNar.size())
        {
          showNarration = false;
          showHint = false;
          numLines = 0;
          time = 0.0;
          auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
          obj->canMove = true;

          NarrativeText->GetHdl<UITextComp>()->SetText("");
          dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
          return;
        }
       // return;
      }
#endif
      if (numLines < currNar.size() && time <= 0.0)
      {
        std::cout << currNar[numLines] << std::endl;
        NarrativeText->GetHdl<UITextComp>()->SetText(currNar[numLines]);
       
        NarrativeText->GetHdl<UITextComp>()->offset.x = -(currNar[numLines].length() * 0.047f);

        NarrativeText->GetHdl<UITextComp>()->tint.w = 255.0f;
        dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
        time = 3.0;
        numLines++;
      }
      else
      {
        //Timer
        if (time > 0.0)
        {
          time -= (dt);
          NarrativeText->GetHdl<UITextComp>()->tint.w = ((time / 3.0f));
          if((ASC_ENGINE.CurrentScene() == "Lv1_Realest" && numLines == 1  )|| numLines == currNar.size())
            dialogBG->GetHdl<GraphicsComp>()->tint.w = ((time / 3.0f));
          
        }
        if (numLines == currNar.size() && time <= 0.0f)
        {
          auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
          obj->canMove = true;
          showNarration = false;
          showHint = false;
          numLines = 0;
          time = 0.0;
          NarrativeText->GetHdl<UITextComp>()->SetText("");
          dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
        }

      }
    }
  }
}
void ObjectiveManager::Update(float dt)
{
  //for (auto& pair : objectiveMap)
  //{
  //  ObjTypeItr& obj = pair.second;
  //  obj.ClearObj();
  //  ASC_FACTORY.GetObjectHandleList(obj, pair.first);
  //}
  NarrativeUpdate(dt);
  CutSceneUpdate();
  if (startFade)
  {
    FadeBlack(dt);
  }
  if(ASC_KEYDOWN(ASC_KEY_P))
  {
    ASC_MSGSYS.SendMsg(Msg_Change_Scene(levelname[currLvl++]));

  }
  if (ASC_KEYDOWN(ASC_KEY_L))
  {
  

  }
  //instant end the level 
  if (ASC_KEYDOWN(ASC_KEY_O))
  {
    fadeTime = 2.0f;
    startFade = true;


  }
  //std::cout << ASC_ENGINE.CurrentScene() << std::endl;
}

void ObjectiveManager::CutSceneUpdate()
{
  if (startCutScene)
  {
    auto obj = ASC_GETSCRIPT_PTR(PlayerController, player);
    if (obj->IsJumpButtonPress() || obj->IsInteractButtonPress())
    {
      if (currCutSceneIndex < endingCutscenes.size())
      {
        g->materialList[0].AlbedoMap = endingCutscenes[currCutSceneIndex++];
        g->SetVisible(false);
        g->SetVisible(true);
      }
      //all cutscene has been play , now we will fade to black 
      else
      {
        ASC_SOUNDMAN.Stop(player->GetHdl<SoundComp>());
        ASC_MSGSYS.SendMsg(Msg_Change_Scene(levelname[currLvl++]));
       // g->SetVisible(false);
        //startFade = true;
      }
    }
  }
}
void ObjectiveManager::Serialize(Serializer & sr, LogicHandle idx)
{
  //sr.Write(GetName());
 
  sr.Write("a", endingCutscenes);
  sr.Write("NarFile", narrationText);
  sr.Write("unlockThese",unlockThese);
  sr.Write("NarrativeFiles", narrativeFiles);
}

void ObjectiveManager::Deserialize(Deserializer & d)
{

  d.Read("a", endingCutscenes);
  d.Read("NarFile", narrationText);
  d.Read("unlockThese", unlockThese);
  d.Read("NarrativeFiles", narrativeFiles);
 
}
void ObjectiveManager::RefreshNarrationFile()
{
  for (auto currNarFile : narrativeFiles)
  {
    std::ifstream hintFile;
    std::string test = ASC_TEXT_FILE;// + path + ".txt";
    test += currNarFile.c_str();
    test += ".txt";
    hintFile.open(test);
    std::vector<std::string> tempLine;
    // Execute a loop until EOF (End of File) 
    while (hintFile)
    {
      std::string line;
      // Read a Line from File 
      std::getline(hintFile, line);
      if(line != "")
         tempLine.push_back(line);
    }
    narrativeLines.push_back(std::move(tempLine));
    hintFile.close();
  }

}
void ObjectiveManager::Reflect(const Reflector & r)
{
  ImGui::Text("NarrativeFileNames");
  r.Reflect("NarrativeFileName", narrativeFiles);

  ImGui::Text("Ending cutScene 2d scene name");
  int i = 0;
  r.Reflect("endingCutscene", endingCutscenes, ASC_GMAN.GetTextureList());



  ImGui::Separator();
  

  ImGui::Text("Mission done to unlock door");
  static int blah = 0;
  
  if (ImGui::Button("add Mission unlock"))
  {
    unlockThese[unlockThese.size()] = "";
  }
  if (ImGui::Button("clearAll"))
  {
    unlockThese.clear();
  }

  ImGui::BeginChild("MissionStuff");
  ImGui::Columns(2);
  
  for (auto& [index, name] : unlockThese)
  {
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    std::string curr = name + std::to_string(index);
    if (r.Reflect(curr.c_str(), name))
    {
     
    }
    ImGui::NextColumn();
  }
  ImGui::EndChild();

  static int inputlvl = 0; static int inputNum = 0;
  ImGui::InputInt("lvl", &inputlvl); ImGui::InputInt("num",& inputNum);

  for (auto const& [lvl,data] : levelData)
  {
    auto currLvl = static_cast<int>(lvl);
    auto name  = "Curr Lvl : " + std::to_string(currLvl);
    ImGui::BeginChild(name.c_str(), ImVec2(0, ImGui::GetWindowSize().y*0.25f), true);
    ImGui::Text(name.c_str());
    ImGui::Separator();
    for (auto const& [num,missionData] : data)
    {
      auto currobj = name + std::to_string(num);
      std::string  currMissNum = "CurrMissNum : " + std::to_string(num);
      ImGui::Text( currMissNum.c_str());
      ImGui::SameLine();
      auto result = missionData.done;
      ImGui::Checkbox("done?", &result);
      ImGui::SameLine();
      ImGui::Separator();
      ImGui::BeginChild(currobj.c_str(),ImVec2(0,ImGui::GetWindowSize().y*0.2f),true);
      ImGui::Columns(3);
      for (auto const&[num,obj] : missionData.mapGameObj)
      {

          auto currNum= std::to_string(num);
          ImGui::Text(currNum.c_str());
       
         
          ImGui::NextColumn();
          ImGui::Text(obj.gameobj->name.c_str());
          ImGui::NextColumn();
          ImGui::Text( (obj.currDone)?"true":"false" );
          ImGui::NextColumn();
       /*   auto result = std::to_string(currNum);
          ImGui::Text(obj.);*/
      }
      ImGui::EndChild();
    }
    ImGui::EndChild();
  }

  ImGui::InputText("Filename", &narrationText[0], 128);
  ImGui::Text("Current File");
  ImGui::Text(&narrationText[0], ".txt");
  if (ImGui::Button("Refresh Narrative File", ImVec2(200, 50)))
  {
      RefreshNarrationFile();
  }
}

void ObjectiveManager::OnDestroy()
{
}

void ObjectiveManager::OnCollisionEnter(GOHandle h)
{
}

void ObjectiveManager::OnCollisionStay(GOHandle h)
{
}

void ObjectiveManager::OnCollisionExit(GOHandle h)
{
}


bool ObjectiveManager::SetAndCheckComplete(GOHandle obj, MissionData * a)
{
    auto infor = ASC_GETSCRIPT_INFOR_PTR(obj);

   // if (infor->misionlvl == -1) return;
    //std::cout << infor->missionNum;
    if (infor->misionlvl == -1)  return false ;
    LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);

    auto& missionData = levelData[lvl].find(infor->missionNum);
    if (missionData != levelData[lvl].end())
    {
          MissionData* group = & levelData[lvl][infor->missionNum];
         for (auto & [num ,gameobj ] : group->mapGameObj)
         {
          
          
      
           if (gameobj.gameobj == obj)
           {
             gameobj.currDone = true;
             group->numDone++;
             a = group;
           }

         }
         auto  pointer =  ASC_GETSCRIPT_INTERACT(obj);
         int amtDone = group->mapGameObj.size();
         //
         if (pointer != nullptr)
         {
           if (pointer->amtOfTimes != -1)
           {
             amtDone = pointer->amtOfTimes;
           }
         }
         if (amtDone == group->numDone)
         {
           group->done = true;

           return true;
         }
     
    }
    return false; 
}

bool ObjectiveManager::CheckIfAllMissionIsDone(GOHandle obj)
{
  auto infor = ASC_GETSCRIPT_INFOR_PTR(obj);
  LEVELS lvl = static_cast<LEVELS>(infor->misionlvl);
  for (auto const& [missionNum,mission] : levelData[lvl])
  {
    if (!mission.done)
    {
      return false;
    }
  }
  // all mission are clear 
  return true; 

}


bool ObjectiveManager::FadeBlack(float dt)
{

 
      //fading in
   //   if (startFade)
      {
        if (fadeTime > 0.0)
        {
          fadeTime -= dt;
          Fade->GetHdl<GraphicsComp>()->SetTransparencyVal((1 - (fadeTime / 2)));
        }

        else
        {
          if (ASC_ENGINE.CurrentScene() != finalLvlName)
          {      
            ASC_MSGSYS.SendMsg(Msg_Change_Scene(levelname[currLvl++]));
          }
      

          if (!playEndSound && ASC_ENGINE.CurrentScene() == finalLvlName)
          {
              ASC_SOUNDMAN.FadeTracks(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), ASC_SOUNDMAN.currentBGM, "BGM_Splash.ogg");
              playEndSound = true;
          }
          else
              ASC_SOUNDMAN.Stop(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>());
          
         // p->GetGOHandle()->GetHdl
          //ASC_PLAY("BGM_Splash.ogg");
          startCutScene = true;
          g->SetVisible(true);
          /*fadeout = true;
          fadeTime = 2.0;

          
          p->canMove = true;
          if (PositionOne)
            ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<TransformComp>()->pos = position2->GetHdl<TransformComp>()->pos;
          else
            ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<TransformComp>()->pos = position1->GetHdl<TransformComp>()->pos;
          PositionOne = !PositionOne;*/
          return true; 
        }
      }
      ////fading out
      //else
      //{
      //  if (fadeTime > 0.0)
      //  {
      //    fadeTime -= dt;
      //    Fade->GetHdl<GraphicsComp>()->SetTransparencyVal((fadeTime / 2));
      //  }
      //  
//else
      //  {
      //    fadeout = false;
      //    fadeTime = 0.0;
      //    moving = false;
      //    PlayerController* p = ASC_GETSCRIPT_PTR(PlayerController, ASC_FACTORY.GetObjectHandleList(go_player)[0]);
      //    p->canMove = true;
      //  }
      //}
    

      return false;
}
void ObjectiveManager::ChangeLevel(std::string)
{


 // ASC_MSGSYS.SendMsg(Msg_Change_Scene);

}
void ObjectiveManager::PlayFireBgm()
{
  ASC_SOUNDMAN.FadeTracks(ASC_FACTORY.GetObjectHandleList(go_player)[0]->GetHdl<SoundComp>(), ASC_SOUNDMAN.currentBGM,"BGM_FireTime.ogg");
 

 // ASC_SOUNDMAN.FadeTracks(p->GetGOHandle()->GetHdl<SoundComp>(), ASC_SOUNDMAN.currentBGM, "BGM_FireTime.ogg");
  //ASC_SOUNDMAN.Stop(s, ASC_SOUNDMAN.currentBGM);
  //ASC_SOUNDMAN.Play(player->GetHdl<SoundComp>(), "BGM_FireTime.ogg", 0);
 // ASC_SOUNDMAN.currentBGM = "BGM_FireTime.ogg";
  //BGM_FireTime.ogg
}
//