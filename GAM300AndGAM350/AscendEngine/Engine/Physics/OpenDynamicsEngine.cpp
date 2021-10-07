#include "pch.h"



ODENode::ODENode(NewPhysicsComp* np)
    :npc{ np }, comp_world{ &ASC_NEWPHYSICSMANAGER.world },
  comp_space{ &ASC_NEWPHYSICSMANAGER.space },
  apply_physics_surface_collision{ true },
  have_body{ false }, body(0), geom(0),
  collision_pos_offset(0.0f, 0.0f, 0.0f),
  radius{ 1.0f }, length{ 1.0 }, box_length{ 1.0 , 1.0,1.0 },
  model_pivot_point(0.0f, 0.0f, 0.0f),
  continue_collision_detection{ false },
  debug_collided{ false },
  have_triggerbox{ false },
  hack_always_able_raycast{ false },
  legit_moving{true}
{
    geometry = "BOX";
    direction_axis = "ZAXIS";
    dMassSetZero(&mass);


    continue_collision_detection_list_string.clear();
    continue_collision_detection_list.clear();

    body = dBodyCreate(*comp_world);

    geom = dCreateBox(0, 1.0f, 1.0f, 1.0f);
    dMassSetBoxTotal(&mass, 1.0f, 1.0f, 1.0f, 1.0f);

    dQuaternion q;

    GeomSetPosition();
    GeomSetQuaternion(q);

    dQSetIdentity(q);
    BodySetPosition();
    BodySetQuaternion(q);
    BodySetLinearVel();
    BodySetAngularVel();


    // set mass info into the body
    //dBodySetMass(body, &mass);



    trimesh_data = dGeomTriMeshDataCreate();

    dGeomSetData(geom, this);

   // InitSurface();


    dSpaceAdd(*comp_space, geom);

    hack_type = "PHYSICS";


}



ODENode::~ODENode()
{
  
    //continue_collision_detection_list_string.clear();
    //continue_collision_detection_list.clear();


}

/*
void ODENode::InitSurface()
{
    surface_mode_list.emplace("dContactMu2", false);
    surface_mode_list.emplace("dContactAxisDep", false);
    surface_mode_list.emplace("dContactFDir1", false);
    surface_mode_list.emplace("dContactBounce", false);
    surface_mode_list.emplace("dContactSoftERP", false);
    surface_mode_list.emplace("dContactSoftCFM", false);
    surface_mode_list.emplace("dContactMotion1", false);
    surface_mode_list.emplace("dContactMotion2", false);
    surface_mode_list.emplace("dContactMotionN", false);
    surface_mode_list.emplace("dContactSlip1", false);
    surface_mode_list.emplace("dContactSlip2", false);
    surface_mode_list.emplace("dContactRolling", false);
    surface_mode_list.emplace("dContactApprox0", false);
    surface_mode_list.emplace("dContactApprox1_1", false);
    surface_mode_list.emplace("dContactApprox1_2", false);
    surface_mode_list.emplace("dContactApprox1_N", false);
    surface_mode_list.emplace("dContactApprox1", false);

    contact.surface.mode = dContactApprox0;

    contact.surface.mu = 0.0f;
    contact.surface.mu2 = 0.0f;

    contact.surface.rho = 0.0f;
    contact.surface.rho2 = 0.0f;
    contact.surface.rhoN = 0.0f;

    contact.surface.bounce = 0.0f;
    contact.surface.bounce_vel = 0.0f;

    contact.surface.soft_erp = 0.0f;
    contact.surface.soft_cfm = 0.0f;

    contact.surface.motion1 = 0.0f;
    contact.surface.motion2 = 0.0f;
    contact.surface.motionN = 0.0f;

    contact.surface.slip1 = 0.0f;
    contact.surface.slip2 = 0.0f;
}

void ODENode::UpdateSurfaceMode()
{
    // make all bits to 0
    contact.surface.mode &= 0;

    for (auto& it = surface_mode_list.begin(); it != surface_mode_list.end(); ++it)
    {
        if (it->second)
        {
            SetSurfaceMode(it->first);
        }
    }

}
*/
void ODENode::SetApplyPhysicsSurfaceEnable(bool value)
{
    apply_physics_surface_collision = value;
}

bool ODENode::IsApplyPhysicsSurfaceEnable() const
{
    return apply_physics_surface_collision;
}

