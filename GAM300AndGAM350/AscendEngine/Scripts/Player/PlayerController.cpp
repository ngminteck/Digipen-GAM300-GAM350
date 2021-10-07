/******************************************************************************/
/*!
\file   PlayerController.cpp
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

//#define SERIAL_TYPE "Type"
#define DOG_MOVEMENTSPEED "moveSpeed"
#define CLIMBING_SPEED "climbingSpeed"
#define HOVER_HEIGHT "hoverHeight"
#define BOOST_HEIGHT "boostHeight"
#define ASC_SERIAL_POS "Position"
#define ASC_SERIAL_CAM_SCALE_FWD "CameraFwdScale"
#define ASC_SERIAL_CAM_SCALE_UP "CameraUpScale"
#define ASC_SERIAL_MOUSE_SENSITIVITY "MouseSensitivity"
#define ASC_SERIAL_THRUST_MAX "Thrust_Max"
#define ASC_SERIAL_THRUST_HEIGHT "Thrust_Height"
#define ASC_SERIAL_THRUST_ACCEL "Thrust_Accl"
#define ASC_SERIAL_THRUST_DAMP "Thrust_Damping"
#define ASC_CAM_MAX_LERP 2.0f


bool PlayerController::Distancecheck(Vec3 & force)
{
  Vec3 curr_force = t->pos - ref_point;
  curr_force.y = 0;
  float curr_dist = curr_force.MagnitudeSq();

  Vec3 next_force = ((t->pos + force) - ref_point);
  next_force.y = 0;
  float next_distance = next_force.MagnitudeSq();
  
  if (curr_dist > max_fly_distance)
  {
    if (next_distance > curr_dist)
    {
      return false;
    }
  }
  return true;
}

void PlayerController::DealDamage(float dmg)
{
  health -= dmg;
}

PlayerController::PlayerController() :
  ScriptCommonIF(ASC_GETSCRIPTNAME(PlayerController)), 
  player_cam(ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM))
{
  //ASC_SCRIPT_REGISTER_MSG(mTYPE_PLAYER_CREATED_WEAPON, MsgChangeWeapon);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PLAYER_ENABLE_CONTROLS, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PLAYER_DISABLE_CONTROLS, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PLAYER_DISABLE_CAM, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PLAYER_ENABLE_CAM, HandleMessage);

  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_STARTCUTSCENE, HandleMessage);
  ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_ENDCUTSCENE, HandleMessage);
}


//Vec3 PlayerController::GetPlayerCamPos() const
//{
//  return newCamPos;
//}

//int PlayerController::GetCamHandle() const
//{
//  return cam_handle;
//}

Vec2 PlayerController::GetMouseDelta() const
{
  return mDelta;
}

void PlayerController::OnStart()
{
  // index 0 not suppose to be called 
    if (ASC_ENGINE.GetUsingMouse())
    {
        promptTextures.push_back("");
        promptTextures.push_back("UI_MouseClickLeft_albedo");
        promptTextures.push_back("UI_MouseClickLeft_Interact_albedo");
        promptTextures.push_back("UI_MouseClickLeft_move_albedo");
        promptTextures.push_back("UI_MouseClickLeft_Pickup_albedo");
        promptTextures.push_back("UI_MouseClickLeft_Interact_albedo");
        promptTextures.push_back("UI_MouseClickRight_albedo");
        promptTextures.push_back("UI_MouseClickRight_Origin_albedo");
        promptTextures.push_back("UI_MouseClickRight_Trails_albedo");
    }
    else
    {
        promptTextures.push_back("");
        promptTextures.push_back("UI_Controller_LeftClick_albedo");
        promptTextures.push_back("UI_Controller_Interact_albedo");
        promptTextures.push_back("UI_Controller_move_albedo");
        promptTextures.push_back("UI_Controller_Pickup_albedo");
        promptTextures.push_back("UI_Controller_Interact_albedo");
        promptTextures.push_back("UI_Controller_RightClick_albedo");
        promptTextures.push_back("UI_Controller_Origin_albedo");
        promptTextures.push_back("UI_Controller_Trails_albedo");
    }


 /* auto boxLenY = np->obj_phy->box_length[1];
  raystartOffset.y -= boxLenY;*/

 // floorRayLength = 10.0f;

  isCutScene = true;
  canMove = true;
  freezeCam = false;
  holdingItem = false;
  heldItemoffset = Vec3(0, -10.0f, -20.0f);
  //MUST HAVE
  ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);

  ASC_INPUT.CentreMouse();
  mousepos_old = ASC_INPUT.GetMousePos();

  player_cam = ASC_GMAN.GetCamera( GraphicsManager::CameraIdx::PLAYER_CAM);
  
  Vec3 look = player_cam.GetPos() + playerSpawnForward;
  player_cam.LookAt(look);
  defaultFOV = 60;
  player_cam.SetFOV(90);
  np->obj_phy->BodySetLinearDamping(0.2);

  np->InitFloorRay(t->pos, -Vec3::YAXIS, hoverHeight);
  

  np->InitForwardRay(player_cam.GetPos(), player_cam.GetForward(), 200);

  np->obj_phy->hack_always_able_raycast = true;

  //(HB)from now, levelman will handle all camera
  //player_cam.SetPosition(Vec3(0,100,0));
  //player_cam.LookAt(t->pos + t->);
  //t->pos.y = 100;
  //oldHeight = t->pos.y;
  if (StartPos != Vec3::ZERO)
  {
      t->pos = StartPos;
      oldHeight = t->pos.y;
  }

  prompt = ASC_FACTORY.GetGameObjectByName("promptUI");
  if (!prompt.IsValid())
  {
      prompt = ASC_FACTORY.CreateGOfromPB("promptUI");
  }
  UIMission = ASC_FACTORY.GetGameObjectByName("UI_MissionList");
  if (!UIMission.IsValid())
    UIMission = ASC_FACTORY.GetGameObjectByName("UI_MissionList");
  //ASC_SOUNDMAN.Play(s->GetSelfHandle(), "abc.mp3");
  //ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "Drone_Engine_3.ogg");

 // ASC_PLAY
  //HUD_overall = ASC_FACTORY.CreateGOfromPB("HUD_Overall");
 // ref_point = goHdl->GetHdl<TransformComp>()->pos;
 // ref_point.y = 0;
