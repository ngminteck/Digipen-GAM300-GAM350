#include "pch.h"




static void nearCallback(void* data, dGeomID o1, dGeomID o2)
{
    
    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;

    dContact contact[MAX_CONTACTS];

    int num = dCollide(o1, o2, 1, &contact[0].geom, sizeof(dContact));

    for (int i = 0; i < num; ++i)
    {
        ODENode* node1 = static_cast<ODENode*>(dGeomGetData(contact[i].geom.g1));
        ODENode* node2 = static_cast<ODENode*>(dGeomGetData(contact[i].geom.g2));

        if (node1 && node2)
        {
          auto go1 = node1->npc->GetGOHandle(); auto go2 = node2->npc->GetGOHandle();
          if (go1.IsValid() && go2.IsValid())
          {
            if (node1->npc->GetGOHandle()->GetIsEnable() && node2->npc->GetGOHandle()->GetIsEnable())
            {
              if (node1->npc->Enabled && node2->npc->Enabled)
              {
                node1->debug_collided = true;
                node2->debug_collided = true;

                if (node1->hack_type.compare("PHYSICS") == 0 && node2->hack_type.compare("PHYSICS") == 0)
                {

                  if (node1->apply_physics_surface_collision && node2->apply_physics_surface_collision)
                  {
                    for (auto& it1 = node1->ignore_collision_list.begin(); it1 != node1->ignore_collision_list.end(); ++it1)
                    {
                      if (!node2->npc->GetGOHandle().IsValid())
                        continue;

                      if (node2->npc->GetGOHandle()->name.compare(*it1) == 0)
                        continue;
                    }

                    for (auto& it2 = node2->ignore_collision_list.begin(); it2 != node2->ignore_collision_list.end(); ++it2)
                    {
                      if (!node1->npc->GetGOHandle().IsValid())
                        continue;
                      if (node1->npc->GetGOHandle()->name.compare(*it2) == 0)
                        continue;
                    }

                    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
                    contact[i].surface.mu = 0;
                    contact[i].surface.mu2 = 0;
                    contact[i].surface.bounce = 0.0;
                    contact[i].surface.bounce_vel = 0.1;
                    contact[i].surface.soft_cfm = 0.01;



                    dJointID c = dJointCreateContact(ASC_NEWPHYSICSMANAGER.world, ASC_NEWPHYSICSMANAGER.contactgroup, contact + i);


                    dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));

                   // if (dGeomGetBody(contact[i].geom.g1))
                   // {
                   //     std::cout << node1->npc->GetGOHandle()->name << " status:" << dBodyIsEnabled(dGeomGetBody(contact[i].geom.g1)) << std::endl;
                   //
                   //
                   // }
                   // if (dGeomGetBody(contact[i].geom.g2))
                   // {
                   //     std::cout << node2->npc->GetGOHandle()->name << " status:" << dBodyIsEnabled(dGeomGetBody(contact[i].geom.g2)) << std::endl;
                   //
                   //
                   // }

                    if (node1->have_body)
                    {
                        if (!node1->legit_moving)
                        {
                            const dReal * hacky= dBodyGetAngularVel(node1->body);
                            dBodySetLinearVel(node1->body, 0.0, hacky[1], 0.0);
                        }
                    
                    }
                    
                    if (node2->have_body)
                    {
                        if (!node2->legit_moving)
                        {
                            const dReal* hacky = dBodyGetAngularVel(node2->body);
                            dBodySetLinearVel(node2->body, 0.0, hacky[1], 0.0);
                        }
                    }

                  }
                }
              }
            }
          }
        }

    }

}

NewPhysicsManager::NewPhysicsManager(HandleManager<TransformComp>& transform_comps)
	:m_transform(transform_comps), initODE{ dInitODE2(0) }, world{ dWorldCreate() }, space{ dHashSpaceCreate(0) }, contactgroup{ dJointGroupCreate(0)}, world_space_is_alive{true}, init_le{false}
{
	ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &NewPhysicsManager::HandleMsg, this);
	ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &NewPhysicsManager::HandleMsg, this);
	ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_PAUSE, &NewPhysicsManager::HandleMsg, this);
	ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_RESUME, &NewPhysicsManager::HandleMsg, this);

  //Force reg stuff 
  ASC_MSGSYS.RegisterCallBack(mTYPE_BY_GO_HANDLE, &NewPhysicsManager::HandleForceReg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_BY_GO_TYPE, &NewPhysicsManager::HandleForceReg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_BY_GO_NAME, &NewPhysicsManager::HandleForceReg, this);

  ASC_MSGSYS.RegisterCallBack(mTYPE_BY_GO_TYPE_IGNORERAY, &NewPhysicsManager::HandleForceRegIgnoreList, this);

	// create world

  WorldSetting();

  dSpaceSetCleanup(space, 1);

    ccd_list.clear();
  

}

NewPhysicsManager::~NewPhysicsManager()
{
    ccd_list.clear();
    world_space_is_alive = false;
    init_le = false;
	dJointGroupEmpty(contactgroup);
	dJointGroupDestroy(contactgroup);
	dSpaceDestroy(space);
	dWorldDestroy(world);
	dCloseODE();
}

void NewPhysicsManager::Load()
{

   std::cout << "NewPhysicsManager Load() called" << std::endl;
   //
   InitODE();
 

}

void NewPhysicsManager::UnLoad()
{

    std::cout << "NewPhysicsManager UnLoad() called" << std::endl;
   //
    if (init_le)
    {
        DestroyODE();
    }

}

void NewPhysicsManager::Update(float dt)
{

	if (isEnabled)
	{

      //  dt *= fps_limit / dt;
        //fps_remainder += dt;
        //
        //if (fps_remainder >= fps_limit)
        //{

          UpdateTransformToPhysics();
          UpdateCCD();
          dSpaceCollide(space, 0, &nearCallback);
          dWorldQuickStep(world, dt);
          dJointGroupEmpty(contactgroup);
          UpdatePhysicsToTransform();

      //      fps_remainder -= fps_limit;
      //
      //      CanUpdateInput = true;
      //  }
      //  else
      //  {
      //      CanUpdateInput = false;
      //  }
	

	}
    else
    {
#ifdef _RELEASE 


        if (!ASC_SYSTEM.hack_releasemode_submission)
        {
            for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
            {
                NewPhyHandle hdl = it->GetHdl<NewPhysicsComp>();
                if (it->GetIsEnable())
                {
                    if (hdl.IsValid() && hdl->Enabled)
                    {
                        TransformComp& transform = *m_transform[hdl->GetTransHdl()];

                        if (hdl->obj_phy)
                        {
                            if (hdl->obj_phy->have_body)
                            {

                                hdl->obj_phy->BodySetQuaternion(transform.rotation);
                                Vec3 phy_pos = transform.pos + hdl->obj_phy->collision_pos_offset;
                                hdl->obj_phy->BodySetPosition(phy_pos);

                                hdl->obj_phy->GeomDraw(Color::RED, Color::HOT_PINK);

                            }
                            else
                            {
                                hdl->obj_phy->GeomSetQuaternion(transform.rotation);
                                Vec3 phy_pos = transform.pos + hdl->obj_phy->collision_pos_offset;
                                hdl->obj_phy->GeomSetPosition(phy_pos);

                                hdl->obj_phy->GeomDraw(Color::BLACK, Color::GREY);
                            }

                            if (hdl->obj_phy->have_triggerbox)
                            {
                                hdl->obj_trigger->GeomSetQuaternion(transform.rotation);
                                Vec3 phy_pos = transform.pos + hdl->obj_trigger->collision_pos_offset;
                                hdl->obj_trigger->GeomSetPosition(phy_pos);

                                hdl->obj_trigger->GeomDraw(Color::ORANGE, Color::ORANGE_CORAL);
                            }

                            if (hdl->obj_phy->hack_always_able_raycast)
                            {
                                hdl->obj_ray->GeomDraw(Color::YELLOW, Color::WHITE);
                                hdl->obj_floor_ray->GeomDraw(Color::YELLOW, Color::WHITE);
                            }
                        }

                    }
                }
            }
        }
#endif
    }
	
}

void NewPhysicsManager::UpdateTransformToPhysics()
{
    // update position from transform
    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
        NewPhyHandle hdl = it->GetHdl<NewPhysicsComp>();

        if (force_update_once)
        {
            if (hdl.IsValid() && hdl->obj_phy)
            {
                TransformComp& transform = *m_transform[hdl->GetTransHdl()];

                // update obj phy


                Vec3 offset_with_rot = transform.rotation * hdl->obj_phy->collision_pos_offset;

                Vec3 phy_pos = transform.pos + offset_with_rot;

                if (hdl->obj_phy->have_body)
                {

                    hdl->obj_phy->BodySetQuaternion(transform.rotation);


                    hdl->obj_phy->BodySetPosition(phy_pos);
                }
                else
                {
                    hdl->obj_phy->GeomSetQuaternion(transform.rotation);


                    hdl->obj_phy->GeomSetPosition(phy_pos);
                }

                // update obj trigger
                if (hdl->obj_phy->have_triggerbox)
                {
                    hdl->obj_trigger->GeomSetQuaternion(transform.rotation);
                    Vec3 trigger_pos = transform.pos + (transform.rotation * hdl->obj_trigger->collision_pos_offset);
                    hdl->obj_trigger->GeomSetPosition(trigger_pos);
                }

                if (hdl->obj_phy->hack_always_able_raycast)
                {
                    //Vec3 ray_pos = transform.pos + hdl->obj_ray->collision_pos_offset;
                    //hdl->obj_ray->GeomSetPosition(ray_pos);

                    Vec3 rayfloor_pos = transform.pos + (transform.rotation * hdl->obj_floor_ray->collision_pos_offset);
                    hdl->obj_floor_ray->GeomSetPosition(rayfloor_pos);
                }

            }
            force_update_once = false;
        }
        else
        {

            if (it->GetIsEnable())
            {

                if (hdl.IsValid() /* && hdl->Enabled*/)
                {
                    if (it->name.compare("DogPlayer") == 0)
                        hdl->obj_phy->SetRigidBodyEnable(true);

                    if (hdl->obj_phy)
                    {



                        if (hdl->obj_phy->have_body)
                        {
                            TransformComp& transform = *m_transform[hdl->GetTransHdl()];

                            // update obj phy
                            hdl->obj_phy->BodySetQuaternion(transform.rotation);

                            Vec3 offset_with_rot = transform.rotation * hdl->obj_phy->collision_pos_offset;

                            Vec3 phy_pos = transform.pos + offset_with_rot;

                            hdl->obj_phy->BodySetPosition(phy_pos);



                            // update obj trigger
                            if (hdl->obj_phy->have_triggerbox)
                            {
                                hdl->obj_trigger->GeomSetQuaternion(transform.rotation);
                                Vec3 trigger_pos = transform.pos + (transform.rotation * hdl->obj_trigger->collision_pos_offset);
                                hdl->obj_trigger->GeomSetPosition(trigger_pos);
                            }

                            if (hdl->obj_phy->hack_always_able_raycast)
                            {
                                //Vec3 ray_pos = transform.pos + hdl->obj_ray->collision_pos_offset;
                                //hdl->obj_ray->GeomSetPosition(ray_pos);

                                Vec3 rayfloor_pos = transform.pos + (transform.rotation * hdl->obj_floor_ray->collision_pos_offset);
                                hdl->obj_floor_ray->GeomSetPosition(rayfloor_pos);
                            }

                        }


                        hdl->obj_phy->debug_collided = false;
                        hdl->obj_trigger->debug_collided = false;
                        hdl->obj_ray->debug_collided = false;

                        hdl->obj_floor_ray->debug_collided = false;


                    }



                }
            }
        }
    }
}

void NewPhysicsManager::UpdateCCD()
{

    for (auto& it = ccd_list.begin(); it != ccd_list.end(); ++it)
    {
        auto& info = *(it->second);

        // to get new physic comp
        // it->first->npc->Enable

        //f (it->first->Enabled)
        //{

        if (it->first)
        {

            auto obj1 = it->first->npc;
            auto test2 = it->first->npc->GetGOHandle()->name;

            //gameobject must be enable and the trigger must be enable too 
            if (obj1->Enabled && obj1->obj_trigger->GeomCollisionIsEnabled())
            {

                for (auto& it2 = info.begin(); it2 != info.end(); ++it2)
                {
                    // if (!it2->first->Enabled) continue;
                    auto obj2 = it2->first->npc;
                    auto test = it->first->npc->GetGOHandle()->name;
                    if (obj2->Enabled && obj2->obj_trigger->GeomCollisionIsEnabled())
                    {
                        auto test = obj2->GetGOHandle()->name;
                        const int N = 32;
                        dContact contact[N];
                        int n = dCollide(it->first->npc->obj_trigger->geom, it2->first->npc->obj_trigger->geom, N, &(contact[0].geom), sizeof(dContact));

                        if (n > 0)
                        {
                            if (it2->second == mTYPE_LOGIC_ONCOLLIDE_EXIT)
                            {
                                if (it->first->npc->GetGOHandle().IsValid() && it2->first->npc->GetGOHandle().IsValid())
                                {
                                    it2->second = mTYPE_LOGIC_ONCOLLIDE_ENTER;
                                    ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_ENTER, it->first->npc->GetGOHandle(), it2->first->npc->GetGOHandle()));
                                }
                            }
                            else if (it2->second == mTYPE_LOGIC_ONCOLLIDE_ENTER)
                            {
                                if (it->first->npc->GetGOHandle().IsValid() && it2->first->npc->GetGOHandle().IsValid())
                                {
                                    it2->second = mTYPE_LOGIC_ONCOLLIDE_STAY;
                                    ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_STAY, it->first->npc->GetGOHandle(), it2->first->npc->GetGOHandle()));
                                }
                            }
                        }
                        else
                        {
                            if (it2->second == mTYPE_LOGIC_ONCOLLIDE_STAY || it2->second == mTYPE_LOGIC_ONCOLLIDE_ENTER)
                            {
                                if (it->first->npc->GetGOHandle().IsValid() && it2->first->npc->GetGOHandle().IsValid())
                                {
                                    it2->second = mTYPE_LOGIC_ONCOLLIDE_EXIT;
                                    ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_LOGIC_ONCOLLIDE_EXIT, it->first->npc->GetGOHandle(), it2->first->npc->GetGOHandle()));
                                }
                            }
                        }
                    }
                }
            }
        }

    }
}

