/******************************************************************************/
/*!
\file   Editor.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for Engine

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

//// Demonstrate the various window flags. Typically you would just use the default.
//ImGuiWindowFlags window_flags = 0;
//if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
//if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
//if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
//if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
//if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
//if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
//if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
//ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
//ImGuiWindowFlags_NoSavedSettings

//Modal windows are like popups but the user cannot close them by clicking outside the window
//ImGui::BeginPopupModal
//ImGui::EndPopup();

#define Imgui ImGui
#define BUTTON_WITH_SPACING(name) Imgui::Button("    "  name  "    ")

#ifdef _SUBMISSION
void Editor::Init(ObjectFactory& factory, Camera& camera, unsigned int numOfSystems) {}
void Editor::SetSysPerformaceTime(unsigned int systemNum, float val) {}
void Editor::SetLoadedScene(const std::string& scene) {}
void Editor::HelpMarker(std::string text, bool sameLine) {}
void Editor::ItemHoveredToolTip(std::string text) {}
void Editor::SetSelected(GameObject * gameObject, bool scroll) {}
#else
static void ShowHelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(450.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}
void Editor::EnableDocking()
{
  ImGuiIO& io = ImGui::GetIO();
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
 // ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }


}
void Editor::Init(ObjectFactory& factory, Camera& camera, unsigned int numOfSystems)
{
  m_factory = &factory;
  //m_cam = &camera;
  m_cam = &ASC_GMAN.GetCamera();

  WinOp.statWindow = false;

  winFlag =
	  ImGuiWindowFlags_AlwaysAutoResize |
	  ImGuiWindowFlags_NoTitleBar;

  //dont save ini file
  //Imgui::GetIO().IniFilename = nullptr;
  EnableDocking();
  m_pbstuff.UpdateList();
  m_gridassets.UpdateList();
  ZeroMemory(input, 30);

  InitSystemPerformance(numOfSystems);
  InitAddComponent();

  ASC_MSGSYS.RegisterCallBack(mTYPE_SYSTEM_SHORTCUT, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_SCENE_LOAD_RESULT, &Editor::HandleXMLLoad, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_SELECTED, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_DESELECTED, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PAUSE, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_OBJ_DESTROYED, &Editor::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_RESUME, &Editor::HandleMsg, this);
}

void Editor::SetSysPerformaceTime(unsigned int systemNum, float val)
{
  sysPerformVec[systemNum] = val;
}

void Editor::SetLoadedScene(const std::string& scene)
{
  currentScene = scene;
  SetLastOpenedFile(scene);
}

void Editor::HandleXMLLoad(MsgData_Base * msg)
{
  Msg_Error * m = reinterpret_cast<Msg_Error*>(msg);

  ImGui::TextColored(IM_GREEN, m->str.data());

#if 0
  START_MODAL_POPUP(".", 0)
  {
    ImGui::TextColored(IM_GREEN, m->str.data());
    if (ImGui::Button("Ok"))
    {
      ImGui::CloseCurrentPopup();
    }
  }
  END_MODAL_POPUP
#endif
}

void Editor::HandleMsg(MsgData_Base * msg)
{
  switch (msg->type)
  {
  case mTYPE_SYSTEM_SHORTCUT:
  {
    Msg_UINT * m = reinterpret_cast<Msg_UINT*>(msg);
    HandleShortcut(m->var);
  } break;

  case mTYPE_SELECTED:
  {
    Msg_UINT * m = reinterpret_cast<Msg_UINT*>(msg);
    SetSelected(&m_factory->GetGameObjectList()[m->var]);
  } break;

  /*case mTYPE_DESELECTED:
  {
    if (!ImGui::IsMouseHoveringAnyWindow())
      SetSelected(nullptr);
  } break;*/

  case mTYPE_RESUME:
  {
    playing = true;
    showUI = false;
    SetSelected();
    ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
  } break;

  case mTYPE_PLAY:
  {
    playing = true;
    showUI = false;
    SetSelected();
    CallFunctionsBeforeTmpFileSave();
    m_factory->SaveScene("EditorTemp.xml");
    CallFunctionsAfterTmpFileSave();
    //ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PLAY_SCRIPT_INIT));
  } break;

  case mTYPE_STOP:
  {
    playing = false;
    showUI = true;
    ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);
    m_cam = &ASC_GMAN.GetCamera();
    NewScene();
    m_factory->LoadScene(std::string(ASC_SCENES_FLD"EditorTemp.xml").c_str());
  } break;

  case mTYPE_PAUSE:
  {
    ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);
    m_cam = &ASC_GMAN.GetCamera(cam_handle);
  } break;

  case mTYPE_OBJ_DESTROYED:
  {
    GOHandle hdl = static_cast<Msg_Logic_OnCollide*>(msg)->gameObject;
    if (hdl.IsValid())
    {
      GameObject* obj = ASC_GETGO_PTR(hdl);
      m_selections.remove(obj);
    }
    m_selections.clear();
    SetSelected();
  } break;

  default:
    break;
  }
}

void Editor::HandleShortcut(unsigned int shortcut)
{
  switch (shortcut)
  {
  case ID_FILE_SAVE:
  {
    save = true;
    break;
  }
  case ID_FILE_OPEN:
  {
    WinOp.fileMenuWindow = true;
    break;
  }
  case ID_FILE_NEW:
  {
    //displayFileMenu = false;
    clearScene_menu = true;
    break;
  }

  case ID_EDIT_UNDO:
  {
    for (unsigned int i = 0; i < m_selections.size(); ++i)
      actions.UndoAction();

  } break;
  case ID_EDIT_REDO: actions.RedoAction(); break;

  default:
    break;
  }
}

void Editor::InitSystemPerformance(unsigned int num)
{
  sysPerformVec = std::vector<float>(num + 1);
  sysPerformVec_tmp = std::vector<float>(num + 1);
  duration_1sec.zero();
  sysPerformaceUpdateDelay_ms = 1000.0f;
}

void Editor::InitAddComponent()
{
  for (unsigned int i = 0; i < static_cast<unsigned int>(MAX); ++i)
  {
    addCompArr[i] = false;
  }
}

void Editor::Update(float dt)
{
  typedef std::chrono::high_resolution_clock Clock;
  //Clock::time_point start;
  //start = Clock::now();
  openPopupsCount = 0;

  if (!ImGui::IsAnyItemActive())
  {
    UpdateCamControls(dt);
    CheckInput();
  }

  // Start the frame
  ImGui::NewFrame();
  ////
  //ASC_TWEEN.Reflect();

#if _SUBMISSION
  if (showUI)
  {
    if (WinOp.statWindow)
      DrawStatistics(dt);
  }
#else

  if (ASC_KEYTRIGGERED(ASC_KEY_INSERT))
    WinOp.statWindow = !WinOp.statWindow;

  if (WinOp.statWindow)
  {
    DrawStatistics(dt);
  }
  if (WinOp.imageDisplay) 
	  ImageDisplay();
#endif

  {
    UpdatePause();
    if (showUI)
    {
      DrawInvisableOverLay();
      //m_cam = &ASC_GMAN.GetCamera(cam_handle);

      /*if (WinOp.statWindow)
        DrawStatistics(dt);*/
      DrawGameObjecList(dt);
      DrawProperties(dt);
      if (WinOp.prefabListWindow) DrawPrefabWin();
      if (WinOp.GridEditor) DrawGridEditor();
      if (WinOp.WayPointEditor) DrawWayPointEditor();
      if (WinOp.TestCameraShake) TestCamShake();
      if (WinOp.phyGlobalSettings) DrawGlobalPhySetting();
      ShiftObjectsEditor();

      FileMenu();
      //DrawAddComponents();
    }
  }
  ////
  ImGui::Render();
  ////

  //std::chrono::duration<float, std::milli> duration;
  //duration = Clock::now() - start;

  //durations += dt;
  //Log(duration_1sec.count(), "\n");
  if (duration_1sec.count() > sysPerformaceUpdateDelay_ms)
  {
    //updateNow = true;
    duration_1sec = std::chrono::milliseconds(0);
  }
  else
  {
    //duration_1sec += duration;
    duration_1sec += realtime;
    //updateNow = false;
  }
}

