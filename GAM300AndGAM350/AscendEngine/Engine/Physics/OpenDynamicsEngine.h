#pragma once


class ODENode
{
public:
    ODENode(NewPhysicsComp * np);

    ~ODENode();

  
    //int node_no;
    //
    //ODEBranch* my_branch;
    //ODEBranch* child_branch;

    NewPhysicsComp* npc;

    dWorldID* comp_world;
    dSpaceID* comp_space;


    // ode variable
    dBodyID body;
    dGeomID geom;
    dMass mass;
   // dContact contact;
   // dJointGroupID joint;
    dTriMeshDataID trimesh_data;

    Vec3 model_pivot_point;
    // collision offset
    Vec3 collision_pos_offset;

    // geometry
    std::string geometry;

    dReal radius;
    dReal length;
    dVector3 box_length;

    // for capsule and cyclinder
    std::string direction_axis;
    // tri mesh
    std::vector<dReal>vertex;// save as float 
    std::vector<dTriIndex>indices; // save as int 

    bool have_body;
    bool apply_physics_surface_collision;
    std::map<std::string, bool>surface_mode_list;

    std::vector<std::string> ignore_collision_list;


    bool continue_collision_detection;

    bool have_triggerbox;

    bool hack_always_able_raycast;
  

    std::string hack_type;
  

    std::unordered_map<std::string, MsgType> continue_collision_detection_list_string;
    std::unordered_map<ODENode*, MsgType> continue_collision_detection_list;

    //flag to check obj collided in frame
    bool debug_collided;

    bool legit_moving;

    void UpdateTransformInfo(TransformComp* tc);
    void UpdateGeomDimensionWithScale(Vec3& scale);

    void SetTrimeshCurrentTransform();

    void DebugDangerFunction();

    int GetDirectionInt(std::string value);

    void CreateGeometry();
    void CreateTrimesh();


    void GeomDraw(Vec4 nc_color = Color::BLACK, Vec4 c_color = Color::GREY, Vec3 pos ={ 0,0,0 });

    void GeomCollisionSetEnable(bool value);
    bool GeomCollisionIsEnabled()const;


    dReal GeomSphereGetRadius();
    void GeomBoxGetLengths(dVector3 result);
    void GeomCapsuleGetParams(dReal* radius, dReal* length);
    void GeomCylinderGetParams(dReal* radius, dReal* length);
    void GeomPlaneGetParams(dVector4 result);
    dReal GeomRayGetLength();

    void GeomSphereSetRadius(dReal radius);

    dReal GeomSpherePointDepth(dReal x, dReal y, dReal z);
    void GeomBoxSetLengths(dReal lx, dReal ly, dReal lz);

    dReal GeomBoxPointDepth(dReal x, dReal y, dReal z);
    void GeomCapsuleSetParams(dReal radius, dReal length);

    dReal GeomCapsulePointDepth(dReal x, dReal y, dReal z);
    void GeomCylinderSetParams(dReal radius, dReal length);

    void GeomPlaneSetParams(dReal a, dReal b, dReal c, dReal d);
    dReal GeomPlanePointDepth(dReal x, dReal y, dReal z);

    void GeomRaySetLength(dReal length);
    void GeomRaySet(dReal px, dReal py, dReal pz, dReal dx, dReal dy, dReal dz);
    void GeomRayGet(dVector3 start, dVector3 dir);

    void SetHaveBody(bool value);
    bool HaveBody()const;

    void SetRigidBodyEnable(bool value);
    bool IsRigidBodyEnable() const;

    void BodySetGravityEnable(bool value);
    bool BodyGetGravityMode();

    void BodySetPosition(const Vec3& pos);
    void BodySetPosition(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f);
    void BodySetQuaternion(const Quaternion& rot);
    void BodySetQuaternion(const dQuaternion& rot);
    void BodySetQuaternion(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f, const dReal& w = 0.0f);
    void BodySetLinearVel(const Vec3& vel);
    void BodySetLinearVel(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f);
    void BodySetAngularVel(const Vec3& avel);
    void BodySetAngularVel(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f);

    Vec3 BodyGetLinearVel();
    Vec3 BodyGetAngularVel();
    Vec3 BodyGetForce();
    Vec3 BodyGetTorque();

    void GeomSetPosition(const Vec3& pos);
    void GeomSetPosition(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f);
    void GeomSetQuaternion(const Quaternion& rot);
    void GeomSetQuaternion(const dQuaternion& rot);
    void GeomSetQuaternion(const dReal& x = 0.0f, const dReal& y = 0.0f, const dReal& z = 0.0f, const dReal& w = 0.0f);

    void UpdateMassInfo();
    void PreventMassCrash();

    void BodySetMass(const dMass* mass);
    void BodyGetMass(dMass* mass);

    void BodyAddForce(Vec3);
    void BodyAddForce(dReal fx, dReal fy, dReal fz);
    void BodyAddTorque(dReal fx, dReal fy, dReal fz);
    void BodyAddRelForce(Vec3);
    void BodyAddRelForce(dReal fx, dReal fy, dReal fz);
    void BodyAddRelTorque(dReal fx, dReal fy, dReal fz);
    void BodyAddForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
    void BodyAddForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
    void BodyAddRelForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
    void BodyAddRelForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);

    void BodySetForce(dReal x, dReal y, dReal z);
    void BodySetTorque(dReal x, dReal y, dReal z);


    void BodySetDynamic();
    void BodySetKinematic();
    int BodyIsKinematic();

    void BodySetDampingDefaults();
    void BodySetLinearDamping(dReal scale);
    void BodySetAngularDamping(dReal scale);

    dReal GetMass();
    void MassAdjust(dReal new_mass);

    void MassSetParameters(dReal themass, dReal cgx, dReal cgy, dReal cgz, dReal I11, dReal I22, dReal I33, dReal I12, dReal I13, dReal I23);

    //void InitSurface();

    //void UpdateSurfaceMode();

    void SetApplyPhysicsSurfaceEnable(bool value);
    bool IsApplyPhysicsSurfaceEnable() const;

   void InitCCD();

    //void SetSurfaceMode(std::string mode);


};