void NewPhysicsManager::UpdatePhysicsToTransform()
{
    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
        NewPhyHandle hdl = it->GetHdl<NewPhysicsComp>();
        if (it->GetIsEnable())
        {
          if (hdl.IsValid()/* && hdl->Enabled*/)
          {
              if (hdl->obj_phy)
              {
                  // only need pass the phy body info back to transform
                  if (hdl->obj_phy->have_body)
                  {
                      // hdl->obj_phy->BodySetDampingDefaults();

                      hdl->obj_phy->BodySetAngularVel();

                      if (!hdl->obj_phy->legit_moving)
                      {
                          const dReal* hacky = dBodyGetAngularVel(hdl->obj_phy->body);
                          dBodySetLinearVel(hdl->obj_phy->body, 0.0, hacky[1], 0.0);
                      }




                      TransformComp& transform = *m_transform[hdl->GetTransHdl()];

                      dQuaternion phy_quat;
                      dBodyCopyQuaternion(hdl->obj_phy->body, phy_quat);

                      Quat new_rot;

                      new_rot.m[0] = static_cast<float>(phy_quat[0]);
                      new_rot.m[1] = static_cast<float>(phy_quat[1]);
                      new_rot.m[2] = static_cast<float>(phy_quat[2]);
                      new_rot.m[3] = static_cast<float>(phy_quat[3]);

                      //transform.rotation = new_rot;


                      dVector3 phy_pos;

                      dBodyCopyPosition(hdl->obj_phy->body, phy_pos);

                      Vec3 new_pos;

                      Vec3 collision_with_rot_offset = transform.rotation * hdl->obj_phy->collision_pos_offset;

                      if (!hdl->obj_phy->legit_moving)
                      {
                          new_pos.m[0] = transform.pos.x + collision_with_rot_offset.x;
                          new_pos.m[1] = static_cast<float>(phy_pos[1]);
                          new_pos.m[2] = transform.pos.z + collision_with_rot_offset.z;

                          new_pos = new_pos - (transform.rotation * hdl->obj_phy->collision_pos_offset);
                      }
                      else
                      {
                          new_pos.m[0] = static_cast<float>(phy_pos[0]);
                          new_pos.m[1] = static_cast<float>(phy_pos[1]);
                          new_pos.m[2] = static_cast<float>(phy_pos[2]);
                          new_pos = new_pos -(transform.rotation* hdl->obj_phy->collision_pos_offset);
                      }
                    

                      transform.pos = new_pos;

                      transform.dirty = true;

//#ifdef _RELEASE 
                      if (!ASC_SYSTEM.hack_releasemode_submission)
                      {

                          hdl->obj_phy->GeomDraw(Color::RED, Color::HOT_PINK);
                      }
//#endif
                  }
//#ifdef _RELEASE 

                  else
                  {
                      if (!ASC_SYSTEM.hack_releasemode_submission)
                      {
                          hdl->obj_phy->GeomDraw(Color::BLACK, Color::GREY);
                      }
                  }

                  if (!ASC_SYSTEM.hack_releasemode_submission)
                  {

                      if (hdl->obj_phy->have_triggerbox)
                          hdl->obj_trigger->GeomDraw(Color::ORANGE, Color::ORANGE_CORAL);

                      if (hdl->obj_phy->hack_always_able_raycast)
                      {
                          hdl->obj_ray->GeomDraw(Color::YELLOW, Color::WHITE);
                          hdl->obj_floor_ray->GeomDraw(Color::YELLOW, Color::WHITE);
                      }
                  }
//#endif
              }

          }
        }
    }
}


void NewPhysicsManager::DeserShape(NewPhysicsComp& comp, Deserializer& deserializer)
{
    auto th = comp.GetTransHdl();

  if (comp.obj_phy->geometry.compare("BOX") == 0)
  {
    
    Vector3 vec3{0,0,0 };
    deserializer.Read("GeomBoxGetLengths", vec3);

    comp.obj_phy->box_length[0] = static_cast<dReal>(vec3.m[0]);
    comp.obj_phy->box_length[1] = static_cast<dReal>(vec3.m[1]);
    comp.obj_phy->box_length[2] = static_cast<dReal>(vec3.m[2]);

    comp.obj_phy->GeomBoxSetLengths(comp.obj_phy->box_length[0] * th->scale.m[0], comp.obj_phy->box_length[1] * th->scale.m[1], comp.obj_phy->box_length[2] * th->scale.m[2]);
  }
  else if (comp.obj_phy->geometry.compare("CAPSULE") == 0)
  {
    comp.obj_phy->CreateGeometry();
    float len_ = 0;
    float rad_ = 0;

    deserializer.Read("Radius", rad_);
    deserializer.Read("Length", len_);

    comp.obj_phy->length = len_;
    comp.obj_phy->radius = rad_;

    dReal scalar = th->scale.m[0];

    if (scalar < th->scale.m[1])
        scalar = th->scale.m[1];

    comp.obj_phy->GeomCapsuleSetParams(comp.obj_phy->radius * scalar, comp.obj_phy->length * th->scale.m[2]);

  }
  else if (comp.obj_phy->geometry.compare("CYCLINDER") == 0)
  {
      comp.obj_phy->CreateGeometry();
    float radF = 0;
    float lenF = 0; 
    deserializer.Read("Radius", radF);
    deserializer.Read("Length", lenF);

    comp.obj_phy->length = lenF;
    comp.obj_phy->radius = radF;

    dReal scalar = th->scale.m[0];

    if (scalar < th->scale.m[1])
        scalar = th->scale.m[1];

    comp.obj_phy->GeomCylinderSetParams(comp.obj_phy->radius * scalar, comp.obj_phy->length * th->scale.m[2]);
  }
  else if (comp.obj_phy->geometry.compare("PLANE") == 0)
  {
      comp.obj_phy->CreateGeometry();
    Vec4 vec4{ 0,0,0,0 };
    deserializer.Read("plane", vec4);
 

    dVector4 plane{ vec4[0],vec4[1],vec4[2],vec4[3] };
    comp.obj_phy->GeomPlaneSetParams(plane[0],plane[1],plane[2],plane[3]);

  }
  else if (comp.obj_phy->geometry.compare("RAY") == 0)
  {
      comp.obj_phy->CreateGeometry();
    float realLen = 0;
    deserializer.Read("len", realLen);
    comp.obj_phy->length = realLen;
    comp.obj_phy->GeomRaySetLength(comp.obj_phy->length * th->scale.m[2]);
  }
  else if (comp.obj_phy->geometry.compare("TRIMESH") == 0)
  {
      
      TransformComp& tc = *m_transform[comp.GetSelfHandle()->GetTransHdl()];

      comp.obj_phy->vertex.clear();
      comp.obj_phy->indices.clear();

      deserializer.Read("vertex", comp.obj_phy->vertex);
      deserializer.Read("indices", comp.obj_phy->indices);

     /* std::cout << comp.obj_phy->vertex.size() << std::endl;

      std::cout << comp.obj_phy->indices.size() << std::endl;*/

      comp.obj_phy->CreateTrimesh();

  }
  else if (comp.obj_phy->geometry.compare("SPHERE") == 0)
  {
      comp.obj_phy->CreateGeometry();

    float realRad = 0;
    deserializer.Read("rad", realRad);
   comp.obj_phy->radius = realRad;

    dReal scalar = th->scale.m[0];

    if (scalar < th->scale.m[1])
        scalar = th->scale.m[1];

    if (scalar < th->scale.m[2])
        scalar = th->scale.m[2];

    comp.obj_phy->GeomSphereSetRadius(comp.obj_phy->radius * scalar);
  }
  //  Planes are non-placeable geoms. This means that, unlike placeable geoms, planes do not have an assigned position and rotation

  if (!comp.obj_phy->geometry.compare("PLANE") == 0)
  {
    Vec3 vec3;
    deserializer.Read("GeometryOffsetPosition", vec3);
    deserializer.Read("HaveBody", comp.obj_phy->have_body);

    for( auto i = 0; i < 3 ; i++)
      comp.obj_phy->collision_pos_offset.m[i] = vec3[i];


  }
  // needed to calulate mass 
  if (comp.obj_phy->geometry.compare("CAPSULE") == 0 || comp.obj_phy->geometry.compare("CYCLINDER") == 0)
  {

    deserializer.Read("directionAxis", comp.obj_phy->direction_axis);
  }

}
void NewPhysicsManager::DeserBody(NewPhysicsComp& comp, Deserializer& deserializer)
{
 
  bool result = false;
  deserializer.Read("RbEnable", result );
  comp.obj_phy->SetRigidBodyEnable(result);

  // this is a vector3
  Vec3 vel; vel.x = 0; vel.y = 0; vel.z = 0;
  deserializer.Read("Velocity", vel);
  dVector3 dVec{ vel.x, vel.y , vel.z }; 
  dBodySetLinearVel(comp.obj_phy->body, dVec[0], dVec[1], dVec[2]);

  //this is also a vector3
  Vec3 avel{ 0,0,0 };

  deserializer.Read("AngularVelocity", avel);
  dVector3  AdVec{ avel.x,avel.y,avel.z }; 
  dBodySetAngularVel(comp.obj_phy->body, AdVec[0],AdVec[1],AdVec[2] );

  deserializer.Read("GravityEnable:", result); 
  comp.obj_phy->BodySetGravityEnable(result);
  deserializer.Read("prevGrav", comp.prevsBoolForGrav);
  float m = 0;
  deserializer.Read("Mass", m); 
  comp.obj_phy->MassAdjust(m);
}
void NewPhysicsManager::DeserPhysicsSurface(NewPhysicsComp& comp, Deserializer& deserializer)
{

  //if (comp.obj_phy->IsApplyPhysicsSurfaceEnable())
  //{
  //
  //
  //  deserializer.Read("ContactMode", comp.obj_phy->surface_mode_list);
  //
  //  deserializer.Read("Friction1", comp.obj_phy->contact.surface.mu);
  //  deserializer.Read("Friction2", comp.obj_phy->contact.surface.mu2);
  //  deserializer.Read("Densities1", comp.obj_phy->contact.surface.rho);
  //  deserializer.Read("Densities2", comp.obj_phy->contact.surface.rho2);
  //  deserializer.Read("DensitiesN", comp.obj_phy->contact.surface.rhoN);
  //  deserializer.Read("Bounce", comp.obj_phy->contact.surface.bounce);
  //  deserializer.Read("BounceVel", comp.obj_phy->contact.surface.bounce_vel);
  //  deserializer.Read("ErrorReductionParameter", comp.obj_phy->contact.surface.soft_erp);
  //  deserializer.Read("ConstraintForce", comp.obj_phy->contact.surface.soft_cfm);
  //  deserializer.Read("Motion1", comp.obj_phy->contact.surface.motion1);
  //  deserializer.Read("Motion2", comp.obj_phy->contact.surface.motion2);
  //  deserializer.Read("ForceDependentSlip1", comp.obj_phy->contact.surface.slip1);
  //  deserializer.Read("ForceDependentSlip2", comp.obj_phy->contact.surface.slip2);
  //
  //}
}

void NewPhysicsManager::DeserCCD(NewPhysicsComp& comp, Deserializer& deserializer)
{
    deserializer.Read("CCD_Enable", comp.obj_phy->continue_collision_detection);

   // SetCCDEnable(&comp, comp.continue_collision_detection);

    if (comp.obj_phy->continue_collision_detection)
    {
       deserializer.Read("CCD_ListObjects", comp.obj_phy->continue_collision_detection_list_string);
     
       // ASC_FACTORY.GetGameObjectByName()

    }
}
void NewPhysicsManager::DerIgnore(NewPhysicsComp& comp, Deserializer& deserializer)
{
  deserializer.Read("IgnoreList", comp.obj_phy->ignore_collision_list);

}

