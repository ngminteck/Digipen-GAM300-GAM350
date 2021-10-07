/******************************************************************************/
/*!
\file   BaseComponent.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef BASE_COMP_H
#define BASE_COMP_H
class ODENode;
struct BaseComponent
{
  BaseComponent() {}
  virtual ~BaseComponent() {}
  bool Enabled = true;
  //GOHandle m_GOHandle;
  ASC_VarGetSet(GOHandle, GOHandle)
};

template<typename derived>
struct Component : BaseComponent
{
  //Handle m_selfHandle;
  ASC_VarGetSet(Handle<derived>, SelfHandle)
};


///////////////transform and parent stuff///////////////////
struct pchdlWrapper;
struct ParentingObj;
typedef Handle<ParentingObj> pchdl;
typedef Handle<pchdlWrapper> pcWrapHdl;

struct TransformComp : public Component<TransformComp>
{
  TransformComp() :
    localpos(Vec3(0.0f, 0.0f, 0.0f)),
    localscale(Vec3(1.0f, 1.0f, 1.0f)),
    pos(localpos), scale(localscale),
    dirty(true) {}
  /* activated only during parenting ... else just use the normal one*/

  Vec3 localpos;
  Vec3 localscale;
  Quaternion localrotation;
  Matrix4 localmat;

  //handle to parenting obj
  pchdl parentinghdl;

  Vec3 pos;
  Vec3 scale;
  Quaternion rotation;
  Matrix4 mat;
  bool dirty;
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::TRANSFORM;

  //methods
  Handle<TransformComp> GetParent() const;
  Handle<TransformComp> GetChild(int index) const;
  Handle<TransformComp> FindChild(std::string const& cname) const;

  inline void AssignWorldToLocalProperties()
  {
    localpos = pos;
    localscale = scale;
    localrotation = rotation;
  }
};

struct pchdlWrapper
{
  pchdl m_root;
  ASC_VarGetSet(pcWrapHdl, SelfHandle)
};

struct ParentingObj
{
  //m_root will point to the very root of the tree
  pcWrapHdl m_root;
  Handle<TransformComp> m_trans;
  pchdl m_parent;
  std::vector<pchdl> m_child;

  ASC_VarGetSet(pchdl, SelfHandle)
};

//mainly for objectFactory to use for serialization
struct ChildIterate
{
  ChildIterate(ParentingObj& patobj) : m_pobj(&patobj), m_curr(m_pobj->m_child.begin()) {}
  Handle<TransformComp> Next();
  Handle<TransformComp> currTransHdl;
private:
  ParentingObj* m_pobj;
  std::vector<pchdl>::iterator m_curr;
};
///////////////transform and parent stuff///////////////////


//Should merge with graphics component after pipeline is complete
struct MeshMaterial
{
  std::string AlbedoMap;   //Diffuse Map
  std::string GlossMap;    //Metal | Shininess Map
  std::string AOMap;       //Ambient Occulsion Map
  std::string TangentMap;  //Normal Map
  float Roughness, Metalness;
  MeshMaterial() : AlbedoMap("Blank"), GlossMap("Blank"),
    AOMap("Blank"), TangentMap("Blank"), Roughness(5.0f),
    Metalness(0.0f) {}
};

struct GraphicsComp : Component<GraphicsComp>
{
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::GRAPHICS;
  Handle<TransformComp> positionHdl;
  bool autoScale, isUI, isHighlighted;
  bool mapFromWorld;
  Vec4 tint;
  Vec3 posOffset, scaleOffset;
  Vec2 uvOffset, tile;
  float zRotation;

  void SetTransparencyVal(float value) { tint.a = value; };
  MeshMaterial material;
  std::vector<MeshMaterial> materialList;

  std::string modelfile;
  GraphicsComp() : positionHdl(),
    autoScale(true), isUI(false),
    isHighlighted(false), mapFromWorld(false), tint(1.0f, 1.0f, 1.0f, 1.0f), scaleOffset(1.0f,1.0f,1.0f),
    tile(1.0f, 1.0f), modelfile("Cube"), zRotation(0) 
  {
  }

  void SetVisible(bool val);
  bool GetVisible() const;
  void SetHighlight(bool val);
  void SetColor3(Vec4 c);
};