#ifdef _SUBMISSION
  //GraphicsComp* gc;
  //GetComponent<GraphicsComp>(gc);
  //gc->SetTransparent(0.f);
#endif


  //PauseMenu
  auto& PauseMenu = LoadFromPrefab("PauseBackground");
  if (!PauseMenu.IsValid()) 
      return;
  else
  {
      PauseMenu->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& Paused_BG = ASC_FACTORY.GetGameObjectByName("PausedBG_Blur");
      Paused_BG->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& PauseScreen = ASC_FACTORY.GetGameObjectByName("PauseMenu");
      PauseScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& Resume = ASC_FACTORY.GetGameObjectByName("ResumeButton");
      Resume->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& HowToPlay = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
      HowToPlay->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& Quit = ASC_FACTORY.GetGameObjectByName("QuitButton");
      Quit->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      auto& Selector = ASC_FACTORY.GetGameObjectByName("PausePawSelector");
      Selector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  }



}

int PlayerController::MoveCamToLocation(const Vec3& start, const Vec3& dest, float dt )
{
    ///
    //Return 1 when on the way
    //Return 0 when reached
    if (start != Vec3::ZERO && dest != Vec3::ZERO)
    {
        canMove = false;
        Vec3 distToMove = ((dest - start) / 2.0f);
        if ((dest - player_cam.GetPos()).MagnitudeSq() > 10.0f)
        {
            player_cam.SetPosition(player_cam.GetPos() + (distToMove*dt));
            return 1;
        }
        else
        {
            canMove = true;
            return 0;
        }


    }

}
void PlayerController::Update(float dt)
{

       // dt *= ASC_NEWPHYSICSMANAGER.fps_limit / dt;

#ifdef _RELEASE || _DEBUG
        if (ASC_KEYTRIGGERED(ASC_KEY_F1))
        {
            if (ASC_GMAN.GetCameraIdx() == GraphicsManager::CameraIdx::PLAYER_CAM)
            {
                ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::EDITOR_CAM);
                //canMove = false;
            }
            else
            {
                ASC_GMAN.SetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
                //canMove = true; 
            }
        }
        if (ASC_KEYTRIGGERED(ASC_KEY_F2))
        {
            canMove = !canMove;
        }