void ODENode::InitCCD()
{
    continue_collision_detection_list.clear();

    for (auto& it = continue_collision_detection_list_string.begin(); it != continue_collision_detection_list_string.end(); ++it)
    {
        GOHandle handle = ASC_FACTORY.GetGameObjectByName(it->first);

        if (!handle.IsValid())
            continue;

        NewPhyHandle nph = handle->GetHdl<NewPhysicsComp>();

        if (nph.IsValid())
        {
            if(nph->obj_phy->have_triggerbox)
                 continue_collision_detection_list.emplace(nph->obj_trigger, it->second);
        }
    }
}
/**
void ODENode::SetSurfaceMode(std::string mode)
{

    if (mode.compare("dContactAxisDep") == 0)
    {
        contact.surface.mode |= dContactAxisDep;
    }
    else if (mode.compare("dContactFDir1") == 0)
    {
        contact.surface.mode |= dContactFDir1;
    }
    else if (mode.compare("dContactBounce") == 0)
    {
        contact.surface.mode |= dContactBounce;
    }
    else if (mode.compare("dContactSoftERP") == 0)
    {
        contact.surface.mode |= dContactSoftERP;
    }
    else if (mode.compare("dContactSoftCFM") == 0)
    {
        contact.surface.mode |= dContactSoftCFM;
    }
    else if (mode.compare("dContactMotion1") == 0)
    {
        contact.surface.mode |= dContactMotion1;
    }
    else if (mode.compare("dContactMotion2") == 0)
    {
        contact.surface.mode |= dContactMotion2;
    }
    else if (mode.compare("dContactMotionN") == 0)
    {
        contact.surface.mode |= dContactMotionN;
    }
    else if (mode.compare("dContactSlip1") == 0)
    {
        contact.surface.mode |= dContactSlip1;
    }
    else if (mode.compare("dContactSlip2") == 0)
    {
        contact.surface.mode |= dContactSlip2;
    }
    else if (mode.compare("dContactRolling") == 0)
    {
        contact.surface.mode |= dContactRolling;
    }
    else if (mode.compare("dContactMu2") == 0)
    {
        contact.surface.mode |= dContactMu2;
    }
    else if (mode.compare("dContactApprox1_1") == 0)
    {
        contact.surface.mode |= dContactApprox1_1;
    }
    else if (mode.compare("dContactApprox1_2") == 0)
    {
        contact.surface.mode |= dContactApprox1_2;
    }
    else if (mode.compare("dContactApprox1_N") == 0)
    {
        contact.surface.mode |= dContactApprox1_N;
    }
    else if (mode.compare("dContactApprox1") == 0)
    {
        contact.surface.mode |= dContactApprox1;
    }
    else
    {
        contact.surface.mode |= dContactApprox0;
    }
}
*/
void ODENode::SetHaveBody(bool value)
{

    have_body = value;
}

bool ODENode::HaveBody() const
{
    return have_body;
}

void ODENode::BodySetPosition(const Vec3& pos)
{
    if (body != NULL)
    dBodySetPosition(body, pos.m[0], pos.m[1], pos.m[2]);
}

void ODENode::BodySetPosition(const dReal& x, const dReal& y, const dReal& z)
{
    if (body != NULL)
    dBodySetPosition(body, x, y, z);
}


void ODENode::BodySetQuaternion(const Quaternion& rot)
{
    if (body != NULL && npc!=NULL)
    {
        dQuaternion q;
        q[0] = rot.m[0];
        q[1] = rot.m[1];
        q[2] = rot.m[2];
        q[3] = rot.m[3];
        dBodySetQuaternion(body, q);
    }
}

void ODENode::BodySetQuaternion(const dQuaternion& rot)
{
    if (body != NULL)
    dBodySetQuaternion(body, rot);
}

void ODENode::BodySetQuaternion(const dReal& x, const dReal& y, const dReal& z, const dReal& w)
{
    if (body != NULL && npc!=NULL)
    {
        dQuaternion q;
        q[0] = x;
        q[1] = y;
        q[2] = z;
        q[3] = w;
        dBodySetQuaternion(body, q);
    }
}

void ODENode::BodySetLinearVel(const Vec3& vel)
{
    legit_moving = true;
    dBodySetLinearVel(body, vel.m[0], vel.m[1], vel.m[2]);
}

void ODENode::BodySetLinearVel(const dReal& x, const dReal& y, const dReal& z)
{
    legit_moving = true;
    dBodySetLinearVel(body, x, y, z);
}

void ODENode::BodySetAngularVel(const Vec3& avel)
{
    dBodySetAngularVel(body, avel.m[0], avel.m[1], avel.m[2]);
}

void ODENode::BodySetAngularVel(const dReal& x, const dReal& y, const dReal& z)
{
    dBodySetAngularVel(body, x, y, z);
}

Vec3 ODENode::BodyGetLinearVel()
{
    const dReal* return_val = dBodyGetLinearVel(body);
    return Vec3(static_cast<float>(return_val[0]), static_cast<float>(return_val[1]), static_cast<float>(return_val[2]));
}

Vec3 ODENode::BodyGetAngularVel()
{
    const dReal* return_val = dBodyGetAngularVel(body);
    return Vec3(static_cast<float>(return_val[0]), static_cast<float>(return_val[1]), static_cast<float>(return_val[2]));

}

Vec3 ODENode::BodyGetForce()
{
    const dReal* return_val = dBodyGetForce(body);
    return Vec3(static_cast<float>(return_val[0]), static_cast<float>(return_val[1]), static_cast<float>(return_val[2]));

}

Vec3 ODENode::BodyGetTorque()
{
    const dReal* return_val = dBodyGetTorque(body);
    return Vec3(static_cast<float>(return_val[0]), static_cast<float>(return_val[1]), static_cast<float>(return_val[2]));

}

void ODENode::GeomSetPosition(const Vec3& pos)
{
    if (geom != NULL)
    dGeomSetPosition(geom, pos.m[0], pos.m[1], pos.m[2]);
}

void ODENode::GeomSetPosition(const dReal& x, const dReal& y, const dReal& z)
{
    if (geom != NULL)
    dGeomSetPosition(geom, x, y, z);
}