BaseHandle NewPhysicsManager::Deserialize(const GameObject& gameObject, Deserializer& deserializer)
{
  NewPhyHandle hdl(CreateComponent());
  NewPhysicsComp& physicsComp = *GetComponent(hdl);
  physicsComp.SetGOHandle(gameObject.GetSelfHandle());
  physicsComp.SetTransHdl(Handle<TransformComp>(gameObject[TRANSFORM]));

  auto th = physicsComp.GetTransHdl();

  deserializer.Read("IsEnable", physicsComp.Enabled);

  //what shaped it it

  deserializer.Read("Geometry", physicsComp.obj_phy->geometry);

  DeserShape(physicsComp, deserializer);

  bool result = false;
  deserializer.Read("CollisionEnable", result);
  physicsComp.obj_phy->GeomCollisionSetEnable(result);
  deserializer.Read("PhysicsSurfaceEnable", result);
  physicsComp.obj_phy->SetApplyPhysicsSurfaceEnable(result);

  if (physicsComp.obj_phy->IsApplyPhysicsSurfaceEnable())
  {
    DeserPhysicsSurface(physicsComp, deserializer);
  }

  //if have body
  if (physicsComp.obj_phy->have_body)
  {
      DeserBody(physicsComp, deserializer);
  }
    

  

  physicsComp.obj_phy->mass.c[0] = static_cast<dReal>(physicsComp.obj_phy->collision_pos_offset.m[0]);
  physicsComp.obj_phy->mass.c[1] = static_cast<dReal>(physicsComp.obj_phy->collision_pos_offset.m[1]);
  physicsComp.obj_phy->mass.c[2] = static_cast<dReal>(physicsComp.obj_phy->collision_pos_offset.m[2]);

  DeserCCD(physicsComp, deserializer);
  DerIgnore(physicsComp, deserializer);

  //TransHandle th = physicsComp.GetTransHdl();

  Vec3 offset_with_rot = th->rotation * physicsComp.obj_phy->collision_pos_offset;

  Vec3 pos_with_collision_offset = th->pos + offset_with_rot;

  if (physicsComp.obj_phy->have_body)
  {
      physicsComp.obj_phy->BodySetQuaternion(th->rotation);

      physicsComp.obj_phy->BodySetPosition(pos_with_collision_offset);
  }
  else
  {
      physicsComp.obj_phy->GeomSetQuaternion(th->rotation);

      physicsComp.obj_phy->GeomSetPosition(pos_with_collision_offset);
  }

  // trigger_box

  deserializer.Read("HaveTriggerBox", physicsComp.obj_phy->have_triggerbox);

  if (physicsComp.obj_phy->have_triggerbox)
  {
      Vector3 vec3t{ 0,0,0 };
      deserializer.Read("TriggerGeomBoxGetLengths", vec3t);

      physicsComp.obj_trigger->box_length[0] = static_cast<dReal>(vec3t.m[0]);
      physicsComp.obj_trigger->box_length[1] = static_cast<dReal>(vec3t.m[1]);
      physicsComp.obj_trigger->box_length[2] = static_cast<dReal>(vec3t.m[2]);

      physicsComp.obj_trigger->GeomBoxSetLengths(physicsComp.obj_trigger->box_length[0] * th->scale.m[0], physicsComp.obj_trigger->box_length[1] * th->scale.m[1], physicsComp.obj_trigger->box_length[2] * th->scale.m[2]);

   
      deserializer.Read("TriggerGeometryOffsetPosition", physicsComp.obj_trigger->collision_pos_offset);

      bool trigger_enable;
      deserializer.Read("TriggerCollisionEnable", trigger_enable);

      physicsComp.obj_trigger->GeomCollisionSetEnable(trigger_enable);


      deserializer.Read("IgnoreListTrigger", physicsComp.obj_trigger->ignore_collision_list);

      Vec3 offset_with_rots = th->rotation * physicsComp.obj_phy->collision_pos_offset;

      Vec3 pos_with_collision_offsets = th->pos + offset_with_rots;

      physicsComp.obj_trigger->GeomSetQuaternion(th->rotation);

      physicsComp.obj_trigger->GeomSetPosition(pos_with_collision_offsets);

  }

  // ray skip

  deserializer.Read("IgnoreListRay", physicsComp.obj_ray->ignore_collision_list);
  deserializer.Read("IgnoreListFloorRay", physicsComp.obj_floor_ray->ignore_collision_list);

 

	return hdl;

}

void NewPhysicsManager::SerShape(Serializer& serializer, BaseHandle handle)
{
  NewPhysicsComp& comp = *GetComponent(handle);
  if (comp.obj_phy->geometry.compare("BOX") == 0)
  {
      Vec3 save_box_length = { static_cast<float>(comp.obj_phy->box_length[0]),static_cast<float>(comp.obj_phy->box_length[1]), static_cast<float>(comp.obj_phy->box_length[2]) };
    
    serializer.Write("GeomBoxGetLengths", save_box_length);
  }
  else if (comp.obj_phy->geometry.compare("CAPSULE") == 0)
  {

    serializer.Write("Radius", comp.obj_phy->radius);
    serializer.Write("Length", comp.obj_phy->length);

  }
  else if (comp.obj_phy->geometry.compare("CYCLINDER") == 0)
  {

    serializer.Write("Radius", comp.obj_phy->radius);
    serializer.Write("Length", comp.obj_phy->length);

  }
  else if (comp.obj_phy->geometry.compare("PLANE") == 0)
  {

    dVector4 plane;

    comp.obj_phy->GeomPlaneGetParams(plane);

    Vector4 vec4{ static_cast<float>(plane[0]),static_cast<float>(plane[1]),static_cast<float>(plane[2]),static_cast<float>(plane[3]) };
    serializer.Write("plane", plane);

  }
  else if (comp.obj_phy->geometry.compare("RAY") == 0)
  {
    serializer.Write("len", comp.obj_phy->length);
  }
  else if (comp.obj_phy->geometry.compare("TRIMESH") == 0)
  {
      serializer.Write("vertex", comp.obj_phy->vertex);

      serializer.Write("indices", comp.obj_phy->indices);
  }
  else if (comp.obj_phy->geometry.compare("SPHERE") == 0)
  {
;
    serializer.Write("rad", comp.obj_phy->radius);
  }
  //  Planes are non-placeable geoms. This means that, unlike placeable geoms, planes do not have an assigned position and rotation

  if (!comp.obj_phy->geometry.compare("PLANE") == 0)
  {
    comp.obj_phy->collision_pos_offset;

    serializer.Write("GeometryOffsetPosition", comp.obj_phy->collision_pos_offset);
    serializer.Write("HaveBody", comp.obj_phy->have_body);

  }
  // needed to calulate mass 
  if (comp.obj_phy->geometry.compare("CAPSULE") == 0 || comp.obj_phy->geometry.compare("CYCLINDER") == 0)
  {
 
    serializer.Write("directionAxis",comp.obj_phy->direction_axis );
  }
 
}

void NewPhysicsManager::SerBody(Serializer& serializer, BaseHandle handle)
{
  NewPhysicsComp& comp = *GetComponent(handle);

  serializer.Write("RbEnable", comp.obj_phy->IsRigidBodyEnable());


  // this is a vector3
  const dReal* curr_vel = dBodyGetLinearVel(comp.obj_phy->body);
  dVector3 vel;
  vel[0] = curr_vel[0];
  vel[1] = curr_vel[1];
  vel[2] = curr_vel[2];
  serializer.Write("Velocity", vel);
  //this is also a vector3
  const dReal* curr_avel = dBodyGetAngularVel(comp.obj_phy->body);
  dVector3 avel;
  avel[0] = curr_avel[0];
  avel[1] = curr_avel[1];
  avel[2] = curr_avel[2];
  serializer.Write("AngularVelocity", avel);

  serializer.Write("GravityEnable:", comp.obj_phy->BodyGetGravityMode());
  serializer.Write("prevGrav", comp.prevsBoolForGrav);

 
  dReal mass = comp.obj_phy->mass.mass;
  serializer.Write("Mass", mass);

}
void NewPhysicsManager::SerPhysicsSurface(Serializer& serializer, BaseHandle handle)
{
  NewPhysicsComp& comp = *GetComponent(handle);


  if (comp.obj_phy->IsApplyPhysicsSurfaceEnable())
  {

   // serializer.Write("ContactMode",   comp.obj_phy->surface_mode_list);
   //
   // serializer.Write("Friction1"              ,comp.obj_phy->contact.surface.mu);
   // serializer.Write("Friction2"              ,comp.obj_phy->contact.surface.mu2);
   // serializer.Write("Densities1"             ,comp.obj_phy->contact.surface.rho);
   // serializer.Write("Densities2"             ,comp.obj_phy->contact.surface.rho2);
   // serializer.Write("DensitiesN"             ,comp.obj_phy->contact.surface.rhoN);
   // serializer.Write("Bounce"                 ,comp.obj_phy->contact.surface.bounce);
   // serializer.Write("BounceVel"              ,comp.obj_phy->contact.surface.bounce_vel);
   // serializer.Write("ErrorReductionParameter",comp.obj_phy->contact.surface.soft_erp);
   // serializer.Write("ConstraintForce"        ,comp.obj_phy->contact.surface.soft_cfm);
   // serializer.Write("Motion1"                ,comp.obj_phy->contact.surface.motion1);
   // serializer.Write("Motion2"                ,comp.obj_phy->contact.surface.motion2);
   // serializer.Write("ForceDependentSlip1"    ,comp.obj_phy->contact.surface.slip1);
   // serializer.Write("ForceDependentSlip2"    ,comp.obj_phy->contact.surface.slip2);

  }

}
void NewPhysicsManager::SerCCD(Serializer& serializer, BaseHandle handle)
{
    NewPhysicsComp& comp = *GetComponent(handle);
    serializer.Write("CCD_Enable", comp.obj_phy->continue_collision_detection);
  
    serializer.Write("CCD_ListObjects",comp.obj_phy->continue_collision_detection_list_string);


}
void NewPhysicsManager::SerIgnore(Serializer& serializer, BaseHandle handle)
{
  NewPhysicsComp& comp = *GetComponent(handle);

  serializer.Write("IgnoreList", comp.obj_phy->ignore_collision_list);
}

void NewPhysicsManager::Serialize(Serializer& serializer, BaseHandle handle )
{
 
  NewPhysicsComp& comp = *GetComponent(handle);

  serializer.Write("IsEnable", comp.Enabled);

  //what shaped it it
  serializer.Write("Geometry",  comp.obj_phy->geometry);
  //deal with shape only stuff first 
  SerShape(serializer, handle);

  serializer.Write("CollisionEnable", comp.obj_phy->GeomCollisionIsEnabled());
  serializer.Write("PhysicsSurfaceEnable", comp.obj_phy->IsApplyPhysicsSurfaceEnable());
  if( comp.obj_phy->IsApplyPhysicsSurfaceEnable())
  {
    SerPhysicsSurface(serializer, handle);
  }
 
  //if have body
  if (comp.obj_phy->have_body)
  {
    SerBody(serializer, handle);
  }
  SerCCD(serializer, handle);
  SerIgnore(serializer, handle);
 
  // ser trigger

  serializer.Write("HaveTriggerBox", comp.obj_phy->have_triggerbox);

  if (comp.obj_phy->have_triggerbox)
  {
      Vec3 save_box_lengtht = { static_cast<float>(comp.obj_trigger->box_length[0]),static_cast<float>(comp.obj_trigger->box_length[1]), static_cast<float>(comp.obj_trigger->box_length[2]) };

      serializer.Write("TriggerGeomBoxGetLengths", save_box_lengtht);

      serializer.Write("TriggerGeometryOffsetPosition", comp.obj_trigger->collision_pos_offset);

      serializer.Write("TriggerCollisionEnable", comp.obj_trigger->GeomCollisionIsEnabled());

      serializer.Write("IgnoreListTrigger", comp.obj_trigger->ignore_collision_list);
  }


  // skip ray
  serializer.Write("IgnoreListRay", comp.obj_trigger->ignore_collision_list);
  serializer.Write("IgnoreListFloorRay", comp.obj_trigger->ignore_collision_list);

}