#endif
        if (!canMove)
        {
            if (ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalk.ogg"))
                ASC_SOUNDMAN.Stop(s, "SFX_DogWalk.ogg");
            if (ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalkSlow.ogg"))
                ASC_SOUNDMAN.Stop(s, "SFX_DogWalkSlow.ogg");
        }
        if (freezeCam && !canMove) return;
        if (!freezeCam)
        {
            Vector3 finalForce;
            finalForce = HandleKeyboard(player_cam, dt);

            //if(finalForce == Vec3::ZERO)
            finalForce = HandleController(player_cam, dt);

            //  if (finalForce != Vec3::ZERO) thrust_curr += thrust_accl * dt;
             // thrust_curr = ASC_RANGE(thrust_curr, 0, thrust_max);
            finalForce = finalForce.Normalize();// * thrust_curr;
            //finalForce = Vec3(finalForce.z, finalForce.x, finalForce.y);
            if (canMove)
            {
                if (np.IsValid())
                {

                    dVector3 gravity;
                    dWorldGetGravity(ASC_NEWPHYSICSMANAGER.world, gravity);

                }

            }


        }
        else mDelta = Vec2();
#ifdef _SUBMISSION
        if (canMove)
        {
            if (!movingItem)
            {
                currDelta += mDelta * 7.0f * dt;
                currDelta += (Vec2::ZERO - currDelta) * 10.0f * dt;
                //moving the camera
                player_cam.RotateForward(currDelta.y, currDelta.x);
            }
        }
#else
        if (canMove && !ASC_EDITOR.showUI)
        {
            if (!movingItem)
            {
                currDelta += mDelta * 7.0f * dt;
                currDelta += (Vec2::ZERO - currDelta) * 10.0f * dt;
                //moving the camera
                player_cam.RotateForward(currDelta.y, currDelta.x);
            }
        }
#endif
        if (Swivel)
        {
            currDelta += mDelta * 7.0f * dt;
            currDelta += (Vec2::ZERO - currDelta) * 10.0f * dt;

            currDelta.x = ASC_RANGE(currDelta.x, -ASC_CAM_MAX_LERP, ASC_CAM_MAX_LERP);
            //moving the camera
            player_cam.RotateForward(currDelta.y, currDelta.x);
        }

        //Interact Block
        if (!holdingItem && !movingItem)
        {

            float time = 0.0f;
            dContact info;
            //GOHandle interact = PhysicsManager::RayCast(player_cam.GetPos(), player_cam.GetForward(), time); //ASC_NEWPHYSICSMANAGER.RayCast(player_cam.GetPos(),  player_cam.GetForward(), t->rotation, t->GetGOHandle()->name, 30.0f);


            GOHandle interact = np->RayCast(player_cam.GetPos() + (player_cam.GetForward() * np->obj_phy->box_length[0]), player_cam.GetForward(), info, 200);
            if (interact.IsValid())
            {
            }
            if (interact.IsValid() && interact->GetHdl<LogicComp>().IsValid())
            {
                Vec3 dist;
                range = Vec3(t->pos.x - info.geom.pos[0], t->pos.y - info.geom.pos[1], t->pos.z - info.geom.pos[2]).MagnitudeSq();
                Interact* sc = ASC_GETSCRIPT_INTERACT(interact);
                if (sc != nullptr && sc->activated && (sc->range * sc->range) >= range)
                {

                std::cout << interact->name << std::endl;
                    interact->GetHdl<GraphicsComp>()->isHighlighted = true;
                    if (sc->prompt != 0)
                    {
                      prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = promptTextures[sc->prompt];
                    }
                   /* switch (sc->prompt)
                    {

                    case 1:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_albedo";
                        break;
                    case 2:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_Interact_albedo";
                        break;
                    case 3:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_move_albedo";
                        break;
                    case 4:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_Pickup_albedo";
                        break;
                    case 5:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickLeft_Swivel__albedo";
                        break;
                    case 6:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickRight_albedo";
                        break;
                    case 7:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickRight_Origin_albedo";
                        break;
                    case 8:
                        prompt->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_MouseClickRight_Trails_albedo";
                        break;
                    default:
                        break;
                    }  */
                    if(sc->prompt)
                    prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(1.0f);
                    
					DoorInteract* doorLock = ASC_GETSCRIPT_PTR(DoorInteract, interact);
					if (doorLock != nullptr && doorLock->lock && (doorLock->range * doorLock->range) >= range)
					{
						ASC_GMAN.m_highlightCol = Color::RED.xyz;
					}
					else if (sc->usedMe)
					{
						ASC_GMAN.m_highlightCol = Color::BLUE.xyz;
					}
					else
					{
						ASC_GMAN.m_highlightCol = Color::YELLOW.xyz;
					}

                    if (interact != lastInteract)
                    {
                        if (lastInteract.IsValid())
                            lastInteract->GetHdl<GraphicsComp>()->isHighlighted = false;
                        lastInteract = interact;
                    }
                }
            }
            else
            {
                if (lastInteract.IsValid())
                {

                    lastInteract->GetHdl<GraphicsComp>()->isHighlighted = false;
                    lastInteract.AssignNewInValid();
                    prompt->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                    //range = 0.0f;

					ASC_GMAN.m_highlightCol = Color::YELLOW.xyz;
                }
            }
        }
        //Holding Item
        else
        {
            if (lastInteract.IsValid())
            {
                if (holdingItem)
                {
                    lastInteract->GetHdl<TransformComp>()->pos = t->pos + heldItemoffset;
                    PickupInteract* pi = ASC_GETSCRIPT_PTR(PickupInteract, lastInteract);
                    //lastInteract->GetHdl<TransformComp>()->rotation = EulerToQuaternion(QuaternionToEuler(t->rotation));
                    lastInteract->GetHdl<TransformComp>()->rotation = t->rotation;
                    lastInteract->GetHdl<TransformComp>()->rotation *= EulerToQuaternion(pi->GrabRot);
                }
				if (holdingItem || movingItem)
				{
					ASC_GMAN.m_highlightCol = Color::BLUE.xyz;
				}
				else
				{
					ASC_GMAN.m_highlightCol = Color::YELLOW.xyz;
				}
            }
        }

    

}

void PlayerController::Serialize(Serializer &sr, LogicHandle idx)
{
  //tinyxml2::XMLElement& elem = sr.m_xmlElem;
  //elem.SetAttribute(ASC_SERIAL_NAME, GetName().data());
  sr.Write(GetName());
  sr.Write(ASC_SERIAL_CAM_SCALE_FWD, cam_scale_fwd);
  sr.Write(ASC_SERIAL_CAM_SCALE_UP, cam_scale_up);
  sr.Write(ASC_SERIAL_MOUSE_SENSITIVITY, mouse_sensitivity);
  sr.Write(ASC_SERIAL_THRUST_ACCEL, thrust_accl);
  sr.Write(ASC_SERIAL_THRUST_DAMP, thrust_damping);
  sr.Write(ASC_SERIAL_THRUST_MAX, thrust_max);
  sr.Write(DOG_MOVEMENTSPEED, moveSpeed);
  sr.Write(CLIMBING_SPEED, climbingSpeed);
  sr.Write(HOVER_HEIGHT, hoverHeight);
  sr.Write(ASC_SERIAL_THRUST_HEIGHT, boostHeight);

  sr.Write("SpawnForward", playerSpawnForward);
  sr.Write("rayFloorLen", floorRayLength);
  sr.Write("rayOffSet", raystartOffset);
  sr.Write("addtionalGravForce", additionalGravForce);
  sr.Write("limitToStartApplying", limitToStartApplying);
  sr.Write("StartPos", StartPos);

}

void PlayerController::Deserialize(Deserializer &d)
{
  d.Read(ASC_SERIAL_CAM_SCALE_FWD, cam_scale_fwd);
  d.Read(ASC_SERIAL_CAM_SCALE_UP, cam_scale_up);
  d.Read(ASC_SERIAL_MOUSE_SENSITIVITY, mouse_sensitivity);
  d.Read(ASC_SERIAL_THRUST_ACCEL, thrust_accl);
  d.Read(ASC_SERIAL_THRUST_DAMP, thrust_damping);
  d.Read(ASC_SERIAL_THRUST_MAX, thrust_max);
  d.Read(DOG_MOVEMENTSPEED, moveSpeed);
  d.Read(CLIMBING_SPEED, climbingSpeed);
  d.Read(HOVER_HEIGHT, hoverHeight);
  d.Read(ASC_SERIAL_THRUST_HEIGHT, boostHeight);
  d.Read("SpawnForward", playerSpawnForward);
  d.Read("rayFloorLen", floorRayLength);
  d.Read("rayOffSet", raystartOffset);
  d.Read("addtionalGravForce", additionalGravForce);
  d.Read("limitToStartApplying", limitToStartApplying);
  d.Read("StartPos", StartPos);
}

void PlayerController::Reflect(const Reflector& r)
{
  ImGui::Text("Additional Grav");

  r.Reflect("limitToStartApplying",limitToStartApplying);

  r.Reflect("addtionalGravForce", additionalGravForce);
  ImGui::Text("Ray Related");
  r.Reflect("rayStartOffset", raystartOffset);
  r.Reflect("rayfloorRayLength", floorRayLength);
  ImGui::Separator();
  const float dragSpeed = 0.02f;
  ImGui::PushID(10);
  //r.Reflect("Name", GetName());
  //ImGui::DragFloat(ASC_SERIAL_CAM_SCALE_FWD, &cam_scale_fwd, dragSpeed, 0.0f, 10.0f, "%.1f");
  //ImGui::DragFloat(ASC_SERIAL_CAM_SCALE_UP, &cam_scale_up, dragSpeed, 0.0f, 10.0f, "%.1f");
  //ImGui::DragFloat(ASC_SERIAL_MOUSE_SENSITIVITY, &mouse_sensitivity, dragSpeed, 0.0f, 10.0f, "%.1f");
  //ImGui::DragFloat(ASC_SERIAL_THRUST_ACCEL, &thrust_accl);
  //ImGui::DragFloat(ASC_SERIAL_THRUST_DAMP, &thrust_damping);
  //ImGui::DragFloat(ASC_SERIAL_THRUST_MAX, &thrust_max);
  //ImGui::DragFloat()
  ImGui::DragFloat(BOOST_HEIGHT, &boostHeight);
  ImGui::DragFloat(DOG_MOVEMENTSPEED, &moveSpeed);
  //ImGui::DragFloat(CLIMBING_SPEED, &climbingSpeed);
  ImGui::DragFloat(HOVER_HEIGHT, &hoverHeight);
  ImGui::DragFloat(ASC_SERIAL_THRUST_HEIGHT, &boostHeight);
  ImGui::Checkbox("IsGrounded", &isGrounded);
  ImGui::PopID();

  if (ImGui::Button("Set Camera Spawn Direction", ImVec2(200, 30)))
  {
      playerSpawnForward = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::EDITOR_CAM).GetForward();
  }
  if (ImGui::Button("Set Starting Position", ImVec2(200, 30)))
  {
      StartPos = t->pos;
  }
}