void ODENode::GeomSetQuaternion(const Quaternion& rot)
{
    
    if (geom != NULL && npc!= NULL)
    {
        dQuaternion q;
        q[0] = rot.m[0];
        q[1] = rot.m[1];
        q[2] = rot.m[2];
        q[3] = rot.m[3];
        dGeomSetQuaternion(geom, q);
    }
}

void ODENode::GeomSetQuaternion(const dQuaternion& rot)
{
    if(geom!=NULL && npc!=NULL)
    dGeomSetQuaternion(geom, rot);
}

void ODENode::GeomSetQuaternion(const dReal& x, const dReal& y, const dReal& z, const dReal& w)
{
    if (geom != NULL)
    {
        dQuaternion q;
        q[0] = x;
        q[1] = y;
        q[2] = z;
        q[3] = w;
        dGeomSetQuaternion(geom, q);
    }
}


void ODENode::BodySetMass(const dMass* mass)
{
    dBodySetMass(body, mass);
}

void ODENode::BodyGetMass(dMass* mass)
{
    dBodyGetMass(body, mass);
}

void ODENode::BodyAddForce(Vec3 vec)
{

    dBodyAddForce(body, vec.x, vec.y, vec.z);
}

void ODENode::BodyAddForce(dReal fx, dReal fy, dReal fz)
{
    dBodyAddForce(body, fx, fy, fz);
}

void ODENode::BodyAddTorque(dReal fx, dReal fy, dReal fz)
{
    dBodyAddTorque(body, fx, fy, fz);
}

void ODENode::BodyAddRelForce(Vec3 val)
{
    dBodyAddRelForce(body, val[0], val[1], val[2]);

}
void ODENode::BodyAddRelForce(dReal fx, dReal fy, dReal fz)
{
    dBodyAddRelForce(body, fx, fy, fz);
}

void ODENode::BodyAddRelTorque(dReal fx, dReal fy, dReal fz)
{
    dBodyAddRelTorque(body, fx, fy, fz);
}

void ODENode::BodyAddForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
    dBodyAddForceAtPos(body, fx, fy, fz, px, py, pz);
}

void ODENode::BodyAddForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
    dBodyAddForceAtRelPos(body, fx, fy, fz, px, py, pz);
}

void ODENode::BodyAddRelForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
    if (body != NULL)
    dBodyAddRelForceAtPos(body, fx, fy, fz, px, py, pz);
}

void ODENode::BodyAddRelForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
    if (body != NULL)
    dBodyAddRelForceAtRelPos(body, fx, fy, fz, px, py, pz);
}


void ODENode::BodySetForce(dReal x, dReal y, dReal z)
{
    if (body != NULL)
    dBodySetForce(body, x, y, z);
}

void ODENode::BodySetTorque(dReal x, dReal y, dReal z)
{
    if (body != NULL)
    dBodySetTorque(body, x, y, z);
}

void ODENode::BodySetDynamic()
{
    if (body != NULL)
    dBodySetDynamic(body);
}

void ODENode::BodySetKinematic()
{
    dBodySetKinematic(body);
}

int ODENode::BodyIsKinematic()
{
    return dBodyIsKinematic(body);
}

void ODENode::BodySetDampingDefaults()
{
    if (body != NULL)
    dBodySetDampingDefaults(body);
}

void ODENode::BodySetLinearDamping(dReal scale)
{
    if (body != NULL)
   dBodySetLinearDamping(body, scale);
}

void ODENode::BodySetAngularDamping(dReal scale)
{
    if (body != NULL)
    dBodySetAngularDamping(body, scale);
}

void ODENode::SetRigidBodyEnable(bool value)
{
    if (value)
    {
        dBodyEnable(body);
    }
    else
    {
        dBodyDisable(body);
    }
}

bool ODENode::IsRigidBodyEnable() const
{
    return dBodyIsEnabled(body);
}

void ODENode::BodySetGravityEnable(bool value)
{
    if (value)
    {
        dBodySetGravityMode(body, 1);
    }
    else
    {
        dBodySetGravityMode(body, 0);
    }
}

bool ODENode::BodyGetGravityMode()
{
    return dBodyGetGravityMode(body);
}

void ODENode::GeomCollisionSetEnable(bool value)
{
    if (value)
    {
        dGeomEnable(geom);
    }
    else
    {
        dGeomDisable(geom);
    }
}

bool ODENode::GeomCollisionIsEnabled() const
{
    return dGeomIsEnabled(geom);
}



void ODENode::GeomSphereSetRadius(dReal radius)
{
    if (!(geometry.compare("BOX") == 0)
        && !(geometry.compare("CAPSULE") == 0)
        && !(geometry.compare("CYCLINDER") == 0)
        && !(geometry.compare("PLANE") == 0)
        && !geometry.compare("RAY") == 0)
    {
        dGeomSphereSetRadius(geom, radius);
    }
}

dReal ODENode::GeomSphereGetRadius()
{
    if (!(geometry.compare("BOX") == 0)
        && !(geometry.compare("CAPSULE") == 0)
        && !(geometry.compare("CYCLINDER") == 0)
        && !(geometry.compare("PLANE") == 0)
        && !geometry.compare("RAY") == 0)
    {
        return dGeomSphereGetRadius(geom);
    }
    else
    {
        return 0.0f;
    }
}

