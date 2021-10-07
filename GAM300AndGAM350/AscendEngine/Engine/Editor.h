/******************************************************************************/
/*!
\file   Editor.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for Editor

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#define FPS_ARR_SIZE 100

#define IM_GREEN ImVec4{0.2f, .8f, 0.0f,1.0f}
#define IM_RED ImVec4{.8f, 0.2f, 0.0f,1.0f}
#define IM_ORANGE ImVec4{1.f, 0.65f, 0.0f,1.0f}
#define START_MODAL_POPUP(label, flags) ImGui::OpenPopup(label); \
if (ImGui::BeginPopupModal(label, nullptr, flags)) \
{

#define END_MODAL_POPUP ImGui::EndPopup(); }

struct AssetList
{
  std::string m_assetsDir;
  std::string m_assetsExt;
  AssetList(const std::string& dir, const std::string& ext)
    : m_assetsDir(dir), m_assetsExt(ext) {}

  void UpdateList();
  std::vector<std::string> m_list;
  std::vector<const char*> m_list_cstr;
};


//Most probably u need access to everything
#define ASC_EDITOR Editor::Inst()
class Editor : public Singleton<Editor>
{
#ifdef _SUBMISSION
public:
  void Init(ObjectFactory &factory, Camera& camera, unsigned int numofSystems);
  void SetLoadedScene(const std::string &scene);
  void SetSysPerformaceTime(unsigned int systemNum, float val);
  template<typename CONTAINER>
  void Filter(std::string& label, bool& open, CONTAINER& container, int size);
  static void ItemHoveredToolTip(std::string text);
  static void HelpMarker(std::string text, bool sameLine = false);
  void SetSelected(GameObject * gameObject = nullptr, bool scroll = true);



#else
  template<typename CONTAINER>
  void Filter(std::string& label, bool& open, CONTAINER& container, int size);

public:
  Editor() : /*updateNow(false),*/ durations(0), winFlag(0), m_cam(nullptr),
    m_pbstuff(ASC_PREFABS_FLD, ASC_PREFAB_EXT), startScale(1.0f, 1.0f, 1.0f),
    m_gridassets(ASC_GRID_FLD, "xml"), cam_handle(0),
    showUI(true)
  { }

  void Update(float dt);
  void Init(ObjectFactory &factory, Camera& camera, unsigned int numofSystems);
  void SetSysPerformaceTime(unsigned int systemNum, float val);
  void SetLoadedScene(const std::string &scene);
  void SetSelected(GameObject * gameObject = nullptr, bool scroll = true);

  static void ClearFieldToggle(bool& toggle, char* input, int size, bool sameLine);
  static void ItemHoveredToolTip(std::string text);
  static void HelpMarker(std::string text, bool sameLine = false);

  int openPopupsCount = 0;
  bool playing = false; //paused/play

  //GameObject *m_selected = nullptr;
  std::list<GameObject*> m_selections;
  GameObject* m_selected;

  bool showUI;

private:
  void EnableDocking();
  void HandleXMLLoad(MsgData_Base*);
  void HandleMsg(MsgData_Base *);
  void HandleShortcut(unsigned int shortcut);
  void InitSystemPerformance(unsigned int num);
  void InitAddComponent();

  //**** Main functions in Update() ****//
  void CheckInput();
  void DrawWayPointEditor();
  void DrawStatistics(float dt);
  void DrawGameObjecList(float dt);
  void DrawProperties(float dt);
  void DrawPrefabWin();
  void DrawGridEditor();
  void DrawGlobalPhySetting();
  void UpdatePause();
  void FileMenu();
  void DrawGizmo();
  void DrawAddComponents();
  void ShiftObjectsEditor() const;
  void Create3PointLight();
  void ImageDisplay();
  void DrawInvisableOverLay();
  
  //***********************************//

  //Helper functions and variables below
  static void SaveScene(std::string name);
  void CheckEscForPopup(bool& var, bool set);
  static void CheckEscForPopup();
  void ClosePopup(bool& var, bool set);
  void NewScene();
  void UpdateSysPerformance(std::vector<float>& vec) const;
  void AddComponent(COMPONENT_TYPE type) const;
  void DeleteSelectedGO();
  void ImportLevel() const;
  void CallFunctionsBeforeTmpFileSave();
  void CallFunctionsAfterTmpFileSave();

  bool addCompArr[COMPONENT_TYPE::MAX];

  std::vector<float> sysPerformVec_tmp;
  float sysPerformaceUpdateDelay_ms = 1000.0f;

  std::vector<std::string> filenamesVec;
  char input[30];

  //bool animate = true;
  //bool resetGraph = false;
  int cam_handle;
  float arrFPS[FPS_ARR_SIZE];
  int size = 0;
  std::vector<float>sysPerformVec;
  //bool updateNow;
  std::chrono::duration<float, std::milli> duration_1sec;
  float durations;

  unsigned int SelectedObj = 0;

  bool save = false;    //filemenu
  bool displayFileMenu = false;
  bool clearScene_menu = false;

  struct WindowsOption
  {
    bool gizmoInterface = true;
    bool gizmoSelection = true;
    bool statWindow = true;
    bool fileMenuWindow = false;
    bool prefabListWindow = true;
    bool GridEditor = false;
    bool WayPointEditor = false;
    bool TestCameraShake = false;
	  bool imageDisplay = false;
    bool phyGlobalSettings = true;
  } WinOp;

  //this is for constructing the list of filename in the respective folder
  AssetList m_pbstuff;
  AssetList m_gridassets;

  struct GridAssets
  {
    std::vector<std::string> m_gridlist;
    std::vector<const char*> m_gridlist_cstr;
    GOHandle Mapmover;
  } gassets;

  //parenting stuff
  GOHandle tmpchildhdl;

  void UpdateCamControls(float dt);
  void EditorCamControls();
  void TestCamShake();

  ImGuiWindowFlags winFlag;
  ObjectFactory *m_factory = nullptr;

  Camera* m_cam;
  Vec2 mousePos;
  float flySpeed = 500.0f;
  std::string currentScene;
  bool m_scroll;

  Vec3 startScale;

  Action actions;
#endif
};

#endif

template<typename CONTAINER>
inline void Editor::Filter(
  std::string& label, bool& open, CONTAINER & container, int size)
{
  //ImGui::Begin(label.data(), &fileMenuWindow,
  //  ImGuiWindowFlags_AlwaysAutoResize
  //  );
  ImGui::OpenPopup(label.data());
  if (ImGui::BeginPopupModal(label.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ++openPopupsCount;

    static ImGuiTextFilter filter;
    /* ImGui::Text("Filter usage:\n"
       "display all lines\n"
       "xxx      display lines containing xxx\n"
       "xxx, yyy  display lines containing xxx or yyy\n"
       "- xxx     hide lines containing xxx");*/
    filter.Draw("Search");
    for (int i = 0; i < size; i++)
    {
      if (filter.PassFilter(container[i].data()))
      {
        //ImGui::BulletText("%s", container[i].data());
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Selected;
        if (ImGui::TreeNodeEx(container[i].data(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
          if (ImGui::IsItemClicked())
          {
            //load level/scene
            ASC_FACTORY.LoadScene(std::string(ASC_SCENES_FLD + container[i]).c_str());
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
    ImGui::EndPopup();
  }
}