void PlayerController::OnDestroy()
{
  //ASC_GMAN.RemoveCamera(cam_handle);
  //Log("PlayerController::OnDestroy");
}

void PlayerController::OnCollisionEnter(GOHandle other)
{
  if (other->GetType() == GOTYPE::go_wallsegment)
  {
    ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), "SFX_Player_collide_wall_01.ogg");
  }
  //Log("PlayerController::OnCollisionEnter");
  //Log(ASC_FACTORY.GetObjectPtr(other)->name);

  //EnemyGround* ptr;
  //ASC_GETSCRIPT(EnemyGround, ptr, other);
}

void PlayerController::OnCollisionStay(GOHandle other)
{
  //Log("PlayerController::OnCollisionStay");

}

void PlayerController::OnCollisionExit(GOHandle other)
{
  //Log("PlayerController::OnCollisionExit");
}

void PlayerController::HandleMessage(MsgData_Base *m)
{
  switch (m->type)
  {
  case mTYPE_GAME_PLAYER_ENABLE_CONTROLS:
  {
    canMove = true;
  } break;

  case mTYPE_GAME_PLAYER_DISABLE_CONTROLS:
  {
    canMove = false;
   // p->Stop();
  } break;
  
  case mTYPE_GAME_ENDCUTSCENE:
  {
    Vec3 forward = player_cam.GetForward();
    Vec3 right = player_cam.GetRight();
    t->rotation = QuaternionFromMatrix(Mat4::LookAt({}, -forward, -right));

    Log("Resume Momement");
    isCutScene = false;
  }
  case mTYPE_GAME_PLAYER_ENABLE_CAM:
  {
    freezeCam = false;
    canMove = true;
  } break;

  case mTYPE_GAME_STARTCUTSCENE:
    Log("Stop Momement");
    isCutScene = true;
  case mTYPE_GAME_PLAYER_DISABLE_CAM:
  {
    freezeCam = true;
    canMove = false;
  } break;

  default: break;
  }
}

