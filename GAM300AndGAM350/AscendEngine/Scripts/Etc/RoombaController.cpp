#include "pch.h"

RoombaController::RoombaController()
 : ScriptCommonIF(ASC_GETSCRIPTNAME(RoombaController)), 
    checkWaypointOffset(10.f),
    waypointIndex(0),
    revertDirection(false)
{
}

RoombaController::~RoombaController()
{
}

void RoombaController::OnStart()
{
    SetWayPoints();
    obj= ASC_FACTORY.GetGameObjectByName(targetObj);
    Handle < NewPhysicsComp> phy =obj->GetHdl<NewPhysicsComp>();

   
     ASC_LOOP("SFX_DragObject.ogg");
   // ASC_LOOP("SFX_Enemy_standard_attack_01.ogg");
    //stop the puppy from moving and being reactable 
    if (phy.IsValid())
    {
      phy->obj_phy->BodySetGravityEnable(false);
     // phy->obj_phy->
      phy->obj_phy->GeomCollisionSetEnable(false);
     // phy->obj_phy->setcoll
      //phy->obj_phy->SetHaveBody(false);
     // phy->obj_phy->BodySetKinematic();
    }
    np->obj_phy->SetHaveBody(false);
    //sizeOfWaypoint = 2;
    //listOfWaypoint.resize(sizeOfWaypoint);
    //listOfWaypointTempName.resize(sizeOfWaypoint);
    //listOfWaypoint[0] = ASC_FACTORY.GetGameObjectByName("pos1");
    //listOfWaypoint[1] = ASC_FACTORY.GetGameObjectByName("pos2");
}
void RoombaController::SetRoombaActive(bool val)
{
 // Handle < NewPhysicsComp> phy = obj->GetHdl<NewPhysicsComp>();
 // phy->obj_phy->GeomCollisionSetEnable(true);
  stop = val;
 // auto newPos = t->pos;
 // newPos.y += offSety*2;
 // obj->GetHdl<TransformComp>()->pos = newPos;
 // phy->obj_phy->BodySetLinearVel(0.0f, 0.0f, 0.0f);
 // phy->obj_phy->BodySetGravityEnable(true);
  if (stop)
  {
      goHdl->GetHdl<ParticleEmitter>()->isLooping = false;
    ASC_SOUNDMAN.Stop(s,"SFX_DragObject.ogg");
    ASC_PLAY("SFX_PowerDown.ogg");
  }
  np->obj_phy->SetRigidBodyEnable(false);
  np->obj_phy->BodySetGravityEnable(false);
  np->obj_phy->SetHaveBody(false);
  np->obj_phy->GeomCollisionSetEnable(false);

 // obj->GetHdl<TransformComp>()->dirty = true;
}
void RoombaController::Update(float dt)
{
   
    if (listOfWaypoint.size() == 0)
    {
        SetWayPoints();
    }
    // This is to check the first waypoint
    if (listOfWaypoint.size() > 0 && !stop)
    { 

   
      auto newPos = t->pos;
      newPos.y += offSety;
      obj->GetHdl<TransformComp>()->pos = newPos;

      obj->GetHdl<TransformComp>()->dirty = true;
      //obj->GetHdl<TransformComp>()->rotation
      t->dirty = true;
          //std::cout << t->pos.x << std::endl;
           // APPCONSOLE.AddLog("pos", t->pos.x);
              //// Move toward destination
              Vec3 vec3TowardTarget = listOfWaypoint[waypointIndex] - t->pos;
              //vec3TowardTarget *= speedRomba;
              //Vec3 normVec3TowardTarget = vec3TowardTarget.Normalize();
              float magnitudeSq = (vec3TowardTarget.x * vec3TowardTarget.x) + (vec3TowardTarget.y * vec3TowardTarget.y) + (vec3TowardTarget.z * vec3TowardTarget.z);
              //Vec2 vec2TowardTarget = Vec2(vec3TowardTarget.x, vec3TowardTarget.y);
              //float distance = vec2TowardTarget.Magnitude();

              // Prevent normalize divide 0
              if (magnitudeSq >= 0.5f)
              {
                  //t->rotation = Quaternion::LookAt(vec3TowardTarget);
                  Vec3 normVec3TowardTarget = Vector3(vec3TowardTarget.x / sqrtf(magnitudeSq), vec3TowardTarget.y / sqrtf(magnitudeSq), vec3TowardTarget.z / sqrtf(magnitudeSq));
                  float angle = acosf(Vec3::YAXIS.Dot(normVec3TowardTarget));



                  Vec3 myRotation = QuaternionToEuler(t->rotation);
              }

              // Prevent normalize divide 0
              if (magnitudeSq >= 0.3f)
              {
                  t->pos.x += vec3TowardTarget.x / sqrtf(magnitudeSq) * dt * speedRomba;
                  t->pos.z += vec3TowardTarget.z / sqrtf(magnitudeSq) * dt * speedRomba;
                  //std::cout << t->pos.x << " " << t->pos.z << std::endl;
              }
              if (changeDirection)
              {
                changeDirection = false;
             
                  if (revertDirection)
                  {
                    --waypointIndex;

                    if (waypointIndex < 0)
                    {
                      waypointIndex = listOfWaypoint.size() - 1;
                    }
                  }
                  else
                  {
                    ++waypointIndex;

                    // Exceed size
                    if (waypointIndex >= listOfWaypoint.size())
                    {
                      waypointIndex = 0;
                    }
                  }
              }

              // Reach destination
              if (((t->pos.x >= (listOfWaypoint[waypointIndex].x - checkWaypointOffset)) &&
                  (t->pos.x <= (listOfWaypoint[waypointIndex].x + checkWaypointOffset))) &&
                  ((t->pos.z >= (listOfWaypoint[waypointIndex].z - checkWaypointOffset)) &&
                  (t->pos.z <= (listOfWaypoint[waypointIndex].z + checkWaypointOffset))) 
                )
              {
                ASC_PLAY("SFX_Player_left_01.ogg");
                //ASC_PLAY("SFX_Enemy_standard_attack_01.ogg");

                
                  // Move to next waypoint
                  // If the next waypoint not exist, move to next waypoint
                      if (revertDirection)
                      {
                          --waypointIndex;

                          if (waypointIndex < 0)
                          {
                              waypointIndex = static_cast<int>(listOfWaypoint.size()) - 1;
                          }
                      }
                      else
                      {
                          ++waypointIndex;

                          // Exceed size
                          if (waypointIndex >= static_cast<int>(listOfWaypoint.size()))
                          {
                              waypointIndex = 0;
                          }
                      }

              }
        
    }
    //std::cout << waypointIndex;
}

