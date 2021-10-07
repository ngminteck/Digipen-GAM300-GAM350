#pragma once


#define MAX_CONTACTS 1024

typedef ComponentManagerBase<NewPhysicsComp>::COMPHANDLETYPE NewPhyHandle;
#define ASC_NEWPHYSICSMANAGER NewPhysicsManager::Inst()
class NewPhysicsManager : public ComponentManagerBase<NewPhysicsComp>, public Singleton<NewPhysicsManager>
{
public:
    explicit NewPhysicsManager(HandleManager<TransformComp>& transform_comps);
    ~NewPhysicsManager();

    void Load() override;
    void UnLoad() override;

    void Update(float dt) override;
    void UpdateTransformToPhysics();
    void UpdateCCD();
    void UpdatePhysicsToTransform();

    void  DeserShape         (NewPhysicsComp& gameObject, Deserializer&);
    void  DeserBody          (NewPhysicsComp& gameObject, Deserializer&);
    void  DeserPhysicsSurface(NewPhysicsComp& gameObject, Deserializer&) ;
    void  DeserCCD(NewPhysicsComp& gameObject, Deserializer&);
    void  DerIgnore(NewPhysicsComp& gameObject, Deserializer&);
    BaseHandle Deserialize   (const GameObject& gameObject, Deserializer&) override;

    void SerShape(Serializer&, BaseHandle);
    void SerBody(Serializer&, BaseHandle);
    void SerPhysicsSurface(Serializer&, BaseHandle);
    void SerCCD(Serializer&, BaseHandle);
    void SerIgnore(Serializer&, BaseHandle);
    void Serialize(Serializer&, BaseHandle) override;

    void Reflect(BaseHandle, const Reflector&) override;

    BaseHandle CreateComponent() override;
    void ConstructUserDefault(NewPhysicsComp *comp, TransformComp * tc);

    void DestroyComponent(BaseHandle hdl) override;
    void InitComponent(const GameObject& go, BaseHandle hdl) override;


    //function from CompManBase
    void CopyComponentToComponent(NewPhysicsComp& target, const NewPhysicsComp& source) override;

    //For Msging
    void HandleMsg(MsgData_Base*) override;
    void HandleForceReg(MsgData_Base*);
    void HandleForceRegIgnoreList(MsgData_Base*);

    void InitODESerialize();
    void InitODE();
    void DestroyODE();

    void WorldSetting();

    // trimesh
    void GenerateBoundingSphere(ODENode* pc, TransformComp* tc, std::string& model_file);
    void GenerateBoundingBox(ODENode* pc, TransformComp* tc, std::string& model_file);
    void GenerateTriMesh(ODENode* pc, TransformComp* tc, std::string& model_file);

    void GenerateTriggerBox(ODENode* phy, ODENode* trigger);

    // PHY stuff 
    dWorldID* Phy_GetWorldPtr();
    dSpaceID* Phy_GetSpacePtr();
    void      Phy_SetWorldGravity(double);
    dReal     Phy_GetWorldGravity();
    
    bool isEnabled = false;

    int initODE;

    dWorldID world;
    dSpaceID space;
    dJointGroupID contactgroup;

    bool world_space_is_alive;
    bool init_le;

    float fps_remainder = 0.0f;
    float fps_limit = 1.0f / 60.0f;

    bool CanUpdateInput = false;


    std::unordered_map< ODENode* , std::unordered_map<ODENode*, MsgType>* > ccd_list;

    bool changesceneinithack = false;

    bool force_update_once = false;

private:
	
   
    
	HandleManager<TransformComp>& m_transform;

};