dReal ODENode::GeomSpherePointDepth(dReal x, dReal y, dReal z)
{
    if (!(geometry.compare("BOX") == 0)
        && !(geometry.compare("CAPSULE") == 0)
        && !(geometry.compare("CYCLINDER") == 0)
        && !(geometry.compare("PLANE") == 0)
        && !geometry.compare("RAY") == 0)
    {
        return  dGeomSpherePointDepth(geom, x, y, z);
    }
    else
    {
        return 0.0f;
    }
}



void ODENode::GeomBoxSetLengths(dReal lx, dReal ly, dReal lz)
{
    if (geometry.compare("BOX") == 0)
    {
        // std::cout << geom << std::endl ;
        dGeomBoxSetLengths(geom, lx, ly, lz);
    }
}

void ODENode::GeomBoxGetLengths(dVector3  result)
{
    if (geometry.compare("BOX") == 0)
    {

        dGeomBoxGetLengths(geom, result);
    }

}

dReal ODENode::GeomBoxPointDepth(dReal x, dReal y, dReal z)
{
    if (geometry.compare("BOX") == 0)
    {
        return dGeomBoxPointDepth(geom, x, y, z);
    }
    else
    {
        return 0.0f;
    }
}

void ODENode::GeomCapsuleSetParams(dReal radius, dReal length)
{
    if (geometry.compare("CAPSULE") == 0)
    {
        dGeomCapsuleSetParams(geom, radius, length);
    }
}

void ODENode::GeomCapsuleGetParams(dReal* radius, dReal* length)
{
    if (geometry.compare("CAPSULE") == 0)
    {
        dGeomCapsuleGetParams(geom, radius, length);
    }
}

dReal ODENode::GeomCapsulePointDepth(dReal x, dReal y, dReal z)
{
    if (geometry.compare("CAPSULE") == 0)
    {
        return dGeomCapsulePointDepth(geom, x, y, z);
    }
    else
    {
        return 0.0f;
    }
}

void ODENode::GeomCylinderSetParams(dReal radius, dReal length)
{
    if (geometry.compare("CYCLINDER") == 0)
    {
        dGeomCylinderSetParams(geom, radius, length);
    }
}

void ODENode::GeomCylinderGetParams(dReal* radius, dReal* length)
{
    if (geometry.compare("CYCLINDER") == 0)
    {
        dGeomCylinderGetParams(geom, radius, length);
    }
}

void ODENode::GeomPlaneSetParams(dReal a, dReal b, dReal c, dReal d)
{
    if (geometry.compare("PLANE") == 0)
    {
        dGeomPlaneSetParams(geom, a, b, c, d);
    }
}

void ODENode::GeomPlaneGetParams(dVector4 result)
{
    if (geometry.compare("PLANE") == 0)
    {
        dGeomPlaneGetParams(geom, result);
    }
}

dReal ODENode::GeomPlanePointDepth(dReal x, dReal y, dReal z)
{
    if (geometry.compare("PLANE") == 0)
    {
        return dGeomPlanePointDepth(geom, x, y, z);
    }
    else
    {
        return 0.0f;
    }
}

void ODENode::GeomRaySetLength(dReal length)
{
    if (geometry.compare("RAY") == 0)
    {
        dGeomRaySetLength(geom, length);
    }
}

dReal ODENode::GeomRayGetLength()
{
    if (geometry.compare("RAY") == 0)
    {
        return dGeomRayGetLength(geom);
    }
    else
    {
        return 0.0f;
    }
}

void ODENode::GeomRaySet(dReal px, dReal py, dReal pz, dReal dx, dReal dy, dReal dz)
{
    if (geometry.compare("RAY") == 0)
    {
        dGeomRaySet(geom, px, py, pz, dx, dy, dz);
    }
}

void ODENode::GeomRayGet(dVector3 start, dVector3 dir)
{
    if (geometry.compare("RAY") == 0)
    {
        dGeomRayGet(geom, start, dir);
    }
}


void ODENode::UpdateTransformInfo(TransformComp* tc)
{
  
    if (geom != NULL)
    {
        UpdateGeomDimensionWithScale(tc->scale);
        GeomSetQuaternion(tc->rotation);
        GeomSetPosition(tc->pos + collision_pos_offset);
    }
    if (body != NULL)
    {
        BodySetQuaternion(tc->rotation);
        BodySetPosition(tc->pos + collision_pos_offset);
    }
 
}

void ODENode::UpdateGeomDimensionWithScale(Vec3& scale)
{
    if (!geometry.empty())
    {
        if (geometry.compare("BOX") == 0)
        {
            dGeomBoxSetLengths(geom, box_length[0] * scale.m[0], box_length[1] * scale.m[1], box_length[2] * scale.m[2]);
            UpdateMassInfo();
        }
        else if (geometry.compare("CAPSULE") == 0)
        {

            dReal scalar = scale.m[0];

            if (scale.m[1] > scalar)
            {
                scalar = scale.m[1];
            }

            dGeomCapsuleSetParams(geom, radius * scalar, length * scale.m[2]);
            UpdateMassInfo();

        }
        else if (geometry.compare("CYCLINDER") == 0)
        {
            dReal scalar = scale.m[0];

            if (scale.m[1] > scalar)
            {
                scalar = scale.m[1];
            }

            dGeomCylinderSetParams(geom, radius * scalar, length * scale.m[2]);
            UpdateMassInfo();
        }
        else if (geometry.compare("PLANE") == 0)
        {

        }
        else if (geometry.compare("RAY") == 0)
        {
            GeomRaySetLength(length * scale.m[2]);
            UpdateMassInfo();
        }
        else if (geometry.compare("SPHERE") == 0)
        {
            dReal scalar;
            scalar = scale.m[0];

            if (scale.m[1] > scalar)
            {
                scalar = scale.m[1];
            }

            if (scale.m[2] > scalar)
            {
                scalar = scale.m[2];
            }

            dGeomSphereSetRadius(geom, radius * scalar);
            UpdateMassInfo();
        }
    }

}

void ODENode::SetTrimeshCurrentTransform()
{
    const dReal* Pos = dGeomGetPosition(geom);
    const dReal* Rot = dGeomGetRotation(geom);

    const dReal Transform[16] =
    {
      Rot[0], Rot[4], Rot[8],  0,
      Rot[1], Rot[5], Rot[9],  0,
      Rot[2], Rot[6], Rot[10], 0,
      Pos[0], Pos[1], Pos[2],  1
    };

    dGeomTriMeshSetLastTransform(geom, *(dMatrix4*)(&Transform));
}

void ODENode::DebugDangerFunction()
{

   // PreventMassCrash();
#ifdef _RELEASE
    if (have_body)
    {
        
        
        dBodySetMass(body, &mass);
    }
#endif

#ifdef _SUBMISSION
    if (have_body)
    {


        dBodySetMass(body, &mass);
    }
#endif
}

void ODENode::MassSetParameters(dReal themass, dReal cgx, dReal cgy, dReal cgz, dReal I11, dReal I22, dReal I33, dReal I12, dReal I13, dReal I23)
{
    dMassSetParameters(&mass, themass, cgx, cgy, cgz, I11, I22, I33, I12, I13, I23);
}

dReal ODENode::GetMass()
{
    return mass.mass;
}

void ODENode::MassAdjust(dReal new_mass)
{
    mass.mass = new_mass;

    if (new_mass < 1.0f)
        mass.mass = 1.0f;

    UpdateMassInfo();
}


void ODENode::UpdateMassInfo()
{
    if (geometry.compare("BOX") == 0)
    {
        dVector3 l;
        GeomBoxGetLengths(l);

        dReal m = mass.mass;
        dMassSetZero(&mass);
        mass.mass = m;
        mass.I[(0) * 4 + (0)] = m / REAL(12.0) * (l[1] * l[1] + l[2] * l[2]);
        mass.I[(1) * 4 + (1)] = m / REAL(12.0) * (l[0] * l[0] + l[2] * l[2]);
        mass.I[(2) * 4 + (2)] = m / REAL(12.0) * (l[0] * l[0] + l[1] * l[1]);


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

        //PreventMassCrash();
        //dBodySetMass(body, &mass);

    }
    else if (geometry.compare("CAPSULE") == 0)
    {
        dReal r;
        dReal l;
        GeomCapsuleGetParams(&r, &l);

        dReal m = mass.mass;
        dMassSetZero(&mass);

        dReal density = 1.0f;
        dReal M1 = static_cast<dReal>(M_PI * r * r * l * density);
        dReal M2 = static_cast<dReal>((REAL(4.0) / REAL(3.0)) * M_PI * r * r * r * density);
        mass.mass = M1 + M2;

        dReal Ia = M1 * (REAL(0.25) * r * r + (REAL(1.0) / REAL(12.0)) * l * l) +
            M2 * (REAL(0.4) * r * r + REAL(0.375) * r * l + REAL(0.25) * l * l);

        dReal Ib = (M1 * REAL(0.5) + M2 * REAL(0.4)) * r * r;

        mass.I[(0) * 4 + (0)] = Ia;
        mass.I[(1) * 4 + (1)] = Ia;
        mass.I[(2) * 4 + (2)] = Ia;
        mass.I[(GetDirectionInt(direction_axis) - 1) * 4 + (GetDirectionInt(direction_axis) - 1)] = Ib;

        dReal scale = m / mass.mass;
        mass.mass = m;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                mass.I[(i) * 4 + (j)] *= scale;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);


        // PreventMassCrash();
        // dBodySetMass(body, &mass);

    }
    else if (geometry.compare("CYCLINDER") == 0)
    {
        dReal r2;
        dReal l;
        GeomCylinderGetParams(&r2, &l);

        dReal m = mass.mass;
        dMassSetZero(&mass);

        r2 = r2 * r2;
        mass.mass = m;
        dReal I = mass.mass * (REAL(0.25) * r2 + (REAL(1.0) / REAL(12.0)) * l * l);
        mass.I[(0) * 4 + (0)] = I;
        mass.I[(1) * 4 + (1)] = I;
        mass.I[(2) * 4 + (2)] = I;
        mass.I[(GetDirectionInt(direction_axis) - 1) * 4 + (GetDirectionInt(direction_axis) - 1)] = mass.mass * REAL(0.5) * r2;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);


        // PreventMassCrash();
        // dBodySetMass(body, &mass);

    }
    else if (geometry.compare("SPHERE") == 0)
    {
        dReal m = mass.mass;
        dMassSetZero(&mass);

        mass.mass = m;
        dReal II = REAL(0.4) * mass.mass * GeomSphereGetRadius() * GeomSphereGetRadius();
        mass.I[(0) * 4 + (0)] = II;
        mass.I[(1) * 4 + (1)] = II;
        mass.I[(2) * 4 + (2)] = II;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);


        //PreventMassCrash();
        //dBodySetMass(body, &mass);
    }
    else if (geometry.compare("TRIMESH") == 0)
    {


#ifdef _DEBUG 
        float m = mass.mass;

        dMassSetTrimesh(&mass, 1.0f, geom);

        dReal scale = m / mass.mass;
        mass.mass = m;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                mass.I[(i) * 4 + (j)] *= scale;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

        //  PreventMassCrash();
      //    dBodySetMass(body, &mass);

#endif 
#ifdef _RELEASE 
        dMassSetTrimeshTotal(&mass, mass.mass, geom);


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

        //   dBodySetMass(body, &mass);

#endif 



    }
    else
    {
        dReal m = mass.mass;
        dMassSetZero(&mass);
        mass.mass = m;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

        //PreventMassCrash();
        //dBodySetMass(body, &mass);
    }

}