void RoombaController::OnDestroy()
{

}
void RoombaController::SetWayPoints()
{
    listOfWaypoint.clear();
    Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, goHdl);

    for (int i = 0; i < w->waypoints.size(); i++)
    {
        listOfWaypoint.push_back(w->waypoints[i]);
    }

}
void RoombaController::Serialize(Serializer& sr, LogicHandle idx)
{

  sr.Write("Waypointsize", static_cast<int>(listOfWaypoint.size()));
  sr.Write("targetName", targetObj);
  sr.Write("speed",speedRomba );

}
void RoombaController::Deserialize(Deserializer& d)
{
    //std::vector<std::string> names;
    d.Read("wayPoints", listOfWaypointTempName);
    d.Read("Waypointsize", sizeOfWaypoint);
    d.Read("targetName", targetObj);
    d.Read("speed", speedRomba);
   // SetWayPoints();
    //listOfWaypoint.resize(sizeOfWaypoint);
   // listOfWaypointTempName.resize(sizeOfWaypoint);

  
}
void RoombaController::Reflect(const Reflector& r)
{
    //int tmpWaypointSize = sizeOfWaypoint;
    r.Reflect("targetName", targetObj);
    r.Reflect("Speed", speedRomba);
    if(ImGui::Button("Get WayPoints", ImVec2(300, 30)))
    {
        SetWayPoints();
    }
 
}

void RoombaController::OnCollisionEnter(GOHandle other)
{
  PlayerController* player = ASC_GETSCRIPT_PTR(PlayerController, other);
  
    //hit the player 
    if(player != nullptr)
    {
      revertDirection = !revertDirection;
      changeDirection = true;
    }  
    //interact behaviour 

  
}
void RoombaController::OnCollisionStay(GOHandle other)
{

}
void RoombaController::OnCollisionExit(GOHandle other)
{

}

void RoombaController::HandleMessage(MsgData_Base* m)
{

}