void NewPhysicsManager::Reflect(BaseHandle hdl, const Reflector& reflector)
{
	NewPhysicsComp& comp = *m_components[NewPhyHandle(hdl)];
    auto th = comp.GetTransHdl();

    GOHandle obj_handle = comp.GetGOHandle();
    reflector.Reflect("IsEnable", comp.Enabled);
  ImGui::BeginChild("ShapeStuff", ImVec2(0, ImGui::GetWindowSize().y *0.3f), true);
  {
    ImGui::TextColored({ 0,10,255, 1 }, "Geometry Information");
    ImGui::Separator();
    ImGui::BeginChild("ShapeStuffChild");

    ImGui::Columns(2);
    ImGui::TextColored({ 0,10,255, 1 }, "Geometry:");
    ImGui::NextColumn();

    int geometrytypeindex = -1;
    std::vector<const char*> geometrytypelist{ "SPHERE", "BOX", "CAPSULE", "CYCLINDER","PLANE","RAY", "TRIMESH"};

    uint32_t counter = 0;
    for (const auto& str : geometrytypelist)
    {
      std::string tostring = str;
      if (comp.obj_phy->geometry.compare(tostring) == 0)
      {
        geometrytypeindex = counter;
        break;
      }
      ++counter;
    }

    if (ImGui::Combo("##geometrytypelistchoice", &geometrytypeindex, geometrytypelist.data(), (uint32_t)geometrytypelist.size()))
    {
      comp.obj_phy->geometry = geometrytypelist[geometrytypeindex];

      if (comp.obj_phy->geometry.compare("TRIMESH") == 0)
      {
          GrapHandle gh = obj_handle->GetHdl<GraphicsComp>();

          if (gh.IsValid())
          {
              NewPhyHandle nph = obj_handle->GetHdl<NewPhysicsComp>();
              TransformComp& tc = *m_transform[nph->GetTransHdl()];

              comp.obj_phy->collision_pos_offset.m[0] = 0.0f;
              comp.obj_phy->collision_pos_offset.m[1] = 0.0f;
              comp.obj_phy->collision_pos_offset.m[2] = 0.0f;

              GenerateTriMesh(comp.obj_phy, &tc, gh->modelfile);
          }
          else
          {
              comp.obj_phy->geometry = "SPHERE";
              comp.obj_phy->CreateGeometry();
          }
      }

      else if (comp.obj_phy->geometry.compare("PLANE") == 0)
      {
        
        comp.obj_phy->SetHaveBody(false);
        
        comp.obj_phy->collision_pos_offset.m[0] = 0.0f;
        comp.obj_phy->collision_pos_offset.m[1] = 0.0f;
        comp.obj_phy->collision_pos_offset.m[2] = 0.0f;



        comp.obj_phy->CreateGeometry();
      }
      else if (comp.obj_phy->geometry.compare("RAY") == 0)
      {
         
          comp.obj_phy->SetHaveBody(false);

          comp.obj_phy->collision_pos_offset.m[0] = 0.0f;
          comp.obj_phy->collision_pos_offset.m[1] = 0.0f;
          comp.obj_phy->collision_pos_offset.m[2] = 0.0f;



          comp.obj_phy->CreateGeometry();
        
      }
      else
      {
          comp.obj_phy->CreateGeometry();
      }
    }
    ImGui::NextColumn();

    //Shape depand stuff 
    {
      if (comp.obj_phy->geometry.compare("BOX") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Box Length:");
        ImGui::NextColumn();
       
        float float_box_length[3]{ static_cast<float>(comp.obj_phy->box_length[0]),static_cast<float>(comp.obj_phy->box_length[1]),static_cast<float>(comp.obj_phy->box_length[2]) };
        if (ImGui::DragFloat3("##Box Length", float_box_length, 0.01f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->box_length[0] = static_cast<dReal>(float_box_length[0]);
            comp.obj_phy->box_length[1] = static_cast<dReal>(float_box_length[1]);
            comp.obj_phy->box_length[2] = static_cast<dReal>(float_box_length[2]);

            if (comp.obj_phy->box_length[0] < 1.0)
            {
                comp.obj_phy->box_length[0] = 1.0;
            }

            if (comp.obj_phy->box_length[1] < 1.0)
            {
                comp.obj_phy->box_length[1] = 1.0;
            }

            if (comp.obj_phy->box_length[2] < 1.0)
            {
                comp.obj_phy->box_length[2] = 1.0;
            }

          comp.obj_phy->GeomBoxSetLengths(comp.obj_phy->box_length[0] * th->scale.m[0], comp.obj_phy->box_length[1] * th->scale.m[1], comp.obj_phy->box_length[2] * th->scale.m[2]);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Box Length With Scale:");
        ImGui::NextColumn();

        std::stringstream bl;
        dVector3 curr_box_length;
        comp.obj_phy->GeomBoxGetLengths(curr_box_length);

        bl.str(std::string());
        bl << "X:" << curr_box_length[0] << " Y:" << curr_box_length[1] << " Z:" << curr_box_length[2];
        ImGui::TextColored({ 0,10,255, 1 }, bl.str().c_str());

        ImGui::Columns(1);

        if (ImGui::Button("Generate Model Bounding Box"))
        {
           
            GrapHandle gh = obj_handle->GetHdl<GraphicsComp>();


            if (gh.IsValid())
            {
                NewPhyHandle nph = obj_handle->GetHdl<NewPhysicsComp>();
                TransformComp& tc = *m_transform[nph->GetTransHdl()];

                GenerateBoundingBox(comp.obj_phy, &tc, gh->modelfile);
            }
        }

        ImGui::Columns(2);


      }
      else if (comp.obj_phy->geometry.compare("CAPSULE") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Radius:");
        ImGui::NextColumn();

        float r = static_cast<float>(comp.obj_phy->radius);

        if (ImGui::DragFloat("##Capsule Radius", &r, 1.0f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->radius = static_cast<dReal>(r);

            if (comp.obj_phy->radius < 1.0)
                comp.obj_phy->radius = 1.0;

            dReal scalar = th->scale.m[0];

            if(scalar < th->scale.m[1])
                scalar = th->scale.m[1];

          comp.obj_phy->GeomCapsuleSetParams(comp.obj_phy->radius * scalar, comp.obj_phy->length * th->scale.m[2]);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Length:");
        ImGui::NextColumn();

        float l = static_cast<float>(comp.obj_phy->length);
        if (ImGui::DragFloat("##Capsule Length", &l, 0.01f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->length = static_cast<dReal>(l);

            if (comp.obj_phy->length < 1.0)
                comp.obj_phy->length = 1.0;

            dReal scalar = th->scale.m[0];

            if (scalar < th->scale.m[1])
                scalar = th->scale.m[1];

          comp.obj_phy->GeomCapsuleSetParams(comp.obj_phy->radius* scalar, comp.obj_phy->length* th->scale.m[2]);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        dReal curr_cap_radius;
        dReal curr_cap_length;
        comp.obj_phy->GeomCapsuleGetParams(&curr_cap_radius, &curr_cap_length);

        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Radius With Scale:");
        ImGui::NextColumn();
        
        std::stringstream cpr;
        cpr.str(std::string());
        cpr << curr_cap_radius;

        ImGui::TextColored({ 0,10,255, 1 }, cpr.str().c_str());
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Length With Scale:");
        ImGui::NextColumn();

        std::stringstream cpl;
        cpl.str(std::string());
        cpl << curr_cap_length;

        ImGui::TextColored({ 0,10,255, 1 }, cpl.str().c_str());
        ImGui::NextColumn();


      }
      else if (comp.obj_phy->geometry.compare("CYCLINDER") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Cyclinder Radius:");
        ImGui::NextColumn();
       
        float r = static_cast<float>(comp.obj_phy->radius);

        if (ImGui::DragFloat("##Cyclinder Radius", &r, 1.0f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->radius = static_cast<dReal>(r);

            if (comp.obj_phy->radius < 1.0)
                comp.obj_phy->radius = 1.0;

            dReal scalar = th->scale.m[0];

            if (scalar < th->scale.m[1])
                scalar = th->scale.m[1];

          comp.obj_phy->GeomCylinderSetParams(comp.obj_phy->radius* scalar, comp.obj_phy->length* th->scale.m[2]);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Cyclinder Length:");
        ImGui::NextColumn();

        float l = static_cast<float>(comp.obj_phy->length);
        if (ImGui::DragFloat("##Cyclinder Length", &l, 0.01f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->length = static_cast<dReal>(l);

            if (comp.obj_phy->length < 1.0)
                comp.obj_phy->length = 1.0;

            dReal scalar = th->scale.m[0];

            if (scalar < th->scale.m[1])
                scalar = th->scale.m[1];

          comp.obj_phy->GeomCylinderSetParams(comp.obj_phy->radius* scalar, comp.obj_phy->length* th->scale.m[2]);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        dReal curr_cyc_radius;
        dReal curr_cyc_length;
        comp.obj_phy->GeomCylinderGetParams(&curr_cyc_radius, &curr_cyc_length);

        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Radius With Scale:");
        ImGui::NextColumn();

        std::stringstream cyr;
        cyr.str(std::string());
        cyr << curr_cyc_radius;

        ImGui::TextColored({ 0,10,255, 1 }, cyr.str().c_str());
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Capsule Length With Scale:");
        ImGui::NextColumn();

        std::stringstream cyl;
        cyl.str(std::string());
        cyl << curr_cyc_length;

        ImGui::TextColored({ 0,10,255, 1 }, cyl.str().c_str());
        ImGui::NextColumn();

      }
      else if (comp.obj_phy->geometry.compare("PLANE") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Plane Param:");
        ImGui::NextColumn();

        dVector4 curr_plane_param;
        comp.obj_phy->GeomPlaneGetParams(curr_plane_param);

        float param[4] = { static_cast<float>(curr_plane_param[0]),static_cast<float>(curr_plane_param[1]) ,static_cast<float>(curr_plane_param[2]) ,static_cast<float>(curr_plane_param[3]) };

        if (ImGui::DragFloat4("##Plane Param", param, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
        {
            dReal a = static_cast<dReal>(param[0]);
            dReal b = static_cast<dReal>(param[1]);
            dReal c = static_cast<dReal>(param[2]);
            dReal d = static_cast<dReal>(param[3]);

            comp.obj_phy->GeomPlaneSetParams(a, b,c,d);
        }
        ImGui::NextColumn();
      }
      else if (comp.obj_phy->geometry.compare("RAY") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Plane Param:");
        ImGui::NextColumn();

        float l = static_cast<float>(comp.obj_phy->length);
        if (ImGui::DragFloat("##Ray Length", &l, 0.01f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->length = static_cast<dReal>(l);

            if (comp.obj_phy->length < 1.0)
                comp.obj_phy->length = 1.0;

          comp.obj_phy->GeomRaySetLength(comp.obj_phy->length * th->scale.m[2]);
        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Ray Length With Scale:");
        ImGui::NextColumn();

        std::stringstream rl;
        rl.str(std::string());
        rl << comp.obj_phy->GeomRayGetLength();

        ImGui::TextColored({ 0,10,255, 1 }, rl.str().c_str());
        ImGui::NextColumn();
      }
      else if (comp.obj_phy->geometry.compare("TRIMESH") == 0)
      {
        ImGui::Columns(1);
        
        if (ImGui::Button("Generate TriMesh From Mesh"))
        {
            GrapHandle gh = obj_handle->GetHdl<GraphicsComp>();

            if (gh.IsValid())
            {
                NewPhyHandle nph = obj_handle->GetHdl<NewPhysicsComp>();
                TransformComp& tc = *m_transform[nph->GetTransHdl()];
                GenerateTriMesh(comp.obj_phy,&tc, gh->modelfile);
            }
            else
            {
                comp.obj_phy->geometry = "SPHERE";
                comp.obj_phy->CreateGeometry();
            }
        }
        
        ImGui::Columns(2);
      }

      else
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Sphere Radius:");
        ImGui::NextColumn();

        float r = static_cast<float>(comp.obj_phy->radius);

        if (ImGui::DragFloat("##Sphere Radius", &r, 1.0f, 1.0f, FLT_MAX, "%.4f"))
        {
            comp.obj_phy->radius = static_cast<dReal>(r);

            if (comp.obj_phy->radius < 1.0)
                comp.obj_phy->radius = 1.0;

            dReal scalar = th->scale.m[0];

            if (scalar < th->scale.m[1])
                scalar = th->scale.m[1];

            if (scalar < th->scale.m[2])
                scalar = th->scale.m[2];

          comp.obj_phy->GeomSphereSetRadius(comp.obj_phy->radius * scalar);
          comp.obj_phy->UpdateMassInfo();
        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Sphere Radius With Scale:");
        ImGui::NextColumn();

        std::stringstream sr;
        sr.str(std::string());
        sr << comp.obj_phy->GeomSphereGetRadius();

        ImGui::TextColored({ 0,10,255, 1 }, sr.str().c_str());
        ImGui::NextColumn();

        ImGui::Columns(1);

        if (ImGui::Button("Generate Model Bounding Sphere"))
        {
            GrapHandle gh = obj_handle->GetHdl<GraphicsComp>();

            if (gh.IsValid())
            {
                NewPhyHandle nph = obj_handle->GetHdl<NewPhysicsComp>();
                TransformComp& tc = *m_transform[nph->GetTransHdl()];

                GenerateBoundingSphere(comp.obj_phy, &tc, gh->modelfile);
            }
        }

        ImGui::Columns(2);
      }
    }

    //  Planes are non-placeable geoms. This means that, unlike placeable geoms, planes do not have an assigned position and rotation
    if (!comp.obj_phy->geometry.compare("PLANE") == 0)
    {
      ImGui::TextColored({ 0,10,255, 1 }, "Geometry Offset Position");
      ImGui::NextColumn();
      if (ImGui::DragFloat3("##Geometry Offset Position", comp.obj_phy->collision_pos_offset.m, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
      {
          comp.obj_phy->mass.c[0] = static_cast<dReal>(comp.obj_phy->collision_pos_offset.m[0]);
          comp.obj_phy->mass.c[1] = static_cast<dReal>(comp.obj_phy->collision_pos_offset.m[1]);
          comp.obj_phy->mass.c[2] = static_cast<dReal>(comp.obj_phy->collision_pos_offset.m[2]);

      }
      ImGui::NextColumn();

      ImGui::TextColored({ 0,10,255, 1 }, "Model Pivot Position");
      ImGui::NextColumn();

      std::stringstream mp;
      mp.str(std::string());
      mp << "X:" << comp.obj_phy->model_pivot_point.m[0] << " Y:" << comp.obj_phy->model_pivot_point.m[1] << " Z:" << comp.obj_phy->model_pivot_point.m[2];

      ImGui::TextColored({ 0,10,255, 1 }, mp.str().c_str());
      ImGui::NextColumn();
    }

    ImGui::TextColored({ 0,10,255, 1 }, "Collision Enable:");
    ImGui::NextColumn();
    bool curr_collision_enable = comp.obj_phy->GeomCollisionIsEnabled();
    if (ImGui::Checkbox("##collision enable", &curr_collision_enable))
    {
      comp.obj_phy->GeomCollisionSetEnable(curr_collision_enable);
    }
    ImGui::NextColumn();
    ImGui::EndChild();
    ImGui::EndChild();
  }

  //RB Stuff
  if (!comp.obj_phy->geometry.compare("PLANE") == 0 && !comp.obj_phy->geometry.compare("RAY") == 0)
  {
    ImGui::BeginChild("RBStuff", ImVec2(0, ImGui::GetWindowSize().y * 0.3f), true);
    bool curr_have_body = comp.obj_phy->have_body;
    ImGui::Columns(2);
    ImGui::TextColored({ 0,10,255, 1 }, "IsDynamic");   ImGui::NextColumn();
    if (ImGui::Checkbox("##IsDynamic", &curr_have_body))
    {
      comp.obj_phy->SetHaveBody(curr_have_body);
    }
    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::BeginChild("RBStuffChild");
    if (comp.obj_phy->have_body)
    {

      ImGui::Columns(2);
      ImGui::TextColored({ 0,10,255, 1 }, "IsRigidBodyEnable");
      ImGui::NextColumn();
      bool curr_body_enable = comp.obj_phy->IsRigidBodyEnable();
      if (ImGui::Checkbox("##IsRigidBodyEnable", &curr_body_enable))
      {
        comp.obj_phy->SetRigidBodyEnable(curr_body_enable);
      }
      ImGui::NextColumn();
      ImGui::TextColored({ 0,10,255, 1 }, "Velocity:");
      ImGui::NextColumn();
      const dReal* curr_vel = dBodyGetLinearVel(comp.obj_phy->body);
      float vel[3] = { static_cast<float>(curr_vel[0]),static_cast<float>(curr_vel[1]) ,static_cast<float>(curr_vel[2]) };
      if (ImGui::DragFloat3("##Velocity", vel, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
      {
        comp.obj_phy->BodySetLinearVel(static_cast<dReal>(vel[0]), static_cast<dReal>(vel[1]), static_cast<dReal>(vel[2]));
      }
      ImGui::NextColumn();

      ImGui::TextColored({ 0,10,255, 1 }, "Angular Velocity:");
      ImGui::NextColumn();
      const dReal* curr_avel = dBodyGetAngularVel(comp.obj_phy->body);
      float avel[3] = { static_cast<float>(curr_avel[0]),static_cast<float>(curr_avel[1]) ,static_cast<float>(curr_avel[2]) };
      if (ImGui::DragFloat3("##Angular #Velocity", avel, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
      {
        comp.obj_phy->BodySetAngularVel(static_cast<dReal>(avel[0]), static_cast<dReal>(avel[1]), static_cast<dReal>(avel[2]));
      }
      ImGui::NextColumn();
      ImGui::TextColored({ 0,10,255, 1 }, "Gravity Enable:");
      ImGui::NextColumn();
      bool currgravity_enable = comp.obj_phy->BodyGetGravityMode();
      if (ImGui::Checkbox("##Gravity enable", &currgravity_enable))
      {
        comp.obj_phy->BodySetGravityEnable(currgravity_enable);
        comp.prevsBoolForGrav = currgravity_enable;
      }
      ImGui::NextColumn();

      ImGui::TextColored({ 0,10,255, 1 }, "Mass:");
      ImGui::NextColumn();
      float mass = static_cast<float>(comp.obj_phy->mass.mass);
      if (ImGui::DragFloat("##Mass", &mass, 0.01f, 1.0f, FLT_MAX, "%.4f"))
      {
          dReal mm = static_cast<dReal>(mass);

          if (mm < 1.0)
              mm = 1.0;
          comp.obj_phy->MassAdjust(mm);
      }
      ImGui::NextColumn();

      ImGui::TextColored({ 0,10,255, 1 }, "Center Of Mass:");
      ImGui::NextColumn();
     
      std::stringstream cm;
      cm.str(std::string());
      cm << "X:" << comp.obj_phy->mass.c[0] << " Y:" << comp.obj_phy->mass.c[1] << " Z:" << comp.obj_phy->mass.c[2];

      ImGui::TextColored({ 0,10,255, 1 }, cm.str().c_str());
   
      ImGui::NextColumn();

      
      ImGui::TextColored({ 0,10,255, 1 }, "inertia:");
      ImGui::NextColumn();

      std::stringstream i1;
      i1.str(std::string());
      i1 << "[" << comp.obj_phy->mass.I[(0) * 4 + (0)] << "][" << comp.obj_phy->mass.I[(0) * 4 + (1)] << "][" << comp.obj_phy->mass.I[(0) * 4 + (2)] << "]";
      ImGui::TextColored({ 0,10,255, 1 }, i1.str().c_str());
      ImGui::NextColumn();
      ImGui::NextColumn();

      std::stringstream i2;
      i2.str(std::string());
      i2 << "[" << comp.obj_phy->mass.I[(1) * 4 + (0)] << "][" << comp.obj_phy->mass.I[(1) * 4 + (1)] << "][" << comp.obj_phy->mass.I[(1) * 4 + (2)] << "]";
      ImGui::TextColored({ 0,10,255, 1 }, i2.str().c_str());
      ImGui::NextColumn();
      ImGui::NextColumn();

      std::stringstream i3;
      i3.str(std::string());
      i3 << "[" << comp.obj_phy->mass.I[(2) * 4 + (0)] << "][" << comp.obj_phy->mass.I[(2) * 4 + (1)] << "][" << comp.obj_phy->mass.I[(2) * 4 + (2)] << "]";
      ImGui::TextColored({ 0,10,255, 1 }, i3.str().c_str());
      ImGui::NextColumn();

      if (comp.obj_phy->geometry.compare("CAPSULE") == 0 || comp.obj_phy->geometry.compare("CYCLINDER") == 0)
      {
        ImGui::TextColored({ 0,10,255, 1 }, "Direction:");
        ImGui::NextColumn();

        int directionindex = -1;
        std::vector<const char*> axislist{ "XAXIS", "YAXIS", "ZAXIS" };

        uint32_t counterloh = 0;
        for (const auto& str : axislist)
        {
          std::string tostring = str;
          if (comp.obj_phy->direction_axis.compare(tostring) == 0)
          {
            directionindex = counterloh;
            break;
          }
          ++counterloh;
        }

        if (ImGui::Combo("##contactmode", &directionindex, axislist.data(), (uint32_t)axislist.size()))
        {
          comp.obj_phy->direction_axis = axislist[directionindex];

        }

      }
      ImGui::NextColumn();
    }
    ImGui::EndChild();
    ImGui::EndChild();
  }


  ImGui::BeginChild("PhysicsSurfaceStuff", ImVec2(0, ImGui::GetWindowSize().y * 0.3f), true);
  {
    ImGui::Columns(2);
    ImGui::TextColored({ 0,10,255, 1 }, "SetApplyPhysicsSurfaceEnable:");
    ImGui::NextColumn();
    bool curr_apply_physicssurface_enable = comp.obj_phy->IsApplyPhysicsSurfaceEnable();
    if (ImGui::Checkbox("##SetApplyPhysicsSurfaceEnable", &curr_apply_physicssurface_enable))
    {
      comp.obj_phy->SetApplyPhysicsSurfaceEnable(curr_apply_physicssurface_enable);
    }
    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::BeginChild("PhysicsSurfaceStuffChild");
    ImGui::NextColumn();
    //if (comp.obj_phy->IsApplyPhysicsSurfaceEnable())
    //{
    //  ImGui::Columns(1);
    //  ImGui::TextColored({ 0,10,255, 1 }, "Surface Mode Table:");
    //
    //  ImGui::Columns(2);
    //
    //  for (auto& it = comp.obj_phy->surface_mode_list.begin(); it != comp.obj_phy->surface_mode_list.end(); ++it)
    //  {
    //    ImGui::TextColored({ 0,10,255, 1 }, it->first.c_str());
    //    ImGui::NextColumn();
    //
    //    std::stringstream label;
    //    label.str(std::string());
    //    label << "##" << it->first;
    //
    //    if (ImGui::Checkbox(label.str().c_str(), &it->second))
    //    {
    //      comp.obj_phy->UpdateSurfaceMode();
    //    }
    //    ImGui::NextColumn();
    //
    //  }
    //
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Coefficient Friction:");
    //  ImGui::NextColumn();
    //  
    //  float mu = static_cast<float>(comp.obj_phy->contact.surface.mu);
    //  if (ImGui::DragFloat("##Friction 1", &mu, 0.01f, 0.0f, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.mu = static_cast<dReal>(mu);
    //
    //      if (comp.obj_phy->contact.surface.mu < 0.0)
    //          comp.obj_phy->contact.surface.mu = 0.0;
    //
    //      comp.obj_phy->contact.surface.mu2 = comp.obj_phy->contact.surface.mu;
    //  }
    //  ImGui::NextColumn();
    //
    //
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Densities:");
    //  ImGui::NextColumn();
    //  float rho = static_cast<float>(comp.obj_phy->contact.surface.rho);
    //  if (ImGui::DragFloat("##Densities 1", &rho, 0.01f, 0.0f, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.rho = static_cast<dReal>(rho);
    //
    //      if (comp.obj_phy->contact.surface.rho < 0.0)
    //          comp.obj_phy->contact.surface.rho = 0.0;
    //
    //    comp.obj_phy->contact.surface.rho2 = comp.obj_phy->contact.surface.rho;
    //    comp.obj_phy->contact.surface.rhoN = comp.obj_phy->contact.surface.rho;
    //  }
    //  ImGui::NextColumn();
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Restitution:");
    //  ImGui::NextColumn();
    //  float bo = static_cast<float>(comp.obj_phy->contact.surface.bounce);
    //  if (ImGui::DragFloat("##Bounce", &bo, 0.01f, 0.0f, 1.0f, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.bounce = static_cast<dReal>(bo);
    //
    //      if (comp.obj_phy->contact.surface.bounce < 0.0)
    //          comp.obj_phy->contact.surface.bounce = 0.0;
    //
    //      if (comp.obj_phy->contact.surface.bounce > 1.0)
    //          comp.obj_phy->contact.surface.bounce = 1.0;
    //
    //  }
    //  ImGui::NextColumn();
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Min Velocity To Bounce:");
    //  ImGui::NextColumn();
    //
    //  float bv = static_cast<float>(comp.obj_phy->contact.surface.bounce_vel);
    //  if (ImGui::DragFloat("##Bounce Velocity:", &bo, 0.01f, 0.0f, 1.0f, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.bounce_vel = static_cast<dReal>(bv);
    //
    //      if (comp.obj_phy->contact.surface.bounce_vel < 0.0)
    //          comp.obj_phy->contact.surface.bounce_vel = 0.0;
    //  }
    //
    //  ImGui::NextColumn();
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Error Reduction Parameter:");
    //  ImGui::NextColumn();
    //
    //  float erp = static_cast<float>(comp.obj_phy->contact.surface.soft_erp);
    //  if (ImGui::DragFloat("##Error Reduction Parameter", &erp, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.soft_erp = static_cast<dReal>(erp);
    //  }
    //  ImGui::NextColumn();
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Constraint Force:");
    //  ImGui::NextColumn();
    //  float cfm = static_cast<float>(comp.obj_phy->contact.surface.soft_cfm);
    //  if (ImGui::DragFloat("##Constraint Force", &cfm, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.soft_cfm = static_cast<dReal>(cfm);
    //  }
    //  ImGui::NextColumn();
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Friction:");
    //  ImGui::NextColumn();
    //  float fr = static_cast<float>(comp.obj_phy->contact.surface.motion1);
    //  if (ImGui::DragFloat("##Motion 1", &fr, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.motion1 = static_cast<dReal>(fr);
    //
    //    comp.obj_phy->contact.surface.motion2 = comp.obj_phy->contact.surface.motion1;
    //    comp.obj_phy->contact.surface.motionN = comp.obj_phy->contact.surface.motion1;
    //  }
    //  ImGui::NextColumn();
    //
    //
    //  ImGui::TextColored({ 0,10,255, 1 }, "Force Dependent Slip 1:");
    //  ImGui::NextColumn();
    //  float s = static_cast<float>(comp.obj_phy->contact.surface.slip1);
    //  if (ImGui::DragFloat("##Force Dependent Slip 1", &s, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
    //  {
    //      comp.obj_phy->contact.surface.slip1 = static_cast<dReal>(s);
    //    comp.obj_phy->contact.surface.slip2 = comp.obj_phy->contact.surface.slip1;
    //  }
    //}

    ImGui::Columns(1);
    ImGui::EndChild();
    ImGui::EndChild();
  }

  ImGui::Columns(1);

  ImGui::BeginChild("IgnoreList", ImVec2(0, ImGui::GetWindowSize().y * 0.3f), true);
  {
    ImGui::TextColored({ 0,10,255, 1 }, "Ignore Collision List:");
    ImGui::Separator();

    static std::vector<const char*> avaliable_choice_object;

    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
      if (it->name.compare(comp.GetGOHandle()->name) != 0)
      {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

        if (nph.IsValid())
        {
          bool not_found = true;

          for (auto& v = comp.obj_phy->ignore_collision_list.begin(); v != comp.obj_phy->ignore_collision_list.end(); ++v)
          {
            if (it->name.compare(*v) == 0)
            {
              not_found = false;
              break;
            }
          }

          if (not_found)
            avaliable_choice_object.push_back(it->name.c_str());
        }
      }
    }

    static int index_selectedla = -1;
    static std::string ignore_name;


    if (ImGui::Combo("##objchoice", &index_selectedla, avaliable_choice_object.data(), (uint32_t)avaliable_choice_object.size()))
    {
      ignore_name = avaliable_choice_object[index_selectedla];
    }
    ImGui::NextColumn();

    if (ImGui::Button("Add In Ignore List"))
    {
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

        if (nph.IsValid())
        {
          // is vaild
          if (it->name.compare(ignore_name) == 0)
          {
            bool notfoundleh = true;

            for (auto& c = comp.obj_phy->ignore_collision_list.begin(); c != comp.obj_phy->ignore_collision_list.end(); ++c)
            {
              if (ignore_name.compare(*c) == 0)
              {
                notfoundleh = false;
                break;

              }
            }
            if (notfoundleh)
              comp.obj_phy->ignore_collision_list.push_back(ignore_name);
            break;
          }
        }
      }
    }
    ImGui::Separator();
    ImGui::BeginChild("ChildOfIgnoreList");
    {
      ImGui::Columns(2);
      avaliable_choice_object.clear();
      //ImGui::NextColumn();

      static bool removelahack = false;
      static int indexlol = -1;

      int counterla = 0;

      for (auto& it = comp.obj_phy->ignore_collision_list.begin(); it != comp.obj_phy->ignore_collision_list.end(); ++it)
      {
        ImGui::TextColored({ 0,10,255, 1 }, (*it).c_str());
        ImGui::NextColumn();

        std::stringstream label;
        label.str(std::string());

        label << "Remove##-" << *it;

        if (ImGui::Button(label.str().c_str()))
        {
          indexlol = counterla;
          removelahack = true;
        }
        ImGui::NextColumn();
        ++counterla;
      }

      if (removelahack)
      {
        comp.obj_phy->ignore_collision_list.erase(comp.obj_phy->ignore_collision_list.begin() + indexlol);
        indexlol = -1;
        removelahack = false;

      }
      ImGui::EndChild();
    }
  }
  ImGui::EndChild();
  //ImGui::TextColored({ 0,10,255, 1 }, "Add To Interact Raycast:");
  //
  //ImGui::Checkbox("##interact", &comp.interact_obj);

  ImGui::BeginChild("CCD", ImVec2(0, ImGui::GetWindowSize().y * 0.3f), true);
  {
     ImGui::Columns(2);
     

     ImGui::TextColored({ 0,10,255, 1 }, "Continuous Collision Detection Enable:");
     ImGui::NextColumn();
    
     ImGui::Checkbox("##CCD", &comp.obj_phy->continue_collision_detection);
    
     ImGui::Separator();
     ImGui::Columns(1);
     ImGui::BeginChild("ChildOfCCD");
     ImGui::Columns(2);
    
     if (comp.obj_phy->continue_collision_detection)
     {
         static std::vector<const char*> avaliable_choice_text;
        
         for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
         {
             if (it->name.compare(comp.GetGOHandle()->name) != 0)
             {
                 NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

                 if (nph.IsValid())
                 {
                     if (nph->obj_phy->have_triggerbox && comp.obj_phy->continue_collision_detection_list_string.find(it->name) == comp.obj_phy->continue_collision_detection_list_string.end())
                     {
                         avaliable_choice_text.push_back(it->name.c_str());
                     }
                 }
             }
         }
         static std::unordered_map<GOTYPE, std::string> objectTypes;
         for (int i = 0; i < (int)GOTYPE::go_max; i++)
         {
             GOTP::GOTypeStrMap;

             
         }
        
         static int index_selected = -1;
         static int type_index = -1;
         static std::string selected_name;
         static std::string selected_type;
        
         if (ImGui::Combo("##objchoice", &index_selected, avaliable_choice_text.data(), (uint32_t)avaliable_choice_text.size()))
         {
             selected_name = avaliable_choice_text[index_selected];
         }
         ImGui::NextColumn();
        
         if (ImGui::Button("Add In CCD List"))
         {
             for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
             {
                 NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();
        
                 if (nph.IsValid())
                 {
                     std::string obj_name = it->name;
                              
        
                     if (obj_name.compare(selected_name) == 0)
                     {
                         comp.obj_phy->continue_collision_detection_list_string.emplace(selected_name, mTYPE_LOGIC_ONCOLLIDE_EXIT);
                         break;
                     }
                 }
             }
         }
         avaliable_choice_text.clear();
         ImGui::NextColumn();
           
         if (ImGui::Combo("##GOTYPE", &type_index, GOTP::GOtypeList.data(), (uint32_t)GOTP::GOtypeList.size()))
         {
             selected_type = GOTP::GOtypeList[type_index];
         }
         ImGui::NextColumn();
         if (ImGui::Button("Add all Objects with type"))
         {
             GOTYPE select = GOTP::strGOTypeMap.find(selected_type)->second;
             for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
             {
                 NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();


                 if (nph.IsValid())
                 {
                     GOTYPE obj_type = it->GetType();
                    

                     if ( obj_type == select)
                     {
                       //  std::string obj = it->name;
                         comp.obj_phy->continue_collision_detection_list_string.emplace(it->name, mTYPE_LOGIC_ONCOLLIDE_EXIT);
                     }
                 }
             }
         }
         static bool removehack = false;
         static std::string remove_name;

         
         for (auto& it = comp.obj_phy->continue_collision_detection_list_string.begin(); it != comp.obj_phy->continue_collision_detection_list_string.end(); ++it)
         {
             ImGui::TextColored({ 0,10,255, 1 }, it->first.c_str());
             ImGui::NextColumn();

             std::stringstream label;
             label.str(std::string());

             label << "Remove##" << it->first;

             if (ImGui::Button(label.str().c_str()))
             {
                 remove_name = it->first;
                 removehack = true;
             }
             ImGui::NextColumn();
         }

         if (removehack)
         {
             comp.obj_phy->continue_collision_detection_list_string.erase(remove_name);
         }

     }
    
     ImGui::EndChild();
  }
  ImGui::EndChild();


  ImGui::BeginChild("TriggerBoxReflectChild", ImVec2(0, ImGui::GetWindowSize().y * 0.3f), true);
  {
    ImGui::Columns(2);
    ImGui::TextColored({ 0,10,255, 1 }, "Have TriggerBox:");
    ImGui::NextColumn();

    if (ImGui::Checkbox("##TriggerBox", &comp.obj_phy->have_triggerbox))
    {
      if (comp.obj_phy->have_triggerbox)
      {
        comp.obj_trigger->GeomCollisionSetEnable(true);
        comp.obj_trigger->collision_pos_offset = comp.obj_phy->collision_pos_offset;
      }
      else
      {
        comp.obj_trigger->GeomCollisionSetEnable(false);
      }
    }
    ImGui::NextColumn();
    if (comp.obj_phy->have_triggerbox)
    {
      ImGui::TextColored({ 0,10,255, 1 }, "Collision Enable:");
      ImGui::NextColumn();
      bool curr_collision_enable = comp.obj_trigger->GeomCollisionIsEnabled();
      if (ImGui::Checkbox("##collision enablet", &curr_collision_enable))
      {
        comp.obj_trigger->GeomCollisionSetEnable(curr_collision_enable);
      }
      ImGui::NextColumn();
  
    }
    ImGui::Columns(1);
    ImGui::Separator();

    ImGui::BeginChild("TriggerBoxChild");
    {
      ImGui::Columns(2);
      if (comp.obj_phy->have_triggerbox)
      {

        ImGui::TextColored({ 0,10,255, 1 }, "Box Length:");
        ImGui::NextColumn();

        float float_box_length[3]{ static_cast<float>(comp.obj_trigger->box_length[0]),static_cast<float>(comp.obj_trigger->box_length[1]),static_cast<float>(comp.obj_trigger->box_length[2]) };
        if (ImGui::DragFloat3("##Box Lengtht", float_box_length, 0.01f, 1.0f, FLT_MAX, "%.4f"))
        {
          comp.obj_trigger->box_length[0] = static_cast<dReal>(float_box_length[0]);
          comp.obj_trigger->box_length[1] = static_cast<dReal>(float_box_length[1]);
          comp.obj_trigger->box_length[2] = static_cast<dReal>(float_box_length[2]);

          if (comp.obj_trigger->box_length[0] < 1.0)
          {
            comp.obj_trigger->box_length[0] = 1.0;
          }

          if (comp.obj_trigger->box_length[1] < 1.0)
          {
            comp.obj_trigger->box_length[1] = 1.0;
          }

          if (comp.obj_trigger->box_length[2] < 1.0)
          {
            comp.obj_trigger->box_length[2] = 1.0;
          }

          comp.obj_trigger->GeomBoxSetLengths(comp.obj_trigger->box_length[0] * th->scale.m[0], comp.obj_trigger->box_length[1] * th->scale.m[1], comp.obj_trigger->box_length[2] * th->scale.m[2]);

        }
        ImGui::NextColumn();

        ImGui::TextColored({ 0,10,255, 1 }, "Box Length With Scale:");
        ImGui::NextColumn();

        std::stringstream bl;
        dVector3 curr_box_length;
        comp.obj_trigger->GeomBoxGetLengths(curr_box_length);

        bl.str(std::string());
        bl << "X:" << curr_box_length[0] << " Y:" << curr_box_length[1] << " Z:" << curr_box_length[2];
        ImGui::TextColored({ 0,10,255, 1 }, bl.str().c_str());

        ImGui::Columns(1);

        if (ImGui::Button("Generate Model Bounding Trigger Box"))
        {

          GrapHandle gh = obj_handle->GetHdl<GraphicsComp>();


          if (gh.IsValid())
          {
            NewPhyHandle nph = obj_handle->GetHdl<NewPhysicsComp>();
            TransformComp& tc = *m_transform[nph->GetTransHdl()];

            GenerateBoundingBox(comp.obj_trigger, &tc, gh->modelfile);
          }
        }

        if (ImGui::Button("Generate Trigger Box From Physics Shape"))
        {

        }

        ImGui::Columns(2);

        ImGui::TextColored({ 0,10,255, 1 }, "Geometry Offset Position");
        ImGui::NextColumn();
        if (ImGui::DragFloat3("##Geometry Offset Positiont", comp.obj_trigger->collision_pos_offset.m, 0.01f, -FLT_MAX, FLT_MAX, "%.4f"))
        {


        }
        

        ImGui::TextColored({ 0,10,255, 1 }, "Ignore Collision List:");
        ImGui::Separator();

        static std::vector<const char*> avaliable_choice_objectt;

        for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
        {
            if (it->name.compare(comp.GetGOHandle()->name) != 0)
            {
                NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

                if (nph.IsValid())
                {
                    bool not_found = true;

                    for (auto& v = comp.obj_trigger->ignore_collision_list.begin(); v != comp.obj_trigger->ignore_collision_list.end(); ++v)
                    {
                        if (it->name.compare(*v) == 0)
                        {
                            not_found = false;
                            break;
                        }
                    }

                    if (not_found)
                        avaliable_choice_objectt.push_back(it->name.c_str());
                }
            }
        }

        static int index_selectedlat = -1;
        static std::string ignore_namet;


        if (ImGui::Combo("##objchoicet", &index_selectedlat, avaliable_choice_objectt.data(), (uint32_t)avaliable_choice_objectt.size()))
        {
            ignore_namet = avaliable_choice_objectt[index_selectedlat];
        }
        ImGui::NextColumn();

        if (ImGui::Button("Add In Ignore List Trigger"))
        {
            for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
            {
                NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

                if (nph.IsValid())
                {
                    // is vaild
                    if (it->name.compare(ignore_namet) == 0)
                    {
                        bool notfoundleh = true;

                        for (auto& c = comp.obj_trigger->ignore_collision_list.begin(); c != comp.obj_trigger->ignore_collision_list.end(); ++c)
                        {
                            if (ignore_namet.compare(*c) == 0)
                            {
                                notfoundleh = false;
                                break;

                            }
                        }
                        if (notfoundleh)
                            comp.obj_trigger->ignore_collision_list.push_back(ignore_namet);
                        break;
                    }
                }
            }
        }

        ImGui::Columns(2);
        avaliable_choice_objectt.clear();
        //ImGui::NextColumn();

        static bool removelahackt = false;
        static int indexlolt = -1;

        int counterla = 0;

        for (auto& it = comp.obj_trigger->ignore_collision_list.begin(); it != comp.obj_trigger->ignore_collision_list.end(); ++it)
        {
            ImGui::TextColored({ 0,10,255, 1 }, (*it).c_str());
            ImGui::NextColumn();

            std::stringstream label;
            label.str(std::string());

            label << "Removet##-" << *it;

            if (ImGui::Button(label.str().c_str()))
            {
                indexlolt = counterla;
                removelahackt = true;
            }
            ImGui::NextColumn();
            ++counterla;
        }

        if (removelahackt)
        {
            comp.obj_trigger->ignore_collision_list.erase(comp.obj_trigger->ignore_collision_list.begin() + indexlolt);
            indexlolt = -1;
            removelahackt = false;

        }

      }
    }
    ImGui::EndChild();
  }
  ImGui::EndChild();


  ImGui::Columns(1);
  ImGui::TextColored({ 0,10,255, 1 }, "Ignore Collision List Ray:");
  ImGui::Separator();

  ImGui::Columns(2);

  static std::vector<const char*> avaliable_choice_objectr;

  for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
  {
      if (it->name.compare(comp.GetGOHandle()->name) != 0)
      {
          NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

          if (nph.IsValid())
          {
              bool not_found = true;

              for (auto& v = comp.obj_ray->ignore_collision_list.begin(); v != comp.obj_ray->ignore_collision_list.end(); ++v)
              {
                  if (it->name.compare(*v) == 0)
                  {
                      not_found = false;
                      break;
                  }
              }

              if (not_found)
                  avaliable_choice_objectr.push_back(it->name.c_str());
          }
      }
  }

  static int index_selectedlar = -1;
  static std::string ignore_namer;


  if (ImGui::Combo("##objchoicer", &index_selectedlar, avaliable_choice_objectr.data(), (uint32_t)avaliable_choice_objectr.size()))
  {
      ignore_namer = avaliable_choice_objectr[index_selectedlar];
  }
  ImGui::NextColumn();

  if (ImGui::Button("Add In Ignore List Ray"))
  {
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
          NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

          if (nph.IsValid())
          {
              // is vaild
              if (it->name.compare(ignore_namer) == 0)
              {
                  bool notfoundleh = true;

                  for (auto& c = comp.obj_ray->ignore_collision_list.begin(); c != comp.obj_ray->ignore_collision_list.end(); ++c)
                  {
                      if (ignore_namer.compare(*c) == 0)
                      {
                          notfoundleh = false;
                          break;

                      }
                  }
                  if (notfoundleh)
                      comp.obj_ray->ignore_collision_list.push_back(ignore_namer);
                  break;
              }
          }
      }
  }

  ImGui::Columns(2);
  avaliable_choice_objectr.clear();
  //ImGui::NextColumn();

  static bool removelahackr = false;
  static int indexlolr = -1;

  int counterlar = 0;

  for (auto& it = comp.obj_ray->ignore_collision_list.begin(); it != comp.obj_ray->ignore_collision_list.end(); ++it)
  {
      ImGui::TextColored({ 0,10,255, 1 }, (*it).c_str());
      ImGui::NextColumn();

      std::stringstream label;
      label.str(std::string());

      label << "Remover##-" << *it;

      if (ImGui::Button(label.str().c_str()))
      {
          indexlolr = counterlar;
          removelahackr = true;
      }
      ImGui::NextColumn();
      ++counterlar;
  }

  if (removelahackr)
  {
      comp.obj_ray->ignore_collision_list.erase(comp.obj_ray->ignore_collision_list.begin() + indexlolr);
      indexlolr = -1;
      removelahackr = false;

  }
  ImGui::Separator();


  ImGui::Columns(1);
  ImGui::TextColored({ 0,10,255, 1 }, "Ignore Collision List Floor Ray:");
  ImGui::Separator();

  ImGui::Columns(2);

  static std::vector<const char*> avaliable_choice_objectfr;

  for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
  {
      if (it->name.compare(comp.GetGOHandle()->name) != 0)
      {
          NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

          if (nph.IsValid())
          {
              bool not_found = true;

              for (auto& v = comp.obj_floor_ray->ignore_collision_list.begin(); v != comp.obj_floor_ray->ignore_collision_list.end(); ++v)
              {
                  if (it->name.compare(*v) == 0)
                  {
                      not_found = false;
                      break;
                  }
              }

              if (not_found)
                  avaliable_choice_objectfr.push_back(it->name.c_str());
          }
      }
  }

  static int index_selectedlafr = -1;
  static std::string ignore_namefr;


  if (ImGui::Combo("##objchoicefr", &index_selectedlafr, avaliable_choice_objectfr.data(), (uint32_t)avaliable_choice_objectfr.size()))
  {
      ignore_namefr = avaliable_choice_objectfr[index_selectedlafr];
  }
  ImGui::NextColumn();

  if (ImGui::Button("Add In Ignore List Floor Ray"))
  {
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
          NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

          if (nph.IsValid())
          {
              // is vaild
              if (it->name.compare(ignore_namefr) == 0)
              {
                  bool notfoundleh = true;

                  for (auto& c = comp.obj_floor_ray->ignore_collision_list.begin(); c != comp.obj_floor_ray->ignore_collision_list.end(); ++c)
                  {
                      if (ignore_namefr.compare(*c) == 0)
                      {
                          notfoundleh = false;
                          break;

                      }
                  }
                  if (notfoundleh)
                      comp.obj_floor_ray->ignore_collision_list.push_back(ignore_namefr);
                  break;
              }
          }
      }
  }

  ImGui::Columns(2);
  avaliable_choice_objectfr.clear();
  //ImGui::NextColumn();

  static bool removelahackfr = false;
  static int indexlolfr = -1;

  int counterlafr = 0;

  for (auto& it = comp.obj_floor_ray->ignore_collision_list.begin(); it != comp.obj_floor_ray->ignore_collision_list.end(); ++it)
  {
      ImGui::TextColored({ 0,10,255, 1 }, (*it).c_str());
      ImGui::NextColumn();

      std::stringstream label;
      label.str(std::string());

      label << "Removefr##-" << *it;

      if (ImGui::Button(label.str().c_str()))
      {
          indexlolfr = counterlafr;
          removelahackfr = true;
      }
      ImGui::NextColumn();
      ++counterlafr;
  }

  if (removelahackfr)
  {
      comp.obj_floor_ray->ignore_collision_list.erase(comp.obj_floor_ray->ignore_collision_list.begin() + indexlolfr);
      indexlolfr = -1;
      removelahackfr = false;

  }

  ImGui::Columns(1);

}

BaseHandle NewPhysicsManager::CreateComponent()
{
	NewPhyHandle hdl = m_components.GetNewObject();
	NewPhysicsComp& physicsComponent = *m_components[hdl];

  //ConstructUserDefault(&physicsComponent, &tc);
	
	//CHECK();
	return hdl;
}

void NewPhysicsManager::ConstructUserDefault(NewPhysicsComp *comp, TransformComp* tc)
{
   
    GOHandle go_handle = comp->GetGOHandle();
    GrapHandle gh = go_handle->GetHdl<GraphicsComp>();

    if (gh.IsValid())
    {

        GenerateBoundingBox(comp->obj_phy, tc, gh->modelfile);
    }
}

void NewPhysicsManager::DestroyComponent(BaseHandle hdl)
{
	NewPhyHandle phdl(hdl);
	NewPhysicsComp& pc = *m_components[phdl];
    pc.DestroyPhysicsComp();
	m_components.ReleaseObj(phdl);
}

void NewPhysicsManager::InitComponent(const GameObject& go, BaseHandle hdl)
{
	NewPhyHandle phdl(hdl);
	m_components[phdl]->SetGOHandle(go.GetSelfHandle());
	NewPhysicsComp* newcomp = m_components[phdl];
	newcomp->SetTransHdl(Handle<TransformComp>(go[TRANSFORM]));

   TransformComp& tc = *m_transform[newcomp->GetSelfHandle()->GetTransHdl()];

  
}


void NewPhysicsManager::CopyComponentToComponent(NewPhysicsComp& target, const NewPhysicsComp& source)
{


    std::cout << "CopyComponentToComponent still explode" << std::endl;

    //auto th = target.GetTransHdl();
    //dGeomSetBody(target.obj_phy->geom, 0);
    //
    //target.Enabled = source.Enabled;
    //
    //target.obj_phy->geometry = source.obj_phy->geometry;
    //
    //target.obj_phy->collision_pos_offset = source.obj_phy->collision_pos_offset;
    //
    //if (target.obj_phy->geometry.compare("TRIMESH") == 0)
    //{
    //    target.obj_phy->vertex.clear();
    //    target.obj_phy->indices.clear();
    //
    //    target.obj_phy->CreateTrimesh();
    //}
    //else
    //{
    //    target.obj_phy->CreateGeometry();
    //
    //
    //    if (target.obj_phy->geometry.compare("BOX") == 0)
    //    {
    //        target.obj_phy->box_length[0] = source.obj_phy->box_length[0];
    //        target.obj_phy->box_length[1] = source.obj_phy->box_length[1];
    //        target.obj_phy->box_length[2] = source.obj_phy->box_length[2];
    //
    //
    //        target.obj_phy->GeomBoxSetLengths(target.obj_phy->box_length[0] * th->scale.m[0], target.obj_phy->box_length[1] * th->scale.m[1], target.obj_phy->box_length[2] * th->scale.m[2]);
    //
    //
    //    }
    //    else if (target.obj_phy->geometry.compare("SPHERE") == 0)
    //    {
    //        target.obj_phy->radius = source.obj_phy->radius;
    //
    //        dReal scalar = th->scale.m[0];
    //
    //        if (scalar < th->scale.m[1])
    //            scalar = th->scale.m[1];
    //
    //        if (scalar < th->scale.m[2])
    //            scalar = th->scale.m[2];
    //
    //        target.obj_phy->GeomSphereSetRadius(target.obj_phy->radius * scalar);
    //
    //    }
    //    else if (target.obj_phy->geometry.compare("CAPSULE") == 0)
    //    {
    //        target.obj_phy->length = source.obj_phy->length;
    //        target.obj_phy->radius = source.obj_phy->radius;
    //
    //        dReal scalar = th->scale.m[0];
    //
    //        if (scalar < th->scale.m[1])
    //            scalar = th->scale.m[1];
    //
    //        target.obj_phy->GeomCapsuleSetParams(target.obj_phy->radius * scalar, target.obj_phy->length * th->scale.m[2]);
    //
    //        target.obj_phy->direction_axis = source.obj_phy->direction_axis;
    //    }
    //    else if (target.obj_phy->geometry.compare("CYCLINDER") == 0)
    //    {
    //        target.obj_phy->length = source.obj_phy->length;
    //        target.obj_phy->radius = source.obj_phy->radius;
    //
    //        dReal scalar = th->scale.m[0];
    //
    //        if (scalar < th->scale.m[1])
    //            scalar = th->scale.m[1];
    //
    //        target.obj_phy->GeomCylinderSetParams(target.obj_phy->radius * scalar, target.obj_phy->length * th->scale.m[2]);
    //
    //        target.obj_phy->direction_axis = source.obj_phy->direction_axis;
    //    }
    //    else if (target.obj_phy->geometry.compare("PLANE") == 0)
    //    {
    //        dVector4 result;
    //        source.obj_phy->GeomPlaneGetParams(result);
    //        target.obj_phy->GeomPlaneSetParams(result[0], result[1], result[2], result[3]);
    //    }
    //    else if (target.obj_phy->geometry.compare("RAY") == 0)
    //    {
    //        target.obj_phy->length = source.obj_phy->length;
    //        target.obj_phy->GeomRaySetLength(target.obj_phy->length * th->scale.m[2]);
    //    }
    //}
    //
    //target.obj_phy->GeomCollisionSetEnable(source.obj_phy->GeomCollisionIsEnabled());
    //target.obj_phy->SetApplyPhysicsSurfaceEnable(source.obj_phy->IsApplyPhysicsSurfaceEnable());
    //
    //target.obj_phy->have_body = source.obj_phy->have_body;
    //
    //
    //if (target.obj_phy->have_body)
    //{
    //    target.obj_phy->SetRigidBodyEnable(source.obj_phy->IsRigidBodyEnable());
    //    target.obj_phy->BodySetGravityEnable(source.obj_phy->BodyGetGravityMode());
    //
    //    target.obj_phy->mass = source.obj_phy->mass;
    //
    //    target.obj_phy->BodySetQuaternion(th->rotation);
    //
    //    target.obj_phy->BodySetPosition(th->pos + target.obj_phy->collision_pos_offset);
    //
    //}
    //else
    //{
    //    target.obj_phy->GeomSetQuaternion(th->rotation);
    //    target.obj_phy->GeomSetPosition(th->pos + target.obj_phy->collision_pos_offset);
    //}
    //
    //target.obj_phy->continue_collision_detection = source.obj_phy->continue_collision_detection;
    //
    //target.obj_phy->continue_collision_detection_list_string.clear();
    //
    //if (target.obj_phy->continue_collision_detection)
    //{
    //    for (auto& it = source.obj_phy->continue_collision_detection_list_string.begin(); it != source.obj_phy->continue_collision_detection_list_string.end(); ++it)
    //    {
    //        if (target.GetGOHandle()->name.compare(it->first) == 0)
    //            continue;
    //
    //        target.obj_phy->continue_collision_detection_list_string.emplace(it->first,it->second);
    //    }
    //}
    //
    //target.obj_phy->ignore_collision_list.clear();
    //for (auto& it = source.obj_phy->ignore_collision_list.begin(); it != source.obj_phy->ignore_collision_list.end(); ++it)
    //{
    //    if (target.GetGOHandle()->name.compare(*it) == 0)
    //        continue;
    //
    //    target.obj_phy->ignore_collision_list.push_back(*it);
    //}
    //
    //target.obj_phy->have_triggerbox = source.obj_phy->have_triggerbox;
    //
    //target.obj_trigger->ignore_collision_list.clear();
    //
    //if (target.obj_phy->have_triggerbox)
    //{
    //    target.obj_trigger->box_length[0] = source.obj_trigger->box_length[0];
    //    target.obj_trigger->box_length[1] = source.obj_trigger->box_length[1];
    //    target.obj_trigger->box_length[2] = source.obj_trigger->box_length[2];
    //
    //    target.obj_trigger->GeomBoxSetLengths(target.obj_trigger->box_length[0] * th->scale.m[0], target.obj_trigger->box_length[1] * th->scale.m[1], target.obj_trigger->box_length[2] * th->scale.m[2]);
    //
    //    target.obj_trigger->collision_pos_offset = source.obj_trigger->collision_pos_offset;
    //
    //    target.obj_trigger->GeomCollisionSetEnable(source.obj_trigger->GeomCollisionIsEnabled());
    //
    //    for (auto& it = source.obj_trigger->ignore_collision_list.begin(); it != source.obj_trigger->ignore_collision_list.end(); ++it)
    //    {
    //        if (target.GetGOHandle()->name.compare(*it) == 0)
    //            continue;
    //
    //        target.obj_trigger->ignore_collision_list.push_back(*it);
    //    }
    //
    //}
    //target.obj_trigger->GeomSetQuaternion(th->rotation);
    //target.obj_trigger->GeomSetPosition(th->pos + target.obj_trigger->collision_pos_offset);
    //
    //target.obj_ray->ignore_collision_list.clear();
    //for (auto& it = source.obj_ray->ignore_collision_list.begin(); it != source.obj_ray->ignore_collision_list.end(); ++it)
    //{
    //    if (target.GetGOHandle()->name.compare(*it) == 0)
    //        continue;
    //
    //    target.obj_ray->ignore_collision_list.push_back(*it);
    //}
    //target.obj_floor_ray->ignore_collision_list.clear();
    //for (auto& it = source.obj_floor_ray->ignore_collision_list.begin(); it != source.obj_floor_ray->ignore_collision_list.end(); ++it)
    //{
    //    if (target.GetGOHandle()->name.compare(*it) == 0)
    //        continue;
    //
    //    target.obj_floor_ray->ignore_collision_list.push_back(*it);
    //}


}

void NewPhysicsManager::HandleMsg(MsgData_Base* mb)
{
	switch (mb->type)
	{
	case mTYPE_STOP:
    {
  
    }
	case mTYPE_GAME_PAUSE:
	{
     
#ifdef _RELEASE 

        if (!ASC_SYSTEM.hack_releasemode_submission)
        {
            DestroyODE();

            if (!world_space_is_alive)
            {

                world = dWorldCreate();


                dWorldSetGravity(world, 0, -125, 0);

                dWorldSetQuickStepNumIterations(world, 64);


                space = dHashSpaceCreate(0);
                contactgroup = dJointGroupCreate(0);


                world_space_is_alive = true;
            }
        }
#endif
		isEnabled = false;
	} break;

	case mTYPE_PLAY:
    {
        
    }
	case mTYPE_GAME_RESUME:
	{
#ifdef _RELEASE 
        if (!ASC_SYSTEM.hack_releasemode_submission)
        {
            InitODE();
        }
#endif
		isEnabled = true;
	} break;

	default: break;
	}
}
void NewPhysicsManager:: HandleForceReg(MsgData_Base* mb )
{
     Msg_Force_Reg_Collide*  realMsg= static_cast<Msg_Force_Reg_Collide*> (mb);
     NewPhyHandle phyHandle = realMsg->registeeGameObject->GetHdl<NewPhysicsComp>();
  switch (mb->type)
  {
  
    case mTYPE_BY_GO_HANDLE:
    {
      break;
    }
    case mTYPE_BY_GO_NAME:
    {
      //std::cout << "Tetst\n";
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();
  
        if (nph.IsValid())
        {
          std::string obj_name = it->name;
  
            if (obj_name.compare( realMsg->nameOfGameObject) == 0)
            {
              phyHandle->obj_phy->continue_collision_detection_list_string.emplace(realMsg->nameOfGameObject, mTYPE_LOGIC_ONCOLLIDE_EXIT);
              break;
            }
        }
      }
      break;
    }
    case mTYPE_BY_GO_TYPE:
    {
      GOTYPE select = realMsg->objType;
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();
  
  
        if (nph.IsValid())
        {
          GOTYPE obj_type = it->GetType();
  
  
          if (obj_type == select)
          {
            //  std::string obj = it->name;

              if(phyHandle.IsValid())
                  phyHandle->obj_phy->continue_collision_detection_list_string.emplace(it->name, mTYPE_LOGIC_ONCOLLIDE_EXIT);
          }
        }
      }
      break;
    }
  }
}
void NewPhysicsManager::HandleForceRegIgnoreList(MsgData_Base* mb)
{
  Msg_Force_Reg_Collide* realMsg = static_cast<Msg_Force_Reg_Collide*> (mb);
  NewPhyHandle phyHandle = realMsg->registeeGameObject->GetHdl<NewPhysicsComp>();
  switch (mb->type)
  {

    case mTYPE_BY_GO_TYPE_IGNORERAY:
    {
      GOTYPE select = realMsg->objType;
      for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
      {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();


        if (nph.IsValid())
        {
          GOTYPE obj_type = it->GetType();


          if (obj_type == select)
          {
            //  std::string obj = it->name;

            if (phyHandle.IsValid())
            {
              bool notfoundleh = true;
              bool notfoundalsoleh = true; 
              for (auto& c = nph->obj_ray->ignore_collision_list.begin(); c != nph->obj_ray->ignore_collision_list.end(); ++c)
              {
                if (realMsg->registeeGameObject->name.compare(*c) == 0)
                {
                  notfoundleh = false;
                  break;

                }
              }
              for (auto& c = nph->obj_floor_ray->ignore_collision_list.begin(); c != nph->obj_floor_ray->ignore_collision_list.end(); c++)
              {
                if (realMsg->registeeGameObject->name.compare(*c) == 0)
                {
                  notfoundalsoleh = false;
                  break;

                }
              }
              if (notfoundleh)
                nph->obj_ray->ignore_collision_list.push_back(realMsg->registeeGameObject->name);
             
              if (notfoundalsoleh)
                nph->obj_floor_ray->ignore_collision_list.push_back(realMsg->registeeGameObject->name);
              break;
            }
          }
        }
      }
      break;
    }
  }
}
void NewPhysicsManager::DestroyODE()
{
    // clear everything resconstruct
    ccd_list.clear();
    for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
    {
        NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

        if (nph.IsValid())
        {
            if (nph->obj_ray != nullptr)
            {
                delete nph->obj_ray;
                nph->obj_ray = nullptr;
            }

            if (nph->obj_trigger != nullptr)
            {
                delete nph->obj_trigger;
                nph->obj_trigger = nullptr;
            }

            if (nph->obj_phy != nullptr)
            {
                delete nph->obj_phy;
                nph->obj_phy = nullptr;
            }
        }
    }
    if (world_space_is_alive)
    {
   
        dJointGroupEmpty(contactgroup);
        dJointGroupDestroy(contactgroup);
        dSpaceDestroy(space);
       // dWorldDestroy(world);
       // dCloseODE();
        world_space_is_alive = false;
    }
   
    init_le = false;
  
  
}

void NewPhysicsManager::WorldSetting()
{
    // can't add aymore high gravity, it will make obj half way sink!
    dWorldSetGravity(world, 0, -300, 0);
    dWorldSetCFM(world, 1e-5);
    dWorldSetAutoDisableFlag(world, 1);
    dWorldSetLinearDamping(world, 0.01);
    dWorldSetAngularDamping(world, 1);
    dWorldSetMaxAngularSpeed(world, 0);

    dWorldSetContactSurfaceLayer(world, 0.001);
    dWorldSetAutoDisableAverageSamplesCount(world, 10);
}



void NewPhysicsManager::InitODESerialize()
{
    if (!world_space_is_alive)
    {
       // initODE = dInitODE2(0);
       // world = dWorldCreate();


        space = dHashSpaceCreate(0);

        dSpaceSetCleanup(space, 1);
        contactgroup = dJointGroupCreate(0);

        world_space_is_alive = true;

        WorldSetting();

    }
    changesceneinithack = true;

    force_update_once = true;
}

void NewPhysicsManager::InitODE()
{
    if (!world_space_is_alive)
    {
       // initODE = dInitODE2(0);
       // world = dWorldCreate();


        space = dHashSpaceCreate(0);
        dSpaceSetCleanup(space, 1);

        contactgroup = dJointGroupCreate(0);

        world_space_is_alive = true;

        WorldSetting();
      
    }

    if (changesceneinithack)
    {
        //isEnabled = false;
        ccd_list.clear();

        for (auto& it = ASC_FACTORY.GetGameObjectList().begin(); it != ASC_FACTORY.GetGameObjectList().end(); ++it)
        {
            NewPhyHandle nph = it->GetHdl<NewPhysicsComp>();

            if (nph.IsValid())
            {

                TransformComp& transform = *m_transform[nph->GetTransHdl()];

                Vec3 offset_with_rot = transform.rotation * nph->obj_phy->collision_pos_offset;

                Vec3 pos_with_collision_offset = transform.pos + offset_with_rot;

                //Vec3 pos_with_collision_offset = transform.pos + nph->obj_phy->collision_pos_offset;
                if (nph->obj_phy->have_body)
                {


                    nph->obj_phy->BodySetQuaternion(transform.rotation);

                    nph->obj_phy->BodySetPosition(pos_with_collision_offset);

                    dGeomSetBody(nph->obj_phy->geom, nph->obj_phy->body);

                    nph->obj_phy->DebugDangerFunction();

                }
                else
                {
                    nph->obj_phy->GeomSetQuaternion(transform.rotation);

                    nph->obj_phy->GeomSetPosition(pos_with_collision_offset);


                }


                // debug_trigger
                nph->obj_phy->debug_collided = false;

                if (nph->obj_phy->continue_collision_detection)
                {
                    nph->obj_phy->InitCCD();
                    ccd_list.emplace(nph->obj_trigger, &nph->obj_phy->continue_collision_detection_list);
                }

                if (nph->obj_phy->have_triggerbox)
                {
                    Vec3 trigger_pos_with_collision_offset = transform.pos + nph->obj_trigger->collision_pos_offset;
                    nph->obj_trigger->GeomSetQuaternion(transform.rotation);
                    nph->obj_trigger->GeomSetPosition(trigger_pos_with_collision_offset);
                }

                if (nph->obj_phy->hack_always_able_raycast)
                {
                    //Vec3 ray_pos = transform.pos + nph->obj_ray->collision_pos_offset;
                    //nph->obj_ray->GeomSetPosition(ray_pos);

                    Vec3 rayfloor_pos = transform.pos + nph->obj_floor_ray->collision_pos_offset;
                    nph->obj_floor_ray->GeomSetPosition(rayfloor_pos);
                }
            }
        }
        //isEnabled = true;

        changesceneinithack = false;
    }
    init_le = true;
    WorldSetting();

    fps_remainder = 0.0f;
    CanUpdateInput = false;

 
}

dWorldID* NewPhysicsManager::Phy_GetWorldPtr()
{

	return &world;
}

dSpaceID* NewPhysicsManager::Phy_GetSpacePtr()
{
	return &space;
}

void NewPhysicsManager::Phy_SetWorldGravity(double value)
{
	
	dWorldSetGravity(world, 0, value, 0);
}

dReal NewPhysicsManager::Phy_GetWorldGravity()
{
    dVector3 g;
    dWorldGetGravity(world, g);
	return g[1];
}



void NewPhysicsManager::GenerateBoundingSphere(ODENode* pc, TransformComp* tc, std::string& model_file)
{
    if (ASC_GMAN.m_objLoader.m_models.find(model_file) != ASC_GMAN.m_objLoader.m_models.end())
    {
        auto& model_container = ASC_GMAN.m_objLoader.m_models.find(model_file)->second;

        int index = static_cast<int>(model_container.size() - 1);

        // is radius not diameter
        Vec3 result = (model_container[index].maxPos - model_container[index].minPos) * 0.5f;

        dReal size[3] = { static_cast<dReal>(result.m[0]),static_cast<dReal>(result.m[1]),static_cast<dReal>(result.m[2]) };

        pc->radius = size[0];

        if (size[1] > pc->radius)
        {
            pc->radius = size[1];
        }

        if (size[2] > pc->radius)
        {
            pc->radius = size[2];
        }

        if (1.0f > pc->radius)
        {
            pc->radius = 1.0f;
        }

        float scalar = tc->scale.m[0];

        if (tc->scale.m[1] > scalar)
        {
            scalar = tc->scale.m[1];
        }

        if (tc->scale.m[2] > scalar)
        {
            scalar = tc->scale.m[2];
        }


        dGeomSphereSetRadius(pc->geom, pc->radius * scalar);

        pc->model_pivot_point = model_container[index].pivot;

        pc->collision_pos_offset.m[0] = (pc->model_pivot_point.m[0] * -1.0f) * tc->scale.m[0];
        pc->collision_pos_offset.m[1] = (pc->model_pivot_point.m[1] * -1.0f) * tc->scale.m[1];
        pc->collision_pos_offset.m[2] = (pc->model_pivot_point.m[2] * -1.0f) * tc->scale.m[2];

        pc->GeomSetQuaternion(tc->rotation);
        pc->GeomSetPosition(tc->pos + pc->collision_pos_offset);

        pc->BodySetQuaternion(tc->rotation);
        pc->BodySetPosition(tc->pos + pc->collision_pos_offset);


        // update mass
        pc->UpdateMassInfo();
    }

}

void NewPhysicsManager::GenerateBoundingBox(ODENode* pc, TransformComp* tc, std::string& model_file)
{

    if (ASC_GMAN.m_objLoader.m_models.find(model_file) != ASC_GMAN.m_objLoader.m_models.end())
    {
        auto& model_container = ASC_GMAN.m_objLoader.m_models.find(model_file)->second;

        int index = static_cast<int>(model_container.size() - 1);

        pc->model_pivot_point = model_container[index].pivot;

        Vec3 result = model_container[index].maxPos - model_container[index].minPos;

        pc->box_length[0] = static_cast <dReal>(result.m[0]);
        pc->box_length[1] = static_cast <dReal>(result.m[1]);
        pc->box_length[2] = static_cast <dReal>(result.m[2]);

        // prevent 0
        if (pc->box_length[0] < 1.0)
            pc->box_length[0] = 1.0;

        if (pc->box_length[1] < 1.0)
            pc->box_length[1] = 1.0;

        if (pc->box_length[2] < 1.0)
            pc->box_length[2] = 1.0;

        dGeomBoxSetLengths(pc->geom, pc->box_length[0] * tc->scale.m[0], pc->box_length[1] * tc->scale.m[1], pc->box_length[2] * tc->scale.m[2]);



        // update collision offsetpivot point


        pc->collision_pos_offset.m[0] = (pc->model_pivot_point.m[0] * -1.0f) * tc->scale.m[0];
        pc->collision_pos_offset.m[1] = (pc->model_pivot_point.m[1] * -1.0f) * tc->scale.m[1];
        pc->collision_pos_offset.m[2] = (pc->model_pivot_point.m[2] * -1.0f) * tc->scale.m[2];

        pc->GeomSetQuaternion(tc->rotation);
        pc->GeomSetPosition(tc->pos + pc->collision_pos_offset);

        pc->BodySetQuaternion(tc->rotation);
        pc->BodySetPosition(tc->pos + pc->collision_pos_offset);


        // update mass
        pc->UpdateMassInfo();
    }
    
}

void NewPhysicsManager::GenerateTriMesh(ODENode* pc, TransformComp* tc, std::string& model_file )
{
 
     pc->vertex.clear();
     pc->indices.clear();

     auto& model_container = ASC_GMAN.m_objLoader.m_objs.find(model_file)->second;

   

     pc->model_pivot_point = model_container[0].pivot;

      pc->collision_pos_offset.m[0] = 0.0;
      pc->collision_pos_offset.m[1] = 0.0;
      pc->collision_pos_offset.m[2] = 0.0;

      int append_indces = 0;
      int largest_number_indices = 0;

      for (auto& it = model_container.begin(); it != model_container.end(); ++it)
      {
          append_indces = largest_number_indices;

          auto& model_vertex_container = it->output;

          for (auto& it = model_vertex_container.begin(); it != model_vertex_container.end(); ++it)
          {

              pc->vertex.push_back(static_cast<dReal>(it->pos.m[0]));
              pc->vertex.push_back(static_cast<dReal>(it->pos.m[1]));
              pc->vertex.push_back(static_cast<dReal>(it->pos.m[2]));

              pc->indices.push_back(it->index + append_indces);

              if (largest_number_indices < it->index)
              {
                  largest_number_indices = it->index;
              }

          }
      }
  

    pc->CreateTrimesh();

    pc->GeomSetQuaternion(tc->rotation);
    pc->GeomSetPosition(tc->pos + pc->collision_pos_offset);

    pc->BodySetQuaternion(tc->rotation);
    pc->BodySetPosition(tc->pos + pc->collision_pos_offset);

}

void NewPhysicsManager::GenerateTriggerBox(ODENode* phy, ODENode* trigger)
{
    std::string shape = phy->geometry;


    if (shape.compare("BOX") == 0)
    {
        trigger->box_length[0] = phy->box_length[0];
        trigger->box_length[1] = phy->box_length[1];
        trigger->box_length[2] = phy->box_length[2];

        trigger->collision_pos_offset = phy->collision_pos_offset;
    }
    else if (shape.compare("SPHERE") == 0)
    {
        trigger->box_length[0] = phy->radius * 2;
        trigger->box_length[1] = phy->radius * 2;
        trigger->box_length[2] = phy->radius * 2;

        trigger->collision_pos_offset = phy->collision_pos_offset;
    }
    else
    {
        trigger->collision_pos_offset = phy->collision_pos_offset;
        std::cout << "lazy do other shape la" << std::endl;
    }

}