void ODENode::PreventMassCrash()
{

    if (mass.mass < 1.0f)
        mass.mass = 1.0f;

    //mass.c[0] = 0.0f;
    //mass.c[1] = 0.0f;
    //mass.c[2] = 0.0f;

    //if (mass.c[0] < 0.0f)
    //    mass.c[0] = 0.0f;
    //
    //if (mass.c[1] < 0.0f)
    //    mass.c[1] = 0.0f;
    //
    //if (mass.c[2] < 0.0f)
    //    mass.c[2] = 0.0f;

    double biggest_value = mass.I[(0) * 4 + (0)];

    if (mass.I[(1) * 4 + (1)] > biggest_value)
    {
        biggest_value = mass.I[(1) * 4 + (1)];
    }

    if (mass.I[(2) * 4 + (2)] > biggest_value)
    {
        biggest_value = mass.I[(2) * 4 + (2)];
    }

    mass.I[(0) * 4 + (0)] = biggest_value;
    mass.I[(1) * 4 + (1)] = biggest_value;
    mass.I[(2) * 4 + (2)] = biggest_value;

    mass.I[(0) * 4 + (1)] = 0.0;
    mass.I[(0) * 4 + (2)] = 0.0;

    mass.I[(1) * 4 + (0)] = 0.0;
    mass.I[(1) * 4 + (2)] = 0.0;

    mass.I[(2) * 4 + (0)] = 0.0;
    mass.I[(2) * 4 + (1)] = 0.0;

    //if (mass.I[(0) * 4 + (0)] < 1.0f)
    //    mass.I[(0) * 4 + (0)] = 1.0f;
    //
    //if (mass.I[(0) * 4 + (1)] < 1.0f)
    //    mass.I[(0) * 4 + (1)] = 1.0f;
    //
    //if (mass.I[(0) * 4 + (2)] < 1.0f)
    //    mass.I[(0) * 4 + (2)] = 1.0f;
    //
    //if (mass.I[(1) * 4 + (0)] < 1.0f)
    //    mass.I[(1) * 4 + (0)] = 1.0f;
    //
    //if (mass.I[(1) * 4 + (1)] < 1.0f)
    //    mass.I[(1) * 4 + (1)] = 1.0f;
    //
    //if (mass.I[(1) * 4 + (2)] < 1.0f)
    //    mass.I[(1) * 4 + (2)] = 1.0f;
    //
    //if (mass.I[(2) * 4 + (0)] < 1.0f)
    //    mass.I[(2) * 4 + (0)] = 1.0f;
    //
    //if (mass.I[(2) * 4 + (1)] < 1.0f)
    //    mass.I[(2) * 4 + (1)] = 1.0f;
    //
    //if (mass.I[(2) * 4 + (2)] < 1.0f)
    //    mass.I[(2) * 4 + (2)] = 1.0f;
    //

}
// The cylinder /capsules long axis is oriented along the body's x, y or z axis according to the value of direction(1 = x, 2 = y, 3 = z).The first function accepts the density of the object, the second accepts its total mass.
int ODENode::GetDirectionInt(std::string value)
{
    if (value.compare("YAXIS") == 0)
    {
        return 2;
    }
    else if (value.compare("ZAXIS") == 0)
    {
        return 3;
    }
    else
    {
        return 1;
    }

}



