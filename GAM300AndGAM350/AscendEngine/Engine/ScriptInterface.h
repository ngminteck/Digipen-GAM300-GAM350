/******************************************************************************/
/*!
\file   ScriptInterface.h
\author Zelin ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for Serializer & deserializer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef SI_H
#define SI_H

#define ASC_SERIAL_NAME "Name"
#define ASC_GETSCRIPTNAME(ScriptEnumName) SItype::TypeToStr[ScriptIndex::SI_##ScriptEnumName]
#define ASC_SETUP_SCRIPT(ScriptEnumName) name = ASC_GETSCRIPTNAME(ScriptEnumName); script_type = SItype::StrToType[name];
#define ASC_SCRIPTNAME_PREFIX "s"
#define ASC_SCRIPT_REGISTER_MSG(msgType, function) msgList.push_back(ASC_MSGSYS.RegisterCallBack(msgType, &std::remove_pointer_t<decltype(this)>::function, this));
#define ASC_GETGONAME() ASC_GETGO_PTR(goHdl)->name
#define ASC_GETGOTYPE() ASC_GETGO_PTR(goHdl)->GetType()

namespace SCRIPTING
{
  struct ObjInfor
  {
    int missionNum = -1;
    int misionlvl = -1; 
    int amtOfTimes = -1;

    bool disableAtStart = false;
    // this will say it will enable all this mission num to be true 
   int targetToEnable;
   // ObjectiveManager::LEVELS belongToWhichLevel = ObjectiveManager::LEVELS::BEDROOM;
  };

  struct ScriptInterface
  {
    ScriptInterface();
    explicit ScriptInterface(const std::string& name);
    virtual ~ScriptInterface();

    void ScriptUpdateHandle();
    virtual void ScriptUpdate(float) = 0;
    virtual void ScriptSerialize(Serializer &sr, LogicHandle idx) = 0;
    virtual void ScriptDeserialize(Deserializer &xml) = 0;
    virtual void ScriptInitialize() = 0;
    //virtual ScriptInterface* CreateNew() const = 0;
    virtual ScriptPoolInfo CreateNew(std::string name) const = 0;
    virtual ScriptInterface* CreateNew_Raw(std::string name) const = 0;

    virtual void ForceRegStuff() {};

    virtual void Reflect(const Reflector&);
    virtual void ScriptStart() = 0;
    virtual void ScriptDestroy() = 0;
    virtual void OnCollisionEnter(GOHandle);
    virtual void OnCollisionStay(GOHandle);
    virtual void OnCollisionExit(GOHandle);
    void DestroySelf() const;

    void InitHandle(GOHandle idx);
    const std::string& GetName() const;


    ScriptIndex GetScriptType() const;
    GOHandle GetGOHandle() const { return goHdl; }
    GOHandle* GetGOHandlePtr(){ return &goHdl; }

    /*
    template<typename T>
    T* GetComponent(T*& ptr)
    {
      //Get comp ptr from GO this script is attached to
      COMPONENT_TYPE type = ptr->CompEnum;
      assert(hdl_arr[type].IsValid());
      ptr = static_cast<T*>(ptr_arr[type]);
      assert(ptr);
      return ptr;
      //return ASC_FACTORY.GetComponent<T>(goHdl);
    }
    */

    bool IsActive() const { return m_active; }
    void SetActive(bool a) { m_active = m_lastActive = a; }
    void Pause() {
      if (m_active)
      {
        m_lastActive = m_active;
        if (!m_persist)
        {
          m_active = false;
        }
      }
    }
    void Resume() { m_active = m_lastActive; }

    static GameObject &GetGameObjectFromHandle(GOHandle objHandle);

    bool inUse = true; //for logicmanager reflect 

    //TweenInfoRef tweenRef;  //this shld not be used!
    //TweenInfoCopy tweenCopy;
    //Blink blink;

    static int scriptCount;

    //for internal exchange of msg between scripts
    //a.k.a broadcast only to object's script
    virtual void HandleInternalMsg(const Msg_Resize& msg) { }
    virtual void HandleInternalMsg(const Msg_Change_Scene& msg) { }
    virtual void HandleInternalMsg(const Msg_Logic_OnCollide& msg) { }
    virtual void HandleInternalMsg(const Msg_UINT& msg) { }
    virtual void HandleInternalMsg(const Msg_Error& msg) { }
    virtual void HandleInternalMsg(const Msg_Game_FloatVal& msg) { }
    virtual void HandleInternalMsg(const Msg_Game_IntVal& msg) { }
    virtual void HandleInternalMsg(const Msg_Game_Vec3Val& msg) { }
    virtual void HandleInternalMsg(const Msg_Game_Vec4Val& msg) { }

   // bool InTheCollingList(dGeomID* address);
  protected:
   // std::vector<std::string > nameToSoundFileName;
    std::string name;
    ScriptIndex script_type = ScriptIndex::SI_MAX_SCRIPT;

    GameObject* obj = nullptr;
    GOHandle goHdl;
    BaseHandle hdl_arr[COMPONENT_TYPE::MAX];
    //BaseComponent* ptr_arr[COMPONENT_TYPE::MAX];

    //for update
    Handle<TransformComp>   t;
    Handle<PhysicsComp>     p;
    Handle<GraphicsComp>    g;
    Handle<SoundComp>       s;
    Handle<LightComp>       l;
    Handle<UITextComp>      u;
    Handle<ParticleEmitter> pe;
    Handle<TagComp>         tc;
    Handle<NewPhysicsComp>  np;
    float runTime;

    Color targetline_color = Color::BLACK;
    //Msging
    MessagingSystem::ReturnTokenList msgList;

    // Helper function
    //template<typename T> 
    //void RegisterMsg(MsgType msgType, void (T::*fn)(MsgData_Base*));
    template<typename T>
    void RegisterMsg(MsgType msgType, T &&fn);
    static void SendMsg(MsgData_Base &&msg);
    static GOHandle LoadFromPrefab(const std::string& name);

    template<typename T>
    T* GetComponent(GOHandle goHdl);
    template<typename T>
    T* GetComponent();
    template<typename T>
    static T* GetScript(GOHandle goHdl)
    {
      static T dummy;
      return static_cast<T *>(GetScript_fromGO(goHdl, dummy.script_type));
    }

    bool &GetPersist() { return m_persist; }

  private:
    bool m_active;
    bool m_lastActive;
    bool m_persist;


  
  };

  /*template <typename T>
  void ScriptInterface::RegisterMsg(MsgType msgType, void(T::*fn)(MsgData_Base*))
  {
    msgList.push_back(ASC_MSGSYS.RegisterCallBack(msgType, &fn, this));
  }*/

  template <typename T>
  void ScriptInterface::RegisterMsg(MsgType msgType, T&& fn)
  {
    msgList.push_back(ASC_MSGSYS.RegisterCallBack(msgType, std::forward<T>(fn)));
  }

  template <typename T>
  T* ScriptInterface::GetComponent(GOHandle goHdl)
  {
    auto ptr = ASC_FACTORY.GetComponentPtr<T>(goHdl);
    assert(ptr);
    return ptr;
  }

  template <typename T>
  T* ScriptInterface::GetComponent()
  {
    return ASC_FACTORY.GetComponentPtr<T>(goHdl);
  }

  template<typename Derived>
  class ScriptCommonIF : public ScriptInterface
  {
  public:
    ScriptCommonIF()
    {
    }

    ScriptCommonIF(const std::string& name) : ScriptInterface(name)
    {
      runTime = 0.0f;
    }
    std::vector<std::string > nameToSoundFileName;
  //  std::unordered_map< std::string, std::string > nameToSoundFileName;

    //virtual ScriptInterface* CreateNew(std::string name) const override
    virtual ScriptPoolInfo CreateNew(std::string name) const override
    {
      std::unordered_map<ScriptIndex, std::vector<ScriptPoolInfo>>& map = ASC_SCRIPTMAN.recycle_map;
      ScriptIndex type = sitpInit.StrToType[name];
      if (map.find(type) != map.end())
      {
        std::vector<ScriptPoolInfo>& vec = map[type];
        if (!vec.size())
        {
          //Log("vec :", (int)vec.size(), "no recycled", name, "newing");
          //return new Derived
          return ScriptPoolInfo(false, -1, new Derived);
        }
        ScriptPoolInfo info = vec.back();
        ScriptInterface* script = vec.back().ptr;
        vec.pop_back();
        new ((void*)script) Derived();
        //Log(name, "script recycled, left :", (int)vec.size());
        //return script;
        return info;
      }
      else
      {
        //Log(name, " no types, newing");
        //return new Derived();
        return ScriptPoolInfo(false, -1, new Derived);
      }
    }
    virtual ScriptInterface* CreateNew_Raw(std::string name) const override
    {
      return new Derived();
    }

    //just for this class....and derived class still need to override
    virtual void ScriptStart() final override {
      //logic man will help call this
      //ScriptInterface::ScriptStart();
      //anything regarding sending/Reg to stuff 
      ForceRegStuff();
      //scripting onstart
      OnStart();
    }

    virtual void ScriptDestroy() final override {
      OnDestroy();
      ScriptInterface::ScriptDestroy();
    }

    virtual void ScriptUpdate(float dt) final override {
      runTime += dt;
      ScriptInterface::ScriptUpdate(dt);
      Update(dt);
    }
    
    virtual void ScriptSerialize(Serializer &sr, LogicHandle idx) final override {
      sr.Write(GetName()); 
      Serialize(sr, idx);
      sr.Write("Persist", GetPersist());
      sr.EndAppend();
    }
    virtual void ScriptDeserialize(Deserializer &xml) final override {
      Deserialize(xml);
      xml.Read("Persist", GetPersist());
    }
    virtual void ScriptInitialize() final override
    {
        Initialize();
    }

  protected:
    virtual void OnStart() {}
    virtual void OnDestroy() {}
    virtual void Update(float) {}
    virtual void Reflect(const Reflector& r) override
    {
      r.Reflect("Sound", nameToSoundFileName, ASC_SOUNDMAN.GetAllTrack(), false);
    }
    virtual void Serialize(Serializer& sr, LogicHandle)
    {
      sr.Write("Sound", nameToSoundFileName);
    }
    virtual void Deserialize(Deserializer& d)
    {
      d.Read("Sound", nameToSoundFileName);
    }
    virtual void Initialize() {}
   // virtual void Reflect(const Reflector&) override;
    //virtual void OnCollisionEnter(Handle);
    //virtual void OnCollisionStay(Handle);
    //virtual void OnCollisionExit(Handle);
  };

 
}

#endif