struct LightComp : Component<LightComp>
{
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::LIGHT;
  size_t handle;

  LightComp() : handle(~0) {}
};

struct LogicComp : Component<LogicComp>
{
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::LOGIC;
  inline void AddScript(int script_idx) { scriptsHdl.push_back(script_idx); }
  std::vector<int> scriptsHdl;
  //int parent_index;
};
struct KeyFrameData
{
    int lerp;
    Vec3 oldData;

    Vec3 vector3fDataStart;
    Vec3 vector3fDataCurr;
    Vec3 vector3fDataSet;
};

struct KeyFrame
{
    KeyFrameData keyFrameData;
    float GetWeight(float curr, float, float next);
    float startTime;
    float duration;
    bool isLocal = true;
    int currWeight = 0;
};
enum AnimationType
{
    TEXTURE = 0,
    SKELETON,
    BOTH
};
struct AnimationComp : Component<AnimationComp>
{

    static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::ANIMATION;
    std::string nameText;
    std::string nameModel;
    Handle<TransformComp> positionHdl;
    bool isLoop;
    bool isPlaying;
    bool reset;
    float currSpeed;
    float currTime[3];
    float maxTime[3];
    int numOfTex;
    int numOfModel;
    int currIndex[3];
    AnimationType type;
    std::map<std::string, KeyFrame> listOfKeyFrame;
    AnimationComp() :
        nameText(""),
        nameModel(""),
        isLoop(false),
        isPlaying(false),
        reset(false),
        currSpeed(0.016f),
        currTime{ 0.0f, 0.0f, 0.0f },
        maxTime{0.0f, 0.0f,0.0f},
        numOfTex(0),
        numOfModel(0),
        currIndex{ 0,0,0 },
        type(AnimationType::TEXTURE)
    {

    }

};

enum SoundType
{
  //INVALID = 0,
  BGM,
  SFX
};

enum SoundState
{
  PLAY,
  PAUSE,
  STOP,
  PROCESSED,
  LOOP
};


//for sound man
struct SoundInfo;
typedef Handle<SoundInfo> SinfoHandle;

struct SoundInfo
{
  SoundInfo() {}
  SoundInfo(const SoundInfo& s)
    : inUse(s.inUse), type(s.type),
    sound3d(s.sound3d), volume(s.volume),
    pitch(s.pitch), listener(s.listener), minDist(s.minDist), maxDist(s.maxDist), noDie(s.noDie),
    name(s.name)
  {}

  bool inUse = true;    //for editor remove audio track from component
  //std::string name = "Default";
  SoundType type = SFX;
  bool sound3d = true;
  float volume = 1.0f;
  float pitch = 1.0f;
  bool listener = false;
  float minDist = 5.0f;
  float maxDist = 20.0f;
  bool noDie = false;
  int loopCount = 1;

  SoundState state = PROCESSED;
  FMOD::Channel* ch = nullptr;
  std::string name; //soundfile name

  ASC_VarGetSet(SinfoHandle, SelfHandle)
};


struct SoundComp : Component<SoundComp>
{
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::SOUND;
  //handle to soundinfo
  //std::vector<SinfoHandle> handles;
  typedef std::unordered_map<std::string, SinfoHandle> sCont;
  typedef sCont::iterator sIter;
  std::unordered_map<std::string, SinfoHandle> handles;
};

struct PhysicsComp : Component<PhysicsComp>
{
  static constexpr COMPONENT_TYPE CompEnum = PHYSICS;
  PhysicsComp() :
    id(-1), collision_type{ "STATIC" }
  { 
      collision_filter_list.emplace("STATIC", "IGNORE");
      collision_filter_list.emplace("DYNAMIC", "IGNORE");
      collision_filter_list.emplace("TRIGGER", "IGNORE");
      collision_filter_list.emplace("PLAYER", "IGNORE");
      collision_filter_list.emplace("OTHER", "IGNORE");
  }

  //to store the transform handle right from the start
  ASC_VarGetSet(Handle<TransformComp>, TransHdl);
  size_t id;
 
  std::string collision_type;
  std::unordered_map<std::string, std::string> collision_filter_list;
  