void ODENode::CreateGeometry()
{
    dGeomDestroy(geom);
    dReal m = mass.mass;

    collision_pos_offset.m[0] = 0.0f;
    collision_pos_offset.m[1] = 0.0f;
    collision_pos_offset.m[2] = 0.0f;

    if (geometry.compare("BOX") == 0)
    {
        geom = dCreateBox(0, 1.0f, 1.0f, 1.0f);
        dMassSetBoxTotal(&mass, m, 1.0f, 1.0f, 1.0f);


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else if (geometry.compare("CAPSULE") == 0)
    {
        geom = dCreateCapsule(0, 1.0f, 1.0f);
        dMassSetCapsuleTotal(&mass, m, GetDirectionInt(direction_axis), 1.0f, 1.0f);


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else if (geometry.compare("CYCLINDER") == 0)
    {
        geom = dCreateCylinder(0, 1.0f, 3.0f);
        dMassSetCylinderTotal(&mass, m, GetDirectionInt(direction_axis), 1.0f, 1.0f);

        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else if (geometry.compare("PLANE") == 0)
    {
        geom = dCreatePlane(0, 1.0f, 1.0f, 1.0f, 1.0f);
        dMassSetZero(&mass);

        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else if (geometry.compare("RAY") == 0)
    {
        geom = dCreateRay(0, 1.0f);
        dMassSetZero(&mass);

        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else if (geometry.compare("TRIMESH") == 0)
    {
        // do nothing , prevent crash as unknown treat as sphere
        mass.mass = 1.0f;


        mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
        mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
        mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    }
    else
    {
        geom = dCreateSphere(0, 1.0f);
        dMassSetSphereTotal(&mass, m, 1.0f);
    }
    dGeomSetData(geom, this);

    dSpaceAdd(*comp_space, geom);
}

void ODENode::CreateTrimesh()
{

    std::cout << npc->GetGOHandle()->name <<" is using Trimesh" << std::endl;
    dReal m = 1.0;

    dGeomDestroy(geom);

    dGeomTriMeshDataDestroy(trimesh_data);

    m = mass.mass;



    int numv = static_cast<int>(vertex.size() / 3);
    int numi = static_cast<int>(indices.size());

    trimesh_data = dGeomTriMeshDataCreate();

   // dGeomTriMeshDataBuildSimple(trimesh_data, vertex.data(), vertex.size(), indices.data(), indices.size());

   dGeomTriMeshDataBuildDouble(trimesh_data, vertex.data(), 3 * sizeof(dReal), numv, indices.data(), numi, 3 * sizeof(dTriIndex));
   dGeomTriMeshDataPreprocess2(trimesh_data, (1U << dTRIDATAPREPROCESS_BUILD_FACE_ANGLES), NULL);

    geom = dCreateTriMesh(0, trimesh_data, 0, 0, 0);




#ifdef _DEBUG 
    // float mm = mass.mass;

     //dMassSetZero(&mass);

     //dMassSetTrimesh(&mass, 1.0f, geom);
     //dReal scale = mm / mass.mass;
     //mass.mass = mm;
     //
     //for (int i = 0; i < 3; i++)
     //    for (int j = 0; j < 3; j++)
     //    {
     //        mass.I[(i) * 4 + (j)] *= scale;
     //    
     //    }
     //



    mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
    mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
    mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    // PreventMassCrash();


     //dBodySetMass(body, &mass);


#endif 
#ifdef _RELEASE 



    dMassSetTrimeshTotal(&mass, m, geom);


    mass.c[0] = static_cast<dReal>(collision_pos_offset.m[0]);
    mass.c[1] = static_cast<dReal>(collision_pos_offset.m[1]);
    mass.c[2] = static_cast<dReal>(collision_pos_offset.m[2]);

    //dBodySetMass(body, &mass);

#endif 




    dGeomSetData(geom, this);

    dSpaceAdd(*comp_space, geom);




}

void ODENode::GeomDraw(Vec4 nc_color, Vec4 c_color,Vec3 offset)
{

    if (geometry.compare("TRIMESH") == 0)
    {
      
        if (have_body)
        {

            Vec4 color = Color::BLUE;

            if (debug_collided)
                color = Color::BLUE_LIGHT;


            const dReal* rot = dBodyGetRotation(body);
            dVector3 pos;
            dBodyCopyPosition(body, pos);
        
            float fpos[3] = { static_cast<float>(pos[0]),static_cast<float>(pos[1]),static_cast<float>(pos[2]) };
        
            float frot[12] = {  static_cast<float>(rot[0]),static_cast<float>(rot[1]),static_cast<float>(rot[2]),static_cast<float>(rot[3]),
                                static_cast<float>(rot[4]),static_cast<float>(rot[5]),static_cast<float>(rot[6]),static_cast<float>(rot[7]),
                                static_cast<float>(rot[8]),static_cast<float>(rot[9]),static_cast<float>(rot[10]),static_cast<float>(rot[11])};

         
        

            for (size_t i = 0; i < vertex.size() -  6; i += 3)
            {
             
             


                Vec3 start;
                start.x = static_cast<float>(fpos[0] + vertex.at(i));
                start.y = static_cast<float>(fpos[1] + vertex.at(i + 1));
                start.z = static_cast<float>(fpos[2] + vertex.at(i + 2));

                Vec3 end;

                end.x = static_cast<float>(fpos[0] + vertex.at(i + 3));
                end.y = static_cast<float>(fpos[1] + vertex.at(i + 4));
                end.z = static_cast<float>(fpos[2] + vertex.at(i + 5));



                ASC_LINERENDER.AddLine(start, end, color);
            }
        
        
        }
        else
        {

            Vec4 color = Color::GREEN;

            if (debug_collided)
                color = Color::INDIGO;


           const dReal* rot = dGeomGetRotation(geom);
           dVector3 pos;
           dGeomCopyPosition(geom, pos);
        
           float fpos[3] = { static_cast<float>(pos[0]),static_cast<float>(pos[1]),static_cast<float>(pos[2]) };
        
           float frot[12] = { static_cast<float>(rot[0]),static_cast<float>(rot[1]),static_cast<float>(rot[2]),static_cast<float>(rot[3]),
                               static_cast<float>(rot[4]),static_cast<float>(rot[5]),static_cast<float>(rot[6]),static_cast<float>(rot[7]),
                               static_cast<float>(rot[8]),static_cast<float>(rot[9]),static_cast<float>(rot[10]),static_cast<float>(rot[11]) };
        
        
           for (size_t i = 0; i < vertex.size() - 3; i+=3 )
           {
              

               Vec3 start;
               start.x = static_cast<float>(fpos[0] + vertex.at(i));
               start.y = static_cast<float>(fpos[1] + vertex.at(i+1));
               start.z = static_cast<float>(fpos[2] + vertex.at(i+2));
              
               Vec3 end;

               end.x = static_cast<float>(fpos[0] + vertex.at(i + 3));
               end.y = static_cast<float>(fpos[1] + vertex.at(i + 4));
               end.z = static_cast<float>(fpos[2] + vertex.at(i + 5));



               ASC_LINERENDER.AddLine(start, end, color);
           }
            
            
        }

    }
    else
    {
        dVector3 convert_draw_pos;

        dQuaternion q;

        if (have_body)
        {
            dBodyCopyPosition(body, convert_draw_pos);

            dBodyCopyQuaternion(body, q);
        }
        else
        {
            dGeomCopyPosition(geom, convert_draw_pos);

            dGeomGetQuaternion(geom, q);
        }

        Vec3 draw_pos;

        draw_pos.m[0] = static_cast<float>(convert_draw_pos[0]);
        draw_pos.m[1] = static_cast<float>(convert_draw_pos[1]);
        draw_pos.m[2] = static_cast<float>(convert_draw_pos[2]);


        Vec3 draw_dir;

        draw_dir.m[0] = static_cast<float>(q[1]);
        draw_dir.m[1] = static_cast<float>(q[2]);
        draw_dir.m[2] = static_cast<float>(q[3]);


        Vec4 color = nc_color;


        if (debug_collided)
        {
            color = c_color;
        }

        if (geometry.compare("BOX") == 0)
        {
            dVector3 result;
            GeomBoxGetLengths(result);

            Vec3 min;

            min.m[0] = 0.0f - static_cast<float>(result[0] * 0.5);
            min.m[1] = 0.0f - static_cast<float>(result[1] * 0.5);
            min.m[2] = 0.0f - static_cast<float>(result[2] * 0.5);

            Vec3 max;

            max.m[0] = 0.0f + static_cast<float>(result[0] * 0.5);
            max.m[1] = 0.0f + static_cast<float>(result[1] * 0.5);
            max.m[2] = 0.0f + static_cast<float>(result[2] * 0.5);

            ASC_LINERENDER.AddCube(draw_pos, min , max, color);

        }
        else if (geometry.compare("CAPSULE") == 0)
        {
            dReal radius;
            dReal length;
            GeomCapsuleGetParams(&radius, &length);


            Vec3 min;

            min.m[0] = 0.0f - static_cast<float>(radius);
            min.m[1] = 0.0f - static_cast<float>(radius);
            min.m[2] = 0.0f - static_cast<float>(length * 0.5);

            Vec3 max;

            max.m[0] = 0.0f + static_cast<float>(radius);
            max.m[1] = 0.0f + static_cast<float>(radius);
            max.m[2] = 0.0f + static_cast<float>(length * 0.5);

            ASC_LINERENDER.AddCube(draw_pos, min, max, color);

        }
        else if (geometry.compare("CYCLINDER") == 0)
        {
            dReal radius;
            dReal length;
            GeomCylinderGetParams(&radius, &length);

            Vec3 min;

            min.m[0] = 0.0f - static_cast<float>(radius);
            min.m[1] = 0.0f - static_cast<float>(radius);
            min.m[2] = 0.0f - static_cast<float>(length * 0.5);

            Vec3 max;

            max.m[0] = 0.0f + static_cast<float>(radius);
            max.m[1] = 0.0f + static_cast<float>(radius);
            max.m[2] = 0.0f + static_cast<float>(length * 0.5);

            ASC_LINERENDER.AddCube(draw_pos, min, max, color);

        }
        else if (geometry.compare("PLANE") == 0)
        {
            dVector4 info;
            GeomPlaneGetParams(info);

        }
        else if (geometry.compare("RAY") == 0)
        {
            dReal length = GeomRayGetLength();

            dVector3 dstart;
            dVector3 ddir;

            dGeomRayGet(geom, dstart, ddir);

            Vec3 pos;

            pos.m[0] = static_cast<float>(dstart[0] + offset.x);
            pos.m[1] = static_cast<float>(dstart[1] + offset.y) ;
            pos.m[2] = static_cast<float>(dstart[2] + offset.z);

            Vec3 dir;

            dir.m[0] = static_cast<float>(ddir[0]);
            dir.m[1] = static_cast<float>(ddir[1]);
            dir.m[2] = static_cast<float>(ddir[2]);


            ASC_LINERENDER.AddLine(pos, dir * static_cast<float>(length) + pos, color);

        }
        else if (geometry.compare("SPHERE") == 0)
        {
            dReal radius = GeomSphereGetRadius();

            ASC_LINERENDER.AddSphere(draw_pos, static_cast<float>(radius), color);
        }
    }

}