void PlayerController::SetCutsceneRot()
{
    GOHandle pos = ASC_FACTORY.GetGameObjectByName("Dogshit");
    Vec3 forward = player_cam.GetForward();
    Vec3 right = player_cam.GetRight();  
    
    Vec3 Lok = QuaternionToEuler(t->rotation);
    Lok.x = Lok.z = 0;
    // Vec3 offset = g->posOffset;
   //  offset = offset.VecQuatProduct(diff);
    pos->GetHdl<TransformComp>()->rotation = EulerToQuaternion(Lok);
    pos->GetHdl<TransformComp>()->pos = t->pos -Vec3(0, hoverHeight,0);
    t->dirty = true;
   

}
Vector3 PlayerController::HandleKeyboard(Camera& cam, float dt)
{
    mDelta = (ASC_INPUT.GetMousePos() - mousepos_old) * mouse_sensitivity* -dt;

  Vec3 forward = cam.GetForward();
  Vec3 right = cam.GetRight();
  bool moving = false;

  ///
  Quaternion q = QuaternionFromMatrix(Mat4::LookAt(Vec3(), forward, right));
  
  Quaternion quat = t->rotation;
  Quaternion diff = q * quat.Inverse();
      if (g.IsValid())
      {
          Vec3 Lok = QuaternionToEuler(diff);
           Vec3 offset = g->posOffset;
          offset = offset.VecQuatProduct(diff);
          g->posOffset = offset;// Set object locations 
      }
      // }
      heldItemoffset = heldItemoffset.VecQuatProduct(diff);
      t->rotation = q;

  
  
 
      finalForce = Vec3::ZERO;
      if (ASC_KEYDOWN(ASC_KEY_W) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_UP))
      {

          if (movingItem)
          {
              if (push)
              {
                  moving = true;
                  finalForce += Vec3(forward.x, 0.0f, forward.z) *ASC_NEWPHYSICSMANAGER.fps_limit / dt;
              }
          }
          else
          {
              moving = true;
              finalForce += Vec3(forward.x, 0.0f, forward.z) *ASC_NEWPHYSICSMANAGER.fps_limit / dt;
          }
      }
      else if (ASC_KEYDOWN(ASC_KEY_S) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_DOWN))
      {
          if (movingItem)
          {
              if (pull)
              {
                  finalForce -= Vec3(forward.x, 0.0f, forward.z)  *ASC_NEWPHYSICSMANAGER.fps_limit / dt;
                  moving = true;
              }
          }
          else
          {
              moving = true;
              finalForce -= Vec3(forward.x, 0.0f, forward.z) * ASC_NEWPHYSICSMANAGER.fps_limit / dt;
          }

      }
      if (!movingItem)
      {
          if (ASC_KEYDOWN(ASC_KEY_A) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_LEFT))
          {
              //  t->pos.y = t->pos.y + (sinf(runTime) * 0.01f);
              finalForce -= right * ASC_NEWPHYSICSMANAGER.fps_limit / dt;
              // np->BodyAddForce(Vec3(-right.x * moveSpeed, -right.y * moveSpeed, -right.z * moveSpeed));

              moving = true;
              // yForce += Vec3::YAXIS * 0.05f * dt;
          }
          else if (ASC_KEYDOWN(ASC_KEY_D) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_RIGHT))
          {
              //  t->pos.y = t->pos.y + (sinf(runTime) * 0.01f);
              finalForce += right * ASC_NEWPHYSICSMANAGER.fps_limit / dt;
              // np->BodyAddForce(Vec3(right.x*moveSpeed, right.y * moveSpeed, right.z * moveSpeed));

              moving = true;
              //  yForce -= Vec3::YAXIS * 0.05f * dt;
          }
          
      }

      if (ASC_KEYTRIGGERED(ASC_KEY_U))
      {
          if (player_cam.GetFOV() == defaultFOV)
          {
              player_cam.SetFOV(90);
          }
          else
          {
              player_cam.SetFOV(defaultFOV);
          }
      }
  
  if (IsInteractButtonPress())
  {

      if (lastInteract.IsValid() && this->isGrounded  )
      {
          Interact* interactScript = ASC_GETSCRIPT_INTERACT(lastInteract);
          if (interactScript != nullptr)
          {
            float iRange = 0.0f;
            if (interactScript->range == 0.0f)
              iRange = 20.0f;
            else
              iRange = interactScript->range;
            if (range < (iRange * iRange) && !holdingItem && !movingItem)
            {
              if (interactScript->activated)
                interactScript->InteractAction();
              //disable LastInteract physics 
            }
            else if (holdingItem || movingItem)
            {
              interactScript->InteractAction();
            }
          }
      }
  }



      //adjusting player height in accordance to terrain height
      float timeHeight = 0;
      GOHandle gHdl;

      dContact infolf;
     // std::cout << np->obj_phy->box_length[1] * t->scale.y << std::endl;
   
      auto offsetCast = t->pos + raystartOffset;
   /*   auto offsetCast2 = t->pos + raystartOffset; 
      offsetCast2.x = 0; offsetCast2.z = 0;*/
     // offsetCast.y -= boxLenY / 2; 

      //if(isGrounded)
      gHdl = np->RayCastFloor( offsetCast, -Vec3::YAXIS, infolf, floorRayLength);

      //dContact inforf;
      //gHdl = np->RayCastRightFront(t->pos, -Vec3::YAXIS, inforf, 900);
      //
      //dContact infolb;
      //gHdl = np->RayCastLeftBack(t->pos, -Vec3::YAXIS, infolb, 900);
      //
      //dContact inforb;
      //gHdl = np->RayCastRightBack(t->pos, -Vec3::YAXIS, inforb, 900);
      //
      timeHeight =t->pos.y - static_cast<float>(infolf.geom.pos[1]);
      //std::cout << t->pos.y << " " << infolf.geom.pos[1] << std::endl;
      //if (gHdl.IsValid())
      //{
      //    std::cout << gHdl->name << std::endl;
      //    std::cout << t->pos.x << "    " << t->pos.z << std::endl;
      //    std::cout << "pos:x" << info.geom.pos[0] << "pos:y" << info.geom.pos[1] << "pos:z" << info.geom.pos[2] << std::endl;
      //    std::cout << "normal:x" << info.geom.normal[0] << " normal:y" << info.geom.normal[1] << " normal:z" << info.geom.normal[2] << std::endl;
      //}
     // np->RayCast(t->pos, -Vec3::YAXIS, info, hoverHeight)
      // gHdl = PhysicsManager::RayCast(t->pos, -Vec3::YAXIS, timeHeight);// ASC_NEWPHYSICSMANAGER.RayCast(t->pos, -Vec3::YAXIS, t->rotation, t->GetGOHandle()->name, 30.0f);

        //hovering 
      if (gHdl.IsValid())
      {
       
       // else
        {
         // std::cout << gHdl->name << " only \n";
        }
        // std::cout << gHdl->name << std::endl; 
          Vec3 rayPoint = t->pos  +(-Vec3::YAXIS * timeHeight);

          if ((timeHeight - hoverHeight) > 20)
          {
              isGrounded = false;
              playOnce = true;
          }
          else
          {
              if (playOnce)
              {
                  ASC_PLAY("SFX_Land.ogg");
                  playOnce = false;
              }

          }
          float newHeight = hoverHeight + rayPoint.y;
         
              if (newHeight < oldHeight)
                  climbingSpeed = 0.07f;
              else if((t->pos.y - rayPoint.y) < hoverHeight)
              {

                  if (fabs(finalForce.x) > 0.0f || fabs(finalForce.z) > 0.0f)
                      climbingSpeed = 0.1f;
                  if(climbingSpeed!= 0.1f)
                     climbingSpeed = 0.006f;

              }
              if (timeHeight - hoverHeight < 0.1f)
              {
                  //ASC_PLAY("SFX_Land.ogg");
                  isGrounded = true;
              }
              if (isGrounded)
              {
                  t->pos.y = oldHeight + (newHeight - oldHeight) * climbingSpeed;
                  np->obj_phy->BodySetLinearDamping(0.5f);
              }

            
          oldHeight = t->pos.y;
      }
      else
      {
          isGrounded = false;
      }

      //tween player ht(hovering)

          // double the falling power if at the high of your jump.
      if (np->obj_phy->BodyGetLinearVel().y <= limitToStartApplying && !isGrounded)
      {
       // std::cout << np->obj_phy->BodyGetLinearVel().y << std::endl;
        /*   dVector3 grav = { 0 };
      /*      dWorldGetGravity(ASC_NEWPHYSICSMANAGER.world, grav);*/
        auto val = np->obj_phy->BodyGetLinearVel();
         np->obj_phy->BodyAddForce(Vec3(0, additionalGravForce, 0));
        /* val.y *= 2.0f;
         np->obj_phy->BodySetLinearVel(val);*/
      }