void Editor::CheckInput()
{
  //if (ASC_KEYTRIGGERED(ASC_KEY_F4))
  //  Create3PointLight();

  //if(ASC_KEYTRIGGERED(ASC_KEY_F1))
 /* if (ASC_KEYDOWN(ASC_KEY_F1))
  {
    SetSelected(&m_factory->AddObject());
    m_factory->AddComponent(*m_selected, GRAPHICS, SOUND, PHYSICS);
    TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
    t->pos.y = 5.0f;
    GraphicsComp * gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
    gc->modelfile = "Sphere";
    PhysicsComp * pc = m_factory->GetComponentPtr<PhysicsComp>(m_selected->GetSelfHandle());
    pc->AddShape(eSPHERE);
  }*/

  if (ASC_KEYTRIGGERED(ASC_KEY_F8))
  {
    int row = 10, col = 10;
    float Spacing = 1.0f, height = 1.0f;
    float hSize = (row - 1) * Spacing * 0.5f;

    Vec4 color(ASC_RAND_COL, ASC_RAND_COL, ASC_RAND_COL, 1.0f);
    for (int x = 0; x < row; ++x)
    {
      for (int z = 0; z < col; ++z)
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, GRAPHICS, PHYSICS);
        TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos = Vec3(x * Spacing, height, z * Spacing);
        t->pos -= Vec3(hSize, 0.0f, hSize);

        GraphicsComp * gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
        gc->modelfile = "Sphere";
        gc->tint = color;
        gc->material.Metalness = x * 1.0f / row;
        gc->material.Roughness = z * 1.0f / col * 1.5f;

        PhysicsComp * pc = m_factory->GetComponentPtr<PhysicsComp>(m_selected->GetSelfHandle());
        pc->AddShape(eSPHERE);
      }
    }
  }

  mousePos = ASC_INPUT.GetMousePos();

  //clone
  if (m_selected && ASC_KEYDOWN(ASC_KEY_LCTRL) && ASC_KEYTRIGGERED(ASC_KEY_D))
  {
    for (GameObject* go : m_selections)
    {
      HighlightObj(*go, false);
      go = m_factory->GetGameObjectPtr(m_factory->CloneGO(go->GetSelfHandle()));
    }

    for (GameObject* go : m_selections)
      HighlightObj(*go, true);

    //SetSelected(m_factory->GetGameObjectPtr(m_factory->CloneGO(m_selected->GetSelfHandle())));
  }

  if (ASC_KEYDOWN(ASC_KEY_LCTRL) && ASC_KEYTRIGGERED(ASC_KEY_G))
    WinOp.GridEditor = !WinOp.GridEditor;

  // Picking
  if (showUI && ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE) && !WinOp.WayPointEditor &&
    !ImGui::IsAnyWindowHovered() && !ImGuizmo::IsOver())
  {
    /*if (!ImGui::IsMouseHoveringAnyWindow())
      SetSelected(nullptr);
*/
    GOHandle hdl;
    unsigned i = ASC_GMAN.GetPickedObject();
    if (i < ASC_GMAN.m_components.m_objects.size())
    {
      hdl = ASC_GMAN.m_components.m_objects[i].GetGOHandle();
    }
    else
    {
      // Pick colliders
      Camera &cam = ASC_GMAN.GetCamera();
      Vec3 pos = cam.GetPos();
      Vec3 start, end;
      cam.GetRayFromMouse(start, end);
      Vec3 dir = (end - start).Normalize();
      float t = 0.0f;

      hdl = PhysicsManager::RayCast(pos, dir, t);
    }

    if (hdl.IsValid())
      SetSelected(m_factory->GetGameObjectPtr(hdl));
    else
      SetSelected();
  }

  if (m_selected && !ImGui::IsAnyItemActive())
  {
    //if (!ImGui::IsMouseHoveringAnyWindow())
    {
      if (ASC_KEYTRIGGERED(ASC_KEY_F))
      {
        // Focus camera
        Vec3 pos = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle())->pos;
        Camera &cam = ASC_GMAN.GetCamera();
        cam.SetPosition(pos + cam.GetForward() * -30.0f);
      }
      else if (ASC_KEYTRIGGERED(ASC_KEY_M))
      {
        auto trans = ASC_GETCOMP_PTR(TransformComp, m_selected->GetSelfHandle());
        trans->pos = m_cam->GetPos() + 30.0f * m_cam->GetForward();
      }
    }

    if (ASC_KEYTRIGGERED(ASC_KEY_DEL))
    {
      WPtHandle hdl = m_selected->GetHdl<WayPointNode>();
      if (hdl.IsValid()) {
        ASC_WP.DeleteWayPoint(m_selected->GetSelfHandle());
      }

      DeleteSelectedGO();
    }
  }

  if (ASC_KEYTRIGGERED((ASC_KEY_BACKQUOTE)) && playing)
  {
    static auto playFn = []()
    {
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_RESUME));
    };

    static auto pauseFn = []()
    {
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PAUSE));
    };

    static void(*sendMsg[])() { playFn, pauseFn };
    showUI = !showUI;
    sendMsg[showUI]();
  }

  if (m_selected && ASC_KEYDOWN(ASC_KEY_LALT) && ASC_KEYTRIGGERED(ASC_KEY_S))
  {
    m_factory->SavePrefab(m_selected);
    m_pbstuff.UpdateList();
  }

  //if waypoint editor mode is on Lctrl + Lclick is done
  if (WinOp.WayPointEditor)
  {

  }

  if (!m_selections.empty() && ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
  {
    for (auto& elem : m_selections)
      actions.StartAction(elem);
  }

  if ((ASC_KEYDOWN(ASC_KEY_LCTRL) || ASC_KEYDOWN(ASC_KEY_RCTRL)) &&
    (ASC_KEYDOWN(ASC_KEY_LSHIFT) || ASC_KEYDOWN(ASC_KEY_RSHIFT)) &&
    ASC_KEYTRIGGERED(ASC_KEY_P))
  {
    //Log("CreateD_Player");
    m_factory->CreateGOfromPB("Player");
  }
}

void Editor::DrawWayPointEditor()
{
  if (ImGui::IsAnyWindowHovered() || ImGuizmo::IsOver()) return;
  //ctrl + left click to link node...
  if (ASC_KEYDOWN(ASC_KEY_LCTRL) && ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
  {
    //if we have selected a waypoint node,
    if (m_selected && m_selected->GetType() == GOTYPE::go_waypoint)
    {
      GameObject* currSel = nullptr;
      unsigned i = ASC_GMAN.GetPickedObject();
      if (i < ASC_GMAN.m_components.m_objects.size())
      {
        GOHandle hdl = ASC_GMAN.m_components.m_objects[i].GetGOHandle();
        if (hdl.IsValid()) {
          currSel = m_factory->GetGameObjectPtr(hdl);
          if (currSel->GetType() != go_waypoint) return;
        }
      }

      //std::vector<GameObject> &objects = m_factory->GetObjectList();
      if (m_selected->GetType() == go_waypoint && currSel)
      {
        //retrieved the rhs obj as well.
        auto lhsHdl = m_selected->GetSelfHandle();
        auto rhsHdl = currSel->GetSelfHandle();
        if (lhsHdl.IsValid() && rhsHdl.IsValid())
        {
          //linking up the two nodes
          auto &lhsComp = (*ASC_GETCOMP_PTR(WayPointNode, lhsHdl));
          auto &rhsComp = (*ASC_GETCOMP_PTR(WayPointNode, rhsHdl));
          ASC_WP.CreateLink(lhsComp, rhsComp);
        }
      }
    }
  }
  //left click to create node.
  //will select a instead node if it exist.
  else if (ASC_KEYDOWN(ASC_KEY_LALT) && ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
  {
    //if selected type is not go_waypoint, we create one.
    Vec3 start, end, dir;
    m_cam->GetRayFromMouse(start, end);
    dir = (end - start).Normalize();
    float contactTime;
    ASC_PHYSICSMANAGER.RayCast(start, dir, contactTime);
    if (contactTime == FLT_MAX) return;

    Vec3 contactPt = start + (dir * contactTime);
    contactPt.y += 1.0f;

    GOHandle wp = ASC_WP.AddWayPoint(contactPt);
    wp->m_prefab_name = "";
    //set selected to prefab.
    SetSelected(ASC_GETGO_PTR(wp));
  }
  else if (ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
  {
    SetSelected(nullptr);

    //checks if raypicked obj is a waypoint. if it is, select it.
    unsigned i = ASC_GMAN.GetPickedObject();
    if (i < ASC_GMAN.m_components.m_objects.size())
    {
      GOHandle hdl = ASC_GMAN.m_components.m_objects[i].GetGOHandle();
      if (hdl.IsValid())
      {
        auto gameObject = m_factory->GetGameObjectPtr(hdl);
        if (gameObject && gameObject->GetType() == GOTYPE::go_waypoint)
        {
          SetSelected(gameObject);
        }
      }
    }
  }
}

void Editor::DrawStatistics(float dt)
{
  APPCONSOLE.Draw();

  Imgui::SetNextWindowPos({ 0.0f, ASC_SYSTEM.winHeight - 170.0f });
  ImGui::SetNextWindowSize({ static_cast<float>(ASC_SYSTEM.winWidth), 30 });
  ImGui::Begin("SysPerformance Window", &WinOp.statWindow,
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar
  );
  
  //ImGui::Checkbox("Update Graph", &animate);
  //ItemHoveredToolTip(animate == true ? g_arr[1] : g_arr[0]);
  //Imgui::SameLine();
  //Imgui::InputFloat("ms Delay", &sysPerformaceUpdateDelay_ms, 500.0f, 5000.0f, 0);

  //if (updateNow && animate)
  //{
  UpdateSysPerformance(sysPerformVec);
  sysPerformVec_tmp = sysPerformVec;
  //}
  //else
  //{
  //  UpdateSysPerformance(sysPerformVec_tmp);
  //}

  //static std::string msg;
  //if (playing)
  //{
  //  msg = "Updating in : " + std::to_string(static_cast<int>(1000.0f - duration_1sec.count()) / 100);
  //  Imgui::TextColored(IM_GREEN, msg.data());
  //}
  //else
  //{
  //  msg = "Game is Paused";
  //  Imgui::TextColored(IM_RED, msg.data());
  //}
  //HelpMarker(std::to_string(static_cast<int>(sysPerformaceUpdateDelay_ms) / 1000) + " sec delay in updating graph\n Empty if game is paused", true);

  ImGui::End();


  arrFPS[size] = 1.0f / dt;
  (size > FPS_ARR_SIZE - 1) ? (size = 0) : size++;

  /*if (animate)
  {
    arrFPS[size] = 1.0f / dt;
    (size > FPS_ARR_SIZE - 1) ? (size = 0) : size++;

    if (resetGraph && !size)
    {
      for (int i = 0; i < FPS_ARR_SIZE; ++i)
        arrFPS[i] = 0;
    }
  }*/

  Imgui::SetNextWindowPos({ 0.0f, ASC_SYSTEM.winHeight - 140.0f });
  ImGui::SetNextWindowSize({ (float)ASC_SYSTEM.winWidth, 40 });
  ImGui::Begin("FPS Window", &WinOp.statWindow,
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoTitleBar
  );

  //ItemHoveredToolTip("Avg FPS does not work with Reset graph per frame.");
  //ImGui::Checkbox("Update Graph", &animate);
  //ItemHoveredToolTip(animate == true ? g_arr[1] : g_arr[0]);
  //Imgui::SameLine(0.0f, 50.0f);
  //ImGui::Checkbox("Reset Graph per frame", &resetGraph);
  //ItemHoveredToolTip("Avg FPS does not work with Reset graph per frame.\n ");

  double avg = std::accumulate(arrFPS, arrFPS + FPS_ARR_SIZE, 0.0) / FPS_ARR_SIZE;
  std::string averageFPS = "avg fps : " + std::to_string(avg);

  ImGui::PlotLines("FPS", arrFPS, FPS_ARR_SIZE, 0, averageFPS.data(), 50.0f, 70.0f, { (float)ASC_SYSTEM.winWidth - 60, 20 }, 4);
  ImGui::End();
}

void Editor::DrawGameObjecList(float dt)
{
  static bool OpenPrefabPopUp = false;

 
  ImGui::Begin("Object", &WinOp.statWindow);

  if (ImGui::Button("Add GameObject"))
  {
    SetSelected(&m_factory->AddObject());
  }
  ImGui::SameLine();
  if (ImGui::Button("Save Prefab"))
  {
    //Should be able to save multiple prefabs here
    if (m_selected)
    {
      //will need to call the saving of object to file
      //and load all the filenames
      m_factory->SavePrefab(m_selected);
      m_pbstuff.UpdateList();
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("ReGen"))
  {
    for (auto &item : m_pbstuff.m_list)
    {
      m_factory->ReloadPB(item.c_str());
      auto goHdl = m_factory->CreateGOfromPB(item.c_str());
      m_factory->SavePrefab(m_factory->GetGameObjectPtr(goHdl));
      m_factory->DestroyObject(goHdl);
    }
    m_pbstuff.UpdateList();
  }

  std::vector<std::string> filterList;
  filterList.push_back("Camera");
  std::string headerName = "GameObjects ("
    + std::to_string(m_factory->GetGameObjectList().size())
    + ")";

  if (ImGui::CollapsingHeader(headerName.data(), ImGuiTreeNodeFlags_DefaultOpen))
  {
    static ImGuiTextFilter filter;
    if (ImGui::Button("x"))
      filter.Clear();
    Imgui::SameLine();
    filter.Draw("Search", 150.0f);
    Imgui::SameLine();
    ShowHelpMarker("Inside the scene");

    for (auto& elem : m_factory->GetGameObjectList())
      filterList.push_back(elem.name);

    ImGui::BeginChild("Items");
    float scrollPos = 0.0f;
    for (unsigned int i = 0; i < filterList.size(); ++i)
    {
      if (filter.PassFilter(filterList[i].data()))
      {
        if (i == 0) EditorCamControls();
        else
        {
          ImGui::PushID(i);

          GameObject* tmpObj = &m_factory->GetGameObjectList()[i - 1];
          if (m_scroll && tmpObj == m_selected)
          {
            m_scroll = false;
            ImGui::SetScrollY(scrollPos);
          }
          scrollPos += 20.0f;

          if (ImGui::Selectable(filterList[i].data(), SelectedObj == i) ||
            m_selected == tmpObj)
          {
            SelectedObj = i;

            //if (ImGui::IsItemClicked())
            SetSelected(tmpObj, false);

            //this is for adding of prefab from object list
          }
          if (ImGui::IsItemHovered() && ASC_KEYTRIGGERED(ASC_BUTTON_RIGHTMOUSE))
          {
            SetSelected(tmpObj);
            OpenPrefabPopUp = true;
          }

          ImGui::PopID();
        }
      }
    }
    ImGui::EndChild();
  }

  //this is for adding of prefab from object list
  constexpr const char* PopUpTitle = "Object removal";
  if (OpenPrefabPopUp)
  {
    ImGui::OpenPopup(PopUpTitle);
    OpenPrefabPopUp = false;
  }
  //this is for popup rendering for hb~
  if (ImGui::BeginPopup(PopUpTitle))
  {
    if (ImGui::MenuItem("Remove Object"))
    {
      DeleteSelectedGO();
    }

    std::string childname = "NIL";
    if (tmpchildhdl.IsValid())
    {
      childname = ASC_GETGO_PTR(tmpchildhdl)->name;
      if (ImGui::MenuItem("Select as Parent"))
      {
        //before attachment of child we set the local position using the world position
        TransformComp* childtrans = ASC_GETCOMP_PTR(TransformComp, tmpchildhdl);
        TransformComp* parentrans = ASC_GETCOMP_PTR(TransformComp, m_selected->GetSelfHandle());
        Matrix4 parentRotMat = QuaternionToMatrix(parentrans->rotation);
        Vector3 relativePos = childtrans->pos - parentrans->pos;
        Vector3 relativeScaleMul = (1.0f / parentrans->scale);
        //using the inverse matrix of the parent coordinate
        childtrans->localpos = (Matrix4::Scale(relativeScaleMul) * parentRotMat * Vec4(relativePos, 1.0f)).xyz;
        //childtrans->localpos = parentRotMat * relativeScaleMul * (relativePos);
        childtrans->localscale = relativeScaleMul * childtrans->scale;
        //using queternion
        childtrans->localrotation = parentrans->rotation.Inverse() * childtrans->rotation;

        childtrans->dirty = true;
        AttachChild(m_selected->GetSelfHandle(), tmpchildhdl);
        tmpchildhdl.AssignNewInValid();
      }
    }
    if (ImGui::MenuItem("Select as Child", childname.c_str()))
    {
      tmpchildhdl = m_selected->GetSelfHandle();
    }


    ImGui::EndPopup();
    //OpenPrefabPopUp = false;
  }

  ImGui::End();
}


void Editor::DrawProperties(float dt)
{
  
  std::vector<const char*> compName;
  if (m_selected)
  {
    for (const auto& elem : strCompMap)
    {
      if (m_selected->components.size() && !(*m_selected)[elem.second].IsValid())
        compName.push_back(elem.first.data());
    }
  }
  Imgui::Begin("GameObjects Properties", &WinOp.statWindow  );
  /*Imgui::SetWindowPos({ ASC_SYSTEM.winWidth - ImGui::GetWindowSize().x, 20.0f });*/

  static int compSelectedIdx = 0;
  if (compName.size() > 0)
  {
    Imgui::Combo("##addComp", &compSelectedIdx, compName.data(), static_cast<int>(compName.size()));
    Imgui::SameLine();

    //Imgui::PushID(12);
    if (ImGui::Button("Add Component"))
    {
      m_factory->AddComponent(*m_selected, strCompMap[compName[compSelectedIdx]]);
    }

    //Imgui::PopID();
    Imgui::Separator();
  }

  if (m_selected && m_selected->components.size())
  {
    Reflector r;
    m_factory->Reflect(*m_selected, r);

    if (ImGui::Button("Save Prefab"))
    {
      //will need to call the saving of object to file
      //and load all the filenames
      m_factory->SavePrefab(m_selected);
      m_pbstuff.UpdateList();
    }

    //calling the gizmo
    if (WinOp.gizmoInterface) DrawGizmo();
  }

  Imgui::End();
  compName.clear();
}

void Editor::DrawPrefabWin()
{
  static ImGuiTextFilter pbfilter;
  static int SelectedIndex = -1;
  static int openPopUp = false;


  ImGui::Begin("Prefabs", nullptr);
  if (ImGui::Button("x"))
    pbfilter.Clear();
  ImGui::SameLine();
  pbfilter.Draw("pb");
  ImGui::SameLine();
  //for information display
  ShowHelpMarker("Double Click on Selectables\n"
    "To Create.\n"
    "Right Click for other stuff");

  int counter = 0;
  //store the current prefab name to be deleted or created if selected
  static const char* selectedPB = nullptr;
  ImGui::BeginChild("items");
  for (const char* cstr : m_pbstuff.m_list_cstr)
  {
    if (pbfilter.PassFilter(cstr))
      if (ImGui::Selectable(cstr, counter == SelectedIndex))
      {
        selectedPB = cstr;
        SelectedIndex = counter;
      }

    if (SelectedIndex == counter && ImGui::IsItemHovered())
    {
      if (ImGui::IsMouseDoubleClicked(0))
      {
        //create item
        //m_factory->LoadPrefab(selectedPB);
        //auto selectedHandle = m_factory->LoadGOFromPBFile(selectedPB);
        m_factory->ReloadPB(selectedPB);
        auto selectedHandle = m_factory->CreateGOfromPB(selectedPB);
        SetSelected(m_factory->GetGameObjectPtr(selectedHandle));
      }
      //open up pop up
      else if (ImGui::IsMouseClicked(1))
      {
        openPopUp = true;
      }
    }

    ++counter;
  }
  ImGui::EndChild();
  if (openPopUp)
  {
    ImGui::OpenPopup("Prefab Options");
    openPopUp = false;
  }

  if (ImGui::BeginPopup("Prefab Options"))
  {
    if (ImGui::MenuItem("Delete"))//, NULL, false, false))
    {
      //delete prefab from folder
      DeleteFileInDir(ASC_PREFABS_FLD, std::string(selectedPB) + ASC_PREFAB_EXT_DOT);
      m_pbstuff.UpdateList();
    }
    ImGui::EndPopup();
  }

  ImGui::End();
}

void Editor::DrawGridEditor()
{
  
  static ImGuiTextFilter filter;
  static int SelectedIndex = -1;
  //store the current file name to be deleted or created if selected
  static const char* selectedGridFile = nullptr;

  static bool newgrid = false;
  static bool Saved = false;
  //static bool MenuEdit = false;
  static int gridmode = 0;
  static GridInfo gi;
  static GridHdl ghdl, pghdl, prevSelectedghdl;
  static char Namebuff[256] = "DefaultGrid";

  //shortcuts
  //if (ASC_KEYTRIGGERED(ASC_KEY_1))
  //{
  //  ghdl.AssignNewInValid();
  //  gridmode = 0;
  //}
  //if (ASC_KEYTRIGGERED(ASC_KEY_2))
  //{
  //  ghdl.AssignNewInValid();
  //  gridmode = 1;
  //}
  //if (ASC_KEYTRIGGERED(ASC_KEY_3))
  //{
  //  ghdl.AssignNewInValid();
  //  gridmode = 2;
  //}

  ImGui::SetNextWindowPos({ (float)ASC_SYSTEM.winWidth * 0.6f, 0.f });
  ImGui::SetWindowSize({ 0.f, 0.f });
  ImGui::Begin("GridEditor");

  //for the mode selection
  ImGui::RadioButton("Select mode", &gridmode, 0); ImGui::SameLine();
  //ItemHoveredToolTip("Press KEY 1");
  ImGui::RadioButton("NULL mode", &gridmode, 1); ImGui::SameLine();
  //ItemHoveredToolTip("Press KEY 2");
  ImGui::RadioButton("ENABLE mode", &gridmode, 2);
  //ItemHoveredToolTip("Press KEY 3");
  ImGui::Separator();

  if (ImGui::Button("New Grid")) newgrid = true;
  if (pghdl.IsValid())
  {
    ImGui::SameLine();
    ImGui::SetWindowSize({ 0.f, 0.f });
    if (ImGui::Button("Remove Grid"))
    {
      ASC_GRIDMAN.RemoveGrid(pghdl);
      pghdl.AssignNewInValid();
    }
    //other stuff like serialization 
    ImGui::SameLine();
    if (ImGui::Button("Serialize Grid"))
    {
      ASC_GRIDMAN.GetGrid(pghdl).Serialize();
      m_gridassets.UpdateList();
      selectedGridFile = nullptr;
      Saved = true;
    }
  }

  static const char* saveconfirm = "Grid Saved!";
  //feedback
  if (Saved)
    ImGui::OpenPopup(saveconfirm);

  if (ImGui::BeginPopupModal(saveconfirm))
  {
    ImGui::SetWindowSize({ 80.f, 50.f });
    if (ImGui::Button("OK"))
    {
      ImGui::CloseCurrentPopup();
      Saved = false;
    }
    ImGui::EndPopup();
  }

  if (newgrid)
  {
    ImGui::Separator();
    ImGui::SetWindowSize({ 0.f, 0.f });
    ImGui::BeginGroup();

    //set the setting
    ImGui::DragInt("Row", (int*)&gi.m_row, 1.0f, 1, 1000);
    ImGui::DragInt("Col", (int*)&gi.m_col, 1.0f, 1, 1000);
    ImGui::DragFloat("Cell width", &gi.m_spacing, 0.05f, 0.f, 50.f);
    ImGui::DragFloat2("Position", &gi.m_startpos.m[0], 1.0f);
    ImGui::DragFloat("Grid Height", &gi.m_height, 1.0f);
    ImGui::InputText("GridName", &Namebuff[0], 255);
    //set it to gi
    gi.m_name = Namebuff;

    if (ImGui::Button("Cancel")) newgrid = false;
    ImGui::SameLine();
    if (ImGui::Button("Create"))
    {
      ASC_GRIDMAN.createGrid(gi);
      newgrid = false;
    }
    ImGui::EndGroup();
  }
  else
    gi = GridInfo();

  int counter = 0;
  ImGui::Separator();
  //ImGui::SetNextWindowCollapsed(false);
  if (ImGui::CollapsingHeader("Loading file"))
  {
    if (selectedGridFile)
    {
      if (ImGui::Button("Load"))
      {
        //we can now call deserliaze of grid objects
        ASC_GRIDMAN.createGridFromFile(selectedGridFile);
      }
      ImGui::SameLine();
      ImGui::Text(selectedGridFile);
    }
    ImGui::SetWindowSize({ 0.f, 0.f });
    if (ImGui::Button("x"))
      filter.Clear();
    ImGui::SameLine();
    filter.Draw("files");
    for (const char* cstr : m_gridassets.m_list_cstr)
    {
      if (filter.PassFilter(cstr))
        if (ImGui::Selectable(cstr, counter == SelectedIndex))
        {
          selectedGridFile = cstr;
          SelectedIndex = counter;
        }
      ++counter;
    }
  }

  //we need to extract data from grid manager
  //over here is the actual implementation that the user will interact with the grid
  static bool CellChange = false;
  static int p_row = -1, p_col = -1;
  static int c_row = -1, c_col = -1;
  if (ASC_KEYDOWN(ASC_BUTTON_LEFTMOUSE) &&
    !Saved && !ImGuizmo::IsOver())
  {
    //reset the moment mouse is pressdown
    ghdl.AssignNewInValid();
    //for each grid check the if it collide
    for (Hexgrid& hg : ASC_GRIDMAN.GetAllGrid())
    {
      if (hg.GetChoosenCell(c_row, c_col))
      {
        ghdl = hg.GetSelfHandle();
        //we should check for previous ghdl but who cares
        CellChange = (p_row == c_row && p_col == c_col) ? false : true;
        //this is so user can click and change the individual cell again
        if (ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE)) CellChange = true;
        p_row = c_row;
        p_col = c_col;
        pghdl = ghdl;
        break;
      }
    }
  }

  //now for the editor implementation
  if (ghdl.IsValid())
  {
    Hexgrid& hg = ASC_GRIDMAN.GetGrid(ghdl);
    //for grid
    if (!gassets.Mapmover.IsValid())
    {
      GameObject& gridObj = m_factory->AddObject();
      gridObj.name = "GridObject";
      gassets.Mapmover = gridObj.GetSelfHandle();
      TransformComp* strans = ASC_TRANSMAN.GetComponent(gridObj[TRANSFORM]);
      //if surrogate moves ....grid moves as well
      strans->pos = hg.GetGridPosInWorld();
    }

    if (hg.boundaryCheck(c_row, c_col))
    {
      hg.RenderChoosenCell(c_row, c_col);
      //now we do somestuff
      if (CellChange) FlipFn[gridmode](hg, c_row, c_col);
    }

    //for assiging the gameobject position as the grid
    GameObject* surrogate = m_factory->GetGameObjectPtr(gassets.Mapmover);
    SetSelected(surrogate);
    TransformComp* strans = ASC_TRANSMAN.GetComponent((*surrogate)[TRANSFORM]);

    //check if is the same grid
    if (prevSelectedghdl != ghdl) strans->pos = hg.GetGridPosInWorld();
    //if surrogate moves ....grid moves as well
    hg.SetWorldPosToGrid(strans->pos);
    prevSelectedghdl = ghdl;
  }
  //else if (m_selected && m_selected->GetSelfHandle() == gassets.Mapmover)
  //{
  //  //c_row = -1; c_col = -1;
  //  //m_selected = nullptr;
  //}

  ImGui::End();
}


void Editor::UpdatePause()
{
  Imgui::SetNextWindowPos({ (ASC_SYSTEM.winWidth >> 1) - 90.0f, 20.0f });
  ImGui::SetNextWindowSize({ 100,35 });
  ImGui::Begin("Play/Pause", &WinOp.statWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, .0f, .0f ,1.0f});
  std::string state;

  if (showUI || !playing)
  {
    if (playing)
    {
      state = "Paused";
      Imgui::PushStyleColor(ImGuiCol_Button, IM_ORANGE);
    }
    else
    {
      state = "Stopped";
      Imgui::PushStyleColor(ImGuiCol_Button, IM_RED);
    }
  }
  else
  {
    state = "Playing";
    Imgui::PushStyleColor(ImGuiCol_Button, IM_GREEN);
  }

  //Imgui::PopStyleColor(stylecolor_push);

  //Imgui::PushStyleColor(ImGuiCol_Button, playing ? IM_GREEN : IM_RED);
  if (Imgui::Button((state + " (F5)").data(), { 90,20 }) || ASC_KEYTRIGGERED(ASC_KEY_F5))
  {
    playing = !playing;
    if (playing)
    {
      MsgData_Base play(mTYPE_PLAY);
      ASC_MSGSYS.SendMsg(play);
    }
    else
    {
      MsgData_Base stop(mTYPE_STOP);
      ASC_MSGSYS.SendMsg(stop);
    }

  }
 /* if (Imgui::Button((state + " (F6)").data(), { 90,20 }) || ASC_KEYTRIGGERED(ASC_KEY_F6))
  {
    playing = !playing;
    if (playing)
    {
      MsgData_Base play(mTYPE_PLAY);
      ASC_MSGSYS.SendMsg(play);
    }
    else
    {
      MsgData_Base stop(mTYPE_STOP);
      ASC_MSGSYS.SendMsg(stop);
    }

  }*/
  if (ASC_KEYTRIGGERED(ASC_KEY_F1))
  {
    showUI = !showUI;
  }
  Imgui::PopStyleColor(2);
  ItemHoveredToolTip("Game is " + state);
  Imgui::End();
}

void Editor::ImportLevel() const
{
  //NewScene();

  //xwstring xws;
  std::wstring xws;
  if (LoadFileDialog(xws, L"xml", L"Scenes"))
  {
    tinyxml2::XMLDocument xmlDoc;
    char textBuf[512];
    size_t tmp;
    wcstombs_s(&tmp, textBuf, xws.c_str(), 512);

    tinyxml2::XMLError result = xmlDoc.LoadFile(textBuf);
    if (result != tinyxml2::XML_SUCCESS)
    {
      LogError("Error loading XML");
    }

    auto elem = xmlDoc.FirstChildElement("instances");
    if (elem)
    {
      int numElems;
      elem->QueryAttribute("count", &numElems);
      auto *child = elem->FirstChildElement();

      while (child)
      {
        char buf[255];
        strcpy_s(buf, child->Attribute("name"));
        size_t len = strlen(buf);
        size_t end = len + 1;
        {
          const char *p = buf + len;
          do
          {
            if (*p >= 'A' && *p <= 'z')
            {
              break;
            }
            --p;
          } while (end--);
        }

        buf[end] = '\0';

        std::string str(buf);
        auto handle = m_factory->CreateGOfromPB(str);

        float fX, fY, fZ;

        auto *xStr = child->Attribute("posX");
        auto *yStr = child->Attribute("posY");
        auto *zStr = child->Attribute("posZ");
        sscanf_s(xStr, "%f", &fX);
        sscanf_s(yStr, "%f", &fY);
        sscanf_s(zStr, "%f", &fZ);
        /* std::istringstream(xStr) >> fX;
        std::istringstream(yStr) >> fY;
        std::istringstream(zStr) >> fZ;*/
        Vec3 pos(fX, fY, fZ);

        xStr = child->Attribute("rotX");
        yStr = child->Attribute("rotY");
        zStr = child->Attribute("rotZ");
        auto *wStr = child->Attribute("rotW");
        float fW;
        sscanf_s(xStr, "%f", &fX);
        sscanf_s(yStr, "%f", &fY);
        sscanf_s(zStr, "%f", &fZ);
        sscanf_s(wStr, "%f", &fW);

        /*std::istringstream(xStr) >> fX;
        std::istringstream(yStr) >> fY;
        std::istringstream(zStr) >> fZ;
        std::istringstream(wStr) >> fW;*/
        Quat rot(fW, fX, fY, fZ);
        //Quat rot = EulerToQuaternion(Vec3(fX, fY, fZ));
        //xradian3 rot(xradian(x_DegToRad(fX)), xradian(x_DegToRad(fY)), xradian(x_DegToRad(fZ)));

        xStr = child->Attribute("scaleX");
        yStr = child->Attribute("scaleY");
        zStr = child->Attribute("scaleZ");
        sscanf_s(xStr, "%f", &fX);
        sscanf_s(yStr, "%f", &fY);
        sscanf_s(zStr, "%f", &fZ);
        /*std::istringstream(xStr) >> fX;
        std::istringstream(yStr) >> fY;
        std::istringstream(zStr) >> fZ;*/
        Vec3 scale(fX, fY, fZ);
        TransformComp *transform = m_factory->GetComponentPtr<TransformComp>(handle);
        transform->pos = pos;
        transform->scale = scale;
        transform->rotation = rot;
        transform->dirty = true;

        child = child->NextSiblingElement();
      }

      for (int i = 0; i < numElems; ++i)
      {
        auto *p1 = elem->FirstChild();
        auto *p2 = elem->FirstChildElement();
        auto *a = p1;
      }
    }
  }
}

void Editor::CallFunctionsBeforeTmpFileSave()
{
}

void Editor::CallFunctionsAfterTmpFileSave()
{
  ASC_WP.SetAllToInvisible();
}

void Editor::FileMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("New Scene"))
      {
        clearScene_menu = true;
      }
      else if (ImGui::MenuItem("Load Scene", "Ctrl + O"))
      {
        WinOp.fileMenuWindow = true;
      }
      else if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
      {
        save = true;
      }
      else if (ImGui::MenuItem("Import From Unity", nullptr))
      {
        ImportLevel();
      }

      Imgui::EndMenu();
    }

    //different windows
    if (ImGui::BeginMenu("Windows"))
    {
      if (ImGui::BeginMenu("Gizmo"))
      {
        if (ImGui::MenuItem("GizmoInterface", nullptr, WinOp.gizmoInterface))
          WinOp.gizmoInterface = !WinOp.gizmoInterface;
        if (ImGui::MenuItem("GizmoSelectBar", nullptr, WinOp.gizmoSelection))
          WinOp.gizmoSelection = !WinOp.gizmoSelection;
        ImGui::EndMenu();
      }

      if (ImGui::MenuItem("Stats", nullptr, WinOp.statWindow))
        WinOp.statWindow = !WinOp.statWindow;
      if (ImGui::MenuItem("PrefabList", nullptr, WinOp.prefabListWindow))
        WinOp.prefabListWindow = !WinOp.prefabListWindow;
      if (ImGui::MenuItem("GridEditor", nullptr, WinOp.GridEditor))
        WinOp.GridEditor = !WinOp.GridEditor;
      ShowHelpMarker("LCTRL+LCLICK = link\nLALT+LCLICK = choose or create WP");
      ImGui::SameLine();
      if (ImGui::MenuItem("WayPointEditor", nullptr, WinOp.WayPointEditor))
      {
        WinOp.WayPointEditor = !WinOp.WayPointEditor;
        if (WinOp.WayPointEditor) ASC_GMAN.SetDebugMode();
      }
      if (ImGui::MenuItem("TestCamShake", nullptr, WinOp.TestCameraShake))
        WinOp.TestCameraShake = !WinOp.TestCameraShake;
	  if (ImGui::MenuItem("ImageDisplay", nullptr, WinOp.imageDisplay))
		  WinOp.imageDisplay = !WinOp.imageDisplay;

      Imgui::EndMenu();
    }

    //different creation
    if (ImGui::BeginMenu("Create"))
    {
      if (ImGui::MenuItem("newBaseObject", nullptr))
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, GRAPHICS,PHYSICS, NEWPHYSICS);
        TransformComp* t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos.y = 5.0f;
        GraphicsComp* gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
        gc->modelfile = "cube";
       /* PhysicsComp* pc = m_factory->GetComponentPtr<PhysicsComp>(m_selected->GetSelfHandle());
        pc->AddShape(eAABB);*/

      }
      else if (ImGui::MenuItem("Cube", nullptr))
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, GRAPHICS, PHYSICS);
        TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos.y = 5.0f;
        GraphicsComp * gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
        gc->modelfile = "Cube";
        PhysicsComp * pc = m_factory->GetComponentPtr<PhysicsComp>(m_selected->GetSelfHandle());
        pc->AddShape(eAABB);

      }
      else if (ImGui::MenuItem("Sphere", nullptr))
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, GRAPHICS, PHYSICS);
        TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos.y = 5.0f;
        GraphicsComp * gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
        gc->modelfile = "Sphere";
        PhysicsComp * pc = m_factory->GetComponentPtr<PhysicsComp>(m_selected->GetSelfHandle());
        pc->AddShape(eSPHERE);
      }
      else if (ImGui::MenuItem("Light", nullptr))
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, GRAPHICS, LIGHT);
        TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos.y = 5.0f;
        t->scale = Vec3(0.5f, 0.5f, 0.5f);
        GraphicsComp * gc = m_factory->GetComponentPtr<GraphicsComp>(m_selected->GetSelfHandle());
        gc->modelfile = "Sphere";
      }
      else if (ImGui::MenuItem("UIText", nullptr))
      {
        SetSelected(&m_factory->AddObject());
        m_factory->AddComponent(*m_selected, UITEXT);
        TransformComp * t = m_factory->GetComponentPtr<TransformComp>(m_selected->GetSelfHandle());
        t->pos.y = 5.0f;
      }
      Imgui::EndMenu();
    }

    ImGui::Text(("[Scene: " + currentScene + "]").c_str());

    ImGui::SameLine();
    std::string suffix = WinOp.WayPointEditor ? "[WayPoint Editing: on]" : "[WayPoint Editing: off]";
    ImGui::Text(suffix.c_str());

    Imgui::EndMainMenuBar();
  }

  if (WinOp.fileMenuWindow)
  {
    //default to clear scene first
    static bool clearScene = true;
    static std::string clearSceneBox = "Clearing Scene first";

    //load scene
    filenamesVec = GetFilesInDir(ASC_SCENES_FLD, "xml");
    //Editor::Filter(std::string("Scene Files"), WinOp.fileMenuWindow, filenamesVec, (int)filenamesVec.size());

    ImGui::OpenPopup("Scene Files");
    if (ImGui::BeginPopupModal("Scene Files", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ++openPopupsCount;

      static ImGuiTextFilter filter;
      if (ImGui::Button("x"))
        filter.Clear();
      Imgui::SameLine();
      filter.Draw("Search");
      int size = static_cast<int>(filenamesVec.size());
      for (int i = 0; i < size; i++)
      {
        if (filter.PassFilter(filenamesVec[i].data()))
        {
          //ImGui::BulletText("%s", container[i].data());
          ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Selected;
          if (ImGui::TreeNodeEx(filenamesVec[i].data(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen))
          {
            if (ImGui::IsItemClicked())
            {
              //clear scene before loading
              if (clearScene)
                NewScene();

              //load level/scene

           
              m_factory->LoadScene(std::string(ASC_SCENES_FLD + filenamesVec[i]).c_str());

              // Remove .xml ext
              filenamesVec[i].at(filenamesVec[i].size() - 4) = '\0';
              SetLoadedScene(filenamesVec[i]);
              WinOp.fileMenuWindow = false;

              break;
            }

            ItemHoveredToolTip("Click to Load");
          }
        }
      }

      if (ImGui::Button("Close"))
      {
        WinOp.fileMenuWindow = false;
      }

      Imgui::SameLine(70.0f);
      Imgui::Checkbox(clearSceneBox.data(), &clearScene);
      clearSceneBox = clearScene == true ? "Clearing Scene first" : "Loading as additional objects";
      ImGui::EndPopup();
    }
  }
  else if (save)
  {
    std::string winname("Save XML");

    ImGui::OpenPopup(winname.data());
    if (ImGui::BeginPopupModal(winname.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ++openPopupsCount;

      // Automatically save with the same file name
      //if (!strlen(input))
      {
        //auto first = currentScene.find_last_of('/');
        //auto last = currentScene.find_last_of('.');

        //std::string extracted(currentScene.begin() + first + 1, currentScene.begin() + last);
        //strcpy_s(input, extracted.c_str());
        strcpy_s(input, currentScene.c_str());
      }

      if (Imgui::InputText("Filename", input, 30))
      {
        currentScene = input;
      }

      ItemHoveredToolTip(std::string("Type without .xml extension!"));

      static bool clear = false;
      ClearFieldToggle(clear, input, 30, false);

      static bool duplicateName = false;
      if (duplicateName)
      {
        ++openPopupsCount;

        START_MODAL_POPUP("OverWrite File?", ImGuiWindowFlags_AlwaysAutoResize)
        {
          Imgui::TextColored(IM_RED, "File with same name already exist! Overwrite existing file?");
          if (BUTTON_WITH_SPACING("Yes"))
          {
            duplicateName = save = false;
            --openPopupsCount;

            SaveScene(input);
          }
          Imgui::SameLine();
          if (BUTTON_WITH_SPACING("No"))
          {
            duplicateName = false;
            Imgui::CloseCurrentPopup();
          }
          CheckEscForPopup();
        }
        END_MODAL_POPUP

      }

      if (BUTTON_WITH_SPACING("Save Now"))
      {
        //check if overwriting
        filenamesVec = GetFilesInDir(ASC_SCENES_FLD, "xml");
        for (auto elem : filenamesVec)
        {
          if (compare_str(std::string(input) + ".xml", elem))
          {
            duplicateName = true;
            break;
          }
        }

        if (!duplicateName)
        {
          SaveScene(input);
          save = false;
        }
      }//save now


      Imgui::SameLine();

      if (BUTTON_WITH_SPACING("Close"))
      {
        save = false;
        --openPopupsCount;
      }

      //check for esc key to close this popup
      CheckEscForPopup(save, false);

      Imgui::EndPopup();
    }
  }
  else if (clearScene_menu)
  {
    std::string name("Are you sure?");
    ImGui::OpenPopup(name.data());
    ++openPopupsCount;
    if (ImGui::BeginPopupModal(name.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
      Imgui::Text("You will lose all unsaved data. Proceed?");
      if (BUTTON_WITH_SPACING("Yes"))
      {
        NewScene();
        ClosePopup(clearScene_menu, false);
        Create3PointLight();
      }
      Imgui::SameLine();
      if (BUTTON_WITH_SPACING("No"))
      {
        ClosePopup(clearScene_menu, false);
      }

      //check for esc key to close this popup
      CheckEscForPopup(clearScene_menu, false);

      Imgui::EndPopup();
    }
  }
}

void Editor::DrawGizmo()
{
  if (!ASC_KEYDOWN(ASC_BUTTON_RIGHTMOUSE) && ASC_KEYDOWN(ASC_KEY_Q) && !ImGui::IsAnyItemActive())
  {
    SetSelected();
    return;
  }

  ImGuizmo::BeginFrame();
  static int currentItem = 1;

  static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
  static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
  //static ImGuizmo::

  if (WinOp.gizmoSelection && ImGui::Begin("Gizmo", &WinOp.gizmoSelection,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoMove))
  {
    ImGui::SetWindowPos({ (ASC_SYSTEM.winWidth >> 1) + 15.f, 21.0f });
    //if (ImGui::RadioButton("Select", &currentItem, 0))
      //hideGizmo = true;
    //ImGui::SameLine();
    if (ImGui::RadioButton("Translate", &currentItem, 1))
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", &currentItem, 2))
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", &currentItem, 3))
      mCurrentGizmoOperation = ImGuizmo::SCALE;
    ImGui::End();
  }

  if (!ASC_KEYDOWN(ASC_BUTTON_RIGHTMOUSE) && !ImGui::IsAnyItemActive())
  {
    if (ASC_KEYDOWN(ASC_KEY_W))
    {
      currentItem = 1;
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    else if (ASC_KEYDOWN(ASC_KEY_E))
    {
      currentItem = 2;
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    else if (ASC_KEYDOWN(ASC_KEY_R))
    {
      currentItem = 3;
      mCurrentGizmoOperation = ImGuizmo::SCALE;
    }
  }

  Mat4 &camView = m_cam->GetV();
  Mat4 &camProjection = m_cam->GetP();
  Mat4 mat;

  Vec3 snap;
  Vec3 bSnap(1.0f, 1.0f, 1.0f);
  if (ASC_KEYDOWN(ASC_KEY_LSHIFT)) snap.Set(0.25f, 0.25f, 0.25f);

  if (m_selections.size() == 1)
  {
    GameObject& sel = *m_selections.front();
    TransformComp* tc = m_factory->GetComponentPtr<TransformComp>(sel.GetSelfHandle());
    Vector3 rot = QuaternionToEuler(tc->rotation);
    ImGuizmo::RecomposeMatrixFromComponents(tc->pos.m, rot.m, tc->scale.m, mat.mat);
    Manipulate(camView.mat, camProjection.mat, mCurrentGizmoOperation, mCurrentGizmoMode, mat.mat, nullptr, snap.m);
    ImGuizmo::DecomposeMatrixToComponents(mat.mat, tc->pos.m, rot.m, tc->scale.m);
    tc->rotation = EulerToQuaternion(rot);
    tc->dirty = true;
  }
  else if (!m_selections.empty())
  {
    Quaternion quart;
    Vec3 startPos, endPos, endScale;

    Mat4 mat;

    for (auto& elem : m_selections)
    {
      TransformComp* tComp = m_factory->GetComponentPtr<TransformComp>(elem->GetSelfHandle());
      startPos += tComp->pos;
    }
    startPos /= static_cast<float>(m_selections.size());

    /*TransformComp* tc = m_factory->GetComponent<TransformComp>(sel.GetSelfHandle());*/
    Vector3 rot = QuaternionToEuler(quart);
    ImGuizmo::RecomposeMatrixFromComponents(startPos.m, rot.m, startScale.m, mat.mat);
    Manipulate(camView.mat, camProjection.mat, mCurrentGizmoOperation, mCurrentGizmoMode, mat.mat, nullptr, snap.m);
    ImGuizmo::DecomposeMatrixToComponents(mat.mat, endPos.m, rot.m, endScale.m);
    quart = EulerToQuaternion(rot);

    Vec3 posDiff = (endPos - startPos);
    Vec3 scaleDiff = (endScale - startScale);
    if (scaleDiff.MagnitudeSq() > FLT_EPSILON)
    {
      posDiff = posDiff * endScale;
      startScale = endScale;
    }

    for (auto& elem : m_selections)
    {
      TransformComp* tComp = m_factory->GetComponentPtr<TransformComp>(elem->GetSelfHandle());
      tComp->pos += posDiff;
      tComp->scale += scaleDiff;
      tComp->dirty = true;
    }
  }
}

void Editor::DrawInvisableOverLay()
{
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::SetNextWindowBgAlpha(0.0f);

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", nullptr, window_flags);
  ImGui::PopStyleVar(3);

  ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  ImGui::End();
}

void Editor::DrawAddComponents()
{
  const unsigned sound_idx = static_cast<unsigned int>(COMPONENT_TYPE::SOUND);
  const unsigned logic_idx = static_cast<unsigned int>(COMPONENT_TYPE::LOGIC);

  Imgui::SetNextWindowPos({ ASC_SYSTEM.winWidth / 2.0f - 300.f, 1.0f });
  ImGui::Begin("Add Component Window", &WinOp.statWindow,
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoTitleBar
  );
  ItemHoveredToolTip("Click to add/drop");

  Imgui::PushStyleColor(ImGuiCol_CheckMark, IM_GREEN);
  if (Imgui::RadioButton("+ Sound Comp", addCompArr[sound_idx]))
  {
    addCompArr[sound_idx] = !addCompArr[sound_idx];
    AddComponent(COMPONENT_TYPE::SOUND);

  }


  Imgui::SameLine();
  Imgui::PushStyleColor(ImGuiCol_CheckMark, IM_GREEN);
  ItemHoveredToolTip("Click to add/drop");
  if (Imgui::RadioButton("+ Logic Comp", addCompArr[logic_idx]))
  {
    addCompArr[logic_idx] = !addCompArr[logic_idx];
    AddComponent(COMPONENT_TYPE::LOGIC);
  }

  Imgui::PopStyleColor(2);
  ImGui::End();
}
void Editor::DrawGlobalPhySetting()
{
  auto & engineObj = ASC_NEWPHYSICSMANAGER;
  auto gravValue = engineObj.Phy_GetWorldGravity();
  
  /*if (Imgui::Begin("PhySetting"))
  {

    if (ImGui::DragFloat("SetWorldGrav", &gravValue, 1.0f, -1000.0f))
    {
      engineObj.Phy_SetWorldGravity(gravValue);
    }

    ImGui::End();
  }*/
}

void Editor::ShiftObjectsEditor() const
{
#if 0
  //ImGui::SetNextWindowPos(ImVec2(80.0f, 80.0f));
  ImGui::Begin("Shift Objects");

  static Vec3 v;
  ImGui::DragFloat3("Pos", v.m);
  //ImGui::SameLine();
  if (ImGui::Button("Shift"))
  {
    for (auto &elem : m_factory->GetGameObjectList())
    {
      auto trans = elem.GetHdl<TransformComp>();
      trans->pos -= v;
      trans->dirty = true;
    }
  }
  ImGui::End();
#endif
}

void Editor::Create3PointLight()
{
  ASC_LIGHTS.AddSkyLights({ 20,30, 20,0 }, { 1,1,1,1 }, 0.5f);
  ASC_LIGHTS.AddSkyLights({ -25,20,-5,0 }, { 1,1,1,1 }, 0.2f);
  ASC_LIGHTS.AddSkyLights({ -5,20,-25,0 }, { 1,1,1,1 }, 0.2f);
}

void Editor::ImageDisplay()
{
	ImGui::Begin("Image");
	for(int i =0; i < 10; ++i)
		ImGui::Image((ImTextureID)i, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::Separator();
	ImGui::End();
}

void Editor::HelpMarker(std::string text, bool sameLine)
{
  if (sameLine)
    Imgui::SameLine();
  ShowHelpMarker(text.data());
}


void Editor::SaveScene(std::string name)
{
  std::string filename(name);
  filename += ".xml";
  //save scene
  ASC_FACTORY.SaveScene(filename.data());
  SetLastOpenedFile(name);
}

void Editor::CheckEscForPopup(bool& var, bool set)
{
  if (ASC_KEYDOWN(ASC_KEY_ESCAPE))
  {
    ClosePopup(var, false);
  }
}

void Editor::CheckEscForPopup()
{
  if (ASC_KEYDOWN(ASC_KEY_ESCAPE))
  {
    Imgui::CloseCurrentPopup();
  }
}

void Editor::ClosePopup(bool& var, bool set)
{
  var = set;
  Imgui::CloseCurrentPopup();
  --openPopupsCount;
}

void Editor::ClearFieldToggle(bool & toggle, char * input, int size, bool sameLine)
{
  if (sameLine)
    Imgui::SameLine();

  Imgui::Checkbox("Clear Field", &toggle);
  if (toggle)
    ZeroMemory(input, size);

  toggle = false;
}

void Editor::ItemHoveredToolTip(std::string text)
{
  if (Imgui::IsItemHovered())
  {
    Imgui::BeginTooltip();
    ImGui::Text(text.data());
    Imgui::EndTooltip();
  }
}

void Editor::NewScene()
{
  m_factory->UnloadScene();
  m_selections.clear();
  SetSelected();
  GameObject::count = 1;
}

void Editor::UpdateSysPerformance(std::vector<float>& vec) const
{
  //float total = 0.0f;
  //total = std::accumulate(vec.begin(), vec.end(), total);
  for (unsigned int i = 0; i < COMPONENT_TYPE::MAX + auxSysNum; ++i)
  {
    std::string txt;
    if (i <= COMPONENT_TYPE::MAX - 1)
      txt = compStrMap[static_cast<COMPONENT_TYPE>(i)];
    else
      txt = auxSysName[i - COMPONENT_TYPE::MAX];
    txt += '\t';
    Imgui::ProgressBar(vec[i] / 16.0f, ImVec2(50.f, 0.f));
    Imgui::SameLine();
    ImGui::Text(txt.data());
    ImGui::SameLine();
  }
}

void Editor::AddComponent(COMPONENT_TYPE type) const
{
  if (m_selected)
  {
    LogError("Unable to Add Component, m_selected == nullptr");
    return;
  }

  m_factory->AddComponent(*m_selected, type);
}

void Editor::DeleteSelectedGO()
{
  for (auto& sel : m_selections)
    m_factory->DestroyObject(sel->GetSelfHandle());

  if (!m_selections.empty())
    SetSelected(); //reset selected
}

void Editor::SetSelected(GameObject* gameObject, bool scroll)
{

  for (auto& sel : m_selections)
    if (sel == gameObject) return;

  if (gameObject)
  {
    //for (GameObject* go : m_selections) HighlightObj(*go, true);
    if (!ASC_KEYDOWN(ASC_KEY_LSHIFT))
    {
      for (GameObject* go : m_selections) HighlightObj(*go, false);
      m_selections.clear();
    }

    m_selections.push_front(gameObject);

    HighlightObj(*gameObject, true);
  }
  else
  {
    for (GameObject* go : m_selections) HighlightObj(*go, false);
  }
  //Log((int)m_selections.size());
  m_selected = gameObject;
  startScale.Set(1.0f, 1.0f, 1.0f);
  ImGuizmo::Enable(gameObject != nullptr);
  SelectedObj = gameObject ? SelectedObj : 0;


  m_scroll = scroll;
}


void Editor::EditorCamControls()
{
  if (Imgui::Button("+"))
  {
    ASC_GMAN.AddCamera();
  }
  Imgui::SameLine();
  Imgui::Text("Camera");
  Imgui::SameLine();
  if (Imgui::Button("-"))
  {
    if (ASC_GMAN.GetTotalCamera() > 1)
      ASC_GMAN.RemoveCamera(ASC_GMAN.GetCameraIdx());
  }

  std::string name = "Camera #"
    + std::to_string(ASC_GMAN.GetCameraIdx())
    + "/"
    + std::to_string(ASC_GMAN.GetTotalCamera() - 1);

  if (ImGui::TreeNode(name.data()))
  {
    bool modified = false;
    bool projection = false;
    Reflector r;
    ImGui::DragFloat("FlySpeed", &flySpeed, 1.0f, 0.0f, 100.0f, "%.1f", 1.0f);
    ImGui::DragFloat("Exposure", &m_cam->GetExposure(), 0.1f, 0.1f, 10.0f);
    modified |= r.Reflect("Position", m_cam->GetPos());
    modified |= r.Reflect("Forward", m_cam->GetForward());
    modified |= r.Reflect("Right", m_cam->GetRight());

    projection |= r.Reflect("Fov", m_cam->GetFOV(), 5, 180, 1);
    projection |= r.Reflect("NearPlane", m_cam->GetNearPlane(), 0.01f, m_cam->GetFarPlane());
    projection |= r.Reflect("FarPlane", m_cam->GetFarPlane(), m_cam->GetNearPlane(), INFINITY);
    ImGui::TreePop();

    if (modified || projection)
      m_cam->SetDirty(modified, projection);
  }
}

void Editor::TestCamShake()
{
  ImGui::Begin("Camshake");

  static float t = 1.0f, I = 0.5f, deFac = 1.0f;
  static float xfac = 1.0f, yfac = 1.0f;
  ImGui::DragFloat("Duration", &t, 0.1f);
  ImGui::DragFloat("Intensity", &I, 0.01f);
  ImGui::DragFloat("Factor", &deFac, 0.001f);
  ImGui::DragFloat("xMultiply", &xfac, 0.01f);
  ImGui::DragFloat("yMultiply", &yfac, 0.01f);
  if (ImGui::Button("Shake!!"))
  {
    m_cam->SetdefaultCamShake(t, I, deFac, xfac, yfac);
    m_cam->StartCamShake();
  }

  ImGui::End();
}

void Editor::UpdateCamControls(float dt)
{
  if (showUI)
  {
    Vec3 lookDir = m_cam->GetForward();
    Vec3 strafe = m_cam->GetRight();
	Vec3 Altitude = m_cam->GetUp();
    Vec3 movDir;

    bool isRightMouseDown = ASC_KEYDOWN(ASC_BUTTON_RIGHTMOUSE) && !ImGui::IsAnyWindowHovered();
    ASC_INPUT.DisplayCursor(!isRightMouseDown);
    static float modifier = 1.0f;
    if (isRightMouseDown)
    {
      if (ASC_KEYDOWN(ASC_KEY_W)) movDir += lookDir;
      if (ASC_KEYDOWN(ASC_KEY_S)) movDir -= lookDir;
      if (ASC_KEYDOWN(ASC_KEY_A)) movDir -= strafe ;
      if (ASC_KEYDOWN(ASC_KEY_D)) movDir += strafe ;

      if (ASC_KEYDOWN(ASC_KEY_E))
        m_cam->GetPos().y += flySpeed * dt;

      if (ASC_KEYDOWN(ASC_KEY_Q))
        m_cam->GetPos().y -= flySpeed * dt;

      if (movDir.MagnitudeSq() != 0.0f)
      {
        Vec3 normDir = movDir.Normalize() * flySpeed * dt * modifier;
        m_cam->SetPosition(m_cam->GetPos() + normDir);

        modifier += 0.01f;
        modifier = min(modifier, 20.f);
      }
      else
        modifier = 1.0f;

      Vec2 currPos = ASC_INPUT.GetMousePos();
      Vec2 mDelta = (mousePos - currPos) * 10.0f * dt;
      m_cam->RotateForward(mDelta.y, mDelta.x);

      int x, y;
	  if (ASC_SYSTEM.isInFocus)
	  {
		  ASC_SYSTEM.GetWindowPos(x, y);
		  if (ASC_SYSTEM.winHeight != 0)
		  {
			  x += ASC_SYSTEM.winWidth / 2;
			  y += ASC_SYSTEM.winHeight / 2;
			  SetCursorPos(static_cast<int>(x), static_cast<int>(y));
		  }
	  }

    }
    else
    {
      //if (ASC_KEYDOWN(ASC_KEY_W)) 
      //if (ASC_KEYDOWN(ASC_KEY_S)) 
      if (ASC_KEYTRIGGERED(ASC_KEY_LEFT)) m_cam = &ASC_GMAN.PrevCamera();
      if (ASC_KEYTRIGGERED(ASC_KEY_RIGHT)) m_cam = &ASC_GMAN.NextCamera();
    }
	bool isMMDown = ASC_KEYDOWN(ASC_BUTTON_MIDDLEMOUSE) && !ImGui::IsAnyWindowHovered();
 	if (isMMDown)
  {
		ASC_INPUT.DisplayCursor(!isMMDown);
		Vec2 currPos = ASC_INPUT.GetMousePos();
		Vec2 mDelta = (mousePos - currPos) * 10.0f * dt;
		
		movDir += strafe * mDelta.x;
		movDir += Altitude * mDelta.y;
		
		// movDir += ((strafe + Altitude) * mDelta.Magnitude());
		if (movDir.MagnitudeSq() != 0.0f)
		{
			Vec3 normDir = movDir.Normalize() * flySpeed * dt * modifier;
			m_cam->SetPosition(m_cam->GetPos() + normDir);

			modifier += 0.01f;
			modifier = min(modifier, 20.f);
		}
		else
			modifier = 1.0f;
		if (ASC_SYSTEM.isInFocus)
		{
			int x, y;
			ASC_SYSTEM.GetWindowPos(x, y);
			if (ASC_SYSTEM.winHeight != 0)
			{
				x += ASC_SYSTEM.winWidth / 2;
				y += ASC_SYSTEM.winHeight / 2;
				SetCursorPos(static_cast<int>(x), static_cast<int>(y));
			}
		}
  }
	int isScroll = ASC_INPUT.GetMouseWheel() != 0 ? ASC_INPUT.GetMouseWheel() : 0;

	if (isScroll && !ImGui::IsAnyWindowHovered())
	{
		movDir += isScroll < 0 ? -lookDir : lookDir;
		if (movDir.MagnitudeSq() != 0.0f)
		{
			Vec3 normDir = movDir.Normalize() * flySpeed * dt * modifier;
			m_cam->SetPosition(m_cam->GetPos() + normDir);

			modifier += 0.01f;
			modifier = min(modifier, 20.f);
		}
		else
			modifier = 1.0f;
	}
  }

  
}


//function for updating list
void AssetList::UpdateList()
{
  m_list = GetFilesInDir(m_assetsDir, m_assetsExt);
  m_list_cstr.clear();
  m_list_cstr.reserve(m_list.size());
  for (std::string& str : m_list)
  {
    //take away the extension
    str.resize(str.size() - 4);
    m_list_cstr.push_back(str.c_str());
  }

}
#endif