  void SetGravity(bool val) const;
  void SetMass(float val) const;
  float GetMass() const;

  void AddForce(const Vec3 &force) const;
  void AddVelocity(const Vec3 &vel);
  void AddShape(int shape); // Supposed to be shape enum
  float GetRadius() const;
  Vec3 GetVelocity() const;
  float GetForceMuliplier() const;
  void SetDynFriction(float val) const;
  void Stop() const;
  void SetStatic(bool flag) const;
  void SetVelocity(Vec3 vel);

  void SetCollisionType(std::string new_collision_type);
  std::string GetCollisionType()const;

};

struct UITextComp : Component<UITextComp>
{
  static constexpr COMPONENT_TYPE CompEnum = UITEXT;
  UITextComp() : tint(1.0f, 1.0f, 1.0f, 1.0f), collider(0.0f, 0.0f), isMouseOver(false), 
    fontTextureID(0), is2D(false) {}

  Vec3 offset;

  //to store the transform handle right from the start
  ASC_VarGetSet(Handle<TransformComp>, TransHdl);

  void SetVisible(bool val);
  bool GetVisible() const;
  void SetText(std::string&& str);
  void SetText(const std::string& str);

  std::string text;
  Vec4 tint, highlight;
  float scale = 1.0f;
  Vec2 collider;
  bool isMouseOver;

private:
  friend class UITextManager;

  float size = 120;
  uint32_t textureWidth = 0;
  uint32_t textureHeight = 20;
  GLuint fontTextureID;
  std::string textureBG;
  Vec4 currCol;
  bool is2D;
};

struct WayPointNode : Component<WayPointNode>
{
  static constexpr COMPONENT_TYPE CompEnum = WAYPOINT;
  ASC_VarGetSet(Handle<TransformComp>, TransHdl);

  WayPointNode() {}
  WayPointNode(const Vec3 &pos) {}

  //Vec3 position;
  std::vector<GOHandle> neighbours;

  // Used for saving/loading
  std::vector<Vec3> neighboursPos;

  int weights = 0;
};

struct Path
{
  std::list<WayPointNode*> nodes;
};
/**/
struct TagComp : Component<TagComp>
{
    static constexpr COMPONENT_TYPE CompEnum = TAG;

    TagComp();
    TagComp(const TagComp& tag);
    ~TagComp();

    std::string GetTagValue(std::string key);
    int GetTagValueInt(std::string key);
    double GetTagValueDouble(std::string key);
    bool GetTagValueBoolean(std::string key);

    void ChangeExistedTagKey(std::string old_key, std::string new_key);
    void ChangeExistedTagValue(std::string key, std::string new_value);
    void AddTag(std::string key, std::string value);
    void DeleteTag(std::string key);

    std::string MakeUniqueKey(std::string key, int counter = 0);

    std::unordered_map<std::string, std::string> tag_container;
};


struct NewPhysicsComp : Component<NewPhysicsComp>
{
    static constexpr COMPONENT_TYPE CompEnum = NEWPHYSICS;

    NewPhysicsComp();

    virtual ~NewPhysicsComp();

    ASC_VarGetSet(Handle<TransformComp>, TransHdl);

    void DestroyPhysicsComp();
   
    ODENode * obj_phy;
    ODENode * obj_trigger;
    ODENode * obj_ray;
    ODENode* obj_ray_platform;
    ODENode * obj_floor_ray;
    ODENode * obj_floor_ray_platform;

    bool prevsBoolForGrav;

    bool RayCastObjectPhysics(const Vec3& pos, const Vec3& dir, GOHandle go, dReal length = 10.0f);

    bool RayCastObjectTrigger(const Vec3& pos, const Vec3& dir, GOHandle go, dReal length = 10.0f);


    GOHandle RayCast(const Vec3& pos, const Vec3& dir, dContact& return_contact_info, dReal length = 10.0f);

    GOHandle RayCastFloor(const Vec3& pos, const Vec3& dir, dContact& return_contact_info, dReal length = 10.0f);
  





    void InitForwardRay(const Vec3& pos, const Vec3& dir,dReal length = 10.0f);

    void InitFloorRay(const Vec3& pos, const Vec3& dir, dReal length = 10.0f);
 
   
};



#endif