//      t->pos.y = t->pos.y + (sinf(runTime) * 0.01f);
  if (IsJumpButtonPress() && isGrounded && !movingItem)
  {
      ASC_PLAY("SFX_Jump.ogg");
      isGrounded = false;
      finalForce += (Vec3::YAXIS + Vec3::YAXIS) * ASC_NEWPHYSICSMANAGER.fps_limit / dt;
      np->obj_phy->BodySetLinearDamping(0.0f);
      climbingSpeed = 0.0f;
  }
  if (finalForce.MagnitudeSq() > 0 && canMove)
  {
      np->obj_phy->SetHaveBody(true);
      np->obj_phy->SetRigidBodyEnable(true);
      if (isGrounded)
      {

          float tempSpeed = movingItem ? moveSpeed * 0.3f : moveSpeed;

          np->obj_phy->BodyAddForce(Vec3(finalForce.x * tempSpeed, finalForce.y * boostHeight, finalForce.z * tempSpeed));
          //Bob the head here
          float bobFreq = movingItem ? 1.0f : 3.0f;

          float y = t->pos.y + (sinf(2*ASC_PI*bobFreq*runTime) * 0.5f);
          Vec3 pos = player_cam.GetPos();
          pos.y = y;
          pos.x = t->pos.x;
          pos.z = t->pos.z;
          player_cam.SetPosition(pos);
      }
     //is not grounded
      else
      {
          float y = t->pos.y + (sinf(runTime) * 0.01f);
          np->obj_phy->BodySetLinearDamping(0.0f);
          Vec3 pos = player_cam.GetPos();
          pos.y = y;
          pos.x = t->pos.x;
          pos.z = t->pos.z;
          player_cam.SetPosition(pos);
          // this is to reduce speed in the air 
          np->obj_phy->BodyAddForce(Vec3(finalForce.x * (moveSpeed * 0.01f), finalForce.y * boostHeight, finalForce.z * (moveSpeed * 0.01f)));
        

      }
      


  }
  else
  {
      player_cam.SetPosition(t->pos);
  }


  if ((ASC_KEYDOWN(ASC_KEY_A) || ASC_KEYDOWN(ASC_KEY_W) || ASC_KEYDOWN(ASC_KEY_S) || ASC_KEYDOWN(ASC_KEY_D)||
    (ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_UP) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_DOWN) ||
      ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_LEFT) || ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::DPAD_RIGHT)))
      
    && isGrounded && canMove)
  {
      if (!movingItem)
      {
          if (!ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalk.ogg"))
              ASC_LOOP("SFX_DogWalk.ogg");

      }
      else
      {
          if (!ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalkSlow.ogg"))
              ASC_LOOP("SFX_DogWalkSlow.ogg");
      }
  }
  else
  {
      if(ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalk.ogg"))
      ASC_SOUNDMAN.Stop(s, "SFX_DogWalk.ogg");
      if(ASC_SOUNDMAN.IsPlaying(s, "SFX_DogWalkSlow.ogg"))
      ASC_SOUNDMAN.Stop(s, "SFX_DogWalkSlow.ogg");
  }
#ifdef _SUBMISSION
  ASC_INPUT.CentreMouse();
  mousepos_old = ASC_INPUT.GetMousePos();
#else
  if (!ASC_EDITOR.showUI)
  {
      ASC_INPUT.CentreMouse();
      mousepos_old = ASC_INPUT.GetMousePos();
  }
#endif
  // Show Quest
 /* if (ASC_KEYTRIGGERED(ASC_KEY_Q))
  {
      if (!isQuestShowing)
      {
          isQuestShowing = true;
          APPCONSOLE.AddLog("Showing quest");
          GOHandle& Quest = ASC_FACTORY.GetGameObjectByName("Quest");
          if (!Quest.IsValid())
              return finalForce;
          else
              Quest->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
      }
      else
      {
          isQuestShowing = false;
          APPCONSOLE.AddLog("NOT Showing quest");
          GOHandle& Quest = ASC_FACTORY.GetGameObjectByName("Quest");
          if (!Quest.IsValid())
              return finalForce;
          else
              Quest->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
      }
  }*/

  if (IsMissionListButtonPressed())
  {
    ASC_MSGSYS.SendMsg(Msg_Logic_Mission(mTYPE_PLAYER_PRESS_MISSIONLIST, GetGOHandle()));
  }


  return finalForce;
}

Vector3 PlayerController::HandleController(Camera& cam, float dt)
{
  Vec3 forward = cam.GetForward();
  Vec3 right = cam.GetRight();

  Vec3 finalForce;

  if (!ASC_CONTROLLER1.LStick_InDeadzone())
  {
    float x = ASC_CONTROLLER1.LeftStick_X();
    finalForce += right * x;
    yForce += Vec3::YAXIS * 0.05f * (float)ASC_SIGN(x) * dt;

    float y = ASC_CONTROLLER1.LeftStick_Y();
    finalForce += forward * y;
  }

  if (!ASC_CONTROLLER1.RStick_InDeadzone())
  {
    float x = ASC_CONTROLLER1.RightStick_X();
    mDelta.x += -x * 120.0f * dt;

    float y = ASC_CONTROLLER1.RightStick_Y();
    mDelta.y += y * 60.0f * dt;
  }

  if (ASC_CONTROLLER1.LeftTrigger())
  {
	  mDelta.x += 1 * 120.0f * dt;
  }

  if (ASC_CONTROLLER1.RightTrigger())
  {
	  mDelta.x += -1 * 120.0f * dt;
  }

  if (ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::L_SHOULDER) 
    /*|| ASC_CONTROLLER1.GetButtonDown(GAMEPAD_INPUT::R_SHOULDER)*/)
    thrust_height += boostHeight * 4.0f * dt;
  else
    thrust_height -= boostHeight * dt;

  return finalForce;
}

bool PlayerController::IsJumpButtonPress()
{
  return ASC_KEYTRIGGERED(ASC_KEY_SPACE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::A);
}
bool PlayerController::IsInteractButtonPress()
{
  return ASC_KEYTRIGGERED(ASC_KEY_E) ||
    ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE) ||
    ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::X);
}

bool PlayerController::IsMissionListButtonPressed() const
{
  return ASC_KEYTRIGGERED(ASC_KEY_Q) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::BACK);
}
