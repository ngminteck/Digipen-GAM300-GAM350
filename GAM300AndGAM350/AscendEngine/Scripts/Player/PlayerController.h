/******************************************************************************/
/*!
\file   PlayerController.h
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
#ifndef PLAYER_CONTROLLER
#define PLAYER_CONTROLLER

class PlayerController : public SCRIPTING::ScriptCommonIF<PlayerController>
{
private:
  std::string scriptName = "_Player";

  GOHandle UIMission;

  //Variables
  Vec3 pos;
  Vec2 mousepos_old;
  Vec2 mousepos_new;
  Vec2 mDelta;
  float cameraOffSety = 0.0f;

  std::vector<std::string> promptTextures;

  //int   cam_handle          = GraphicsManager::CameraIdx::PLAYER_CAM;
  //Vec3 newCamPos; //for shooting
    //  Serialised var
  float max_fly_distance    = 500000;
  Vec3 ref_point;

  float cam_scale_fwd       = -0.5f;
  float cam_scale_up        = 0.5f;
  float cam_scale_back      = 0.5f;
  float mouse_sensitivity   = 1.0f;

  float thrust_accl          = 1000.0f;
  float thrust_curr          = 0.0f;
  float thrust_damping       = 0.98f;
  float thrust_max           = 10000.0f;
  float thrust_height        = 0.0f;
    //
  float climbingSpeed       = 0.07f;
  float hoverHeight         = 100.0f;
  float boostHeight         = 2000.0f;
  float oldHeight		        = 0.0f;
  float range               = 0.0f;
  bool Jumping              = true;
  float health              = 100.0f;
  float limitToStartApplying = 3.0f;
  float additionalGravForce = -100.0f;
 
  Vec3 yForce;
  Vec3 uptiltForce;
  //Vec3 camFwd = Vec3(0.0f, 0.0f, 1.0f);

  Vec3 heldItemoffset;


  
  bool isCutScene;
  bool isGrounded = true;

  bool isQuestShowing = false;

  bool playOnce = true;
  GOHandle lastFloor;
  bool Distancecheck(Vec3& force);

public:
    bool freezeCam;
  Vec3 raystartOffset;
  // this also determines how tall is the dog ?!
  float floorRayLength;
  void DealDamage(float dmg);
  GOHandle lastInteract;
  bool holdingItem;
  Vec2 currDelta;
  bool movingItem = false, push = false, pull = false;
  float moveSpeed = 10.0f;
  Camera& player_cam;
  Vec3 finalForce;
  bool Swivel = false;
  //HUD stuff
  GOHandle HUD_overall;
  GOHandle prompt;
  PlayerController();
  bool canMove;
  float defaultFOV;
  Vec3 playerSpawnForward;
  Vec3 StartPos = Vec3::ZERO;
  void SetCutsceneRot();
  //Vec3 GetPlayerCamPos() const;
  //int GetCamHandle() const;
  Vec2 GetMouseDelta() const;
  int MoveCamToLocation(const Vec3& = Vec3::ZERO, const Vec3& = Vec3::ZERO, float = 0.0f);
  void OnStart() override;
  void Update(float dt) override;
  void OnDestroy() override;

  void Serialize(Serializer &sr, LogicHandle idx) override;
  void Deserialize(Deserializer &d) override;
  void Reflect(const Reflector& r) override;

  void OnCollisionEnter(GOHandle other) override;
  void OnCollisionStay(GOHandle other) override;
  void OnCollisionExit(GOHandle other) override;

  void HandleMessage(MsgData_Base *m);

  Vector3 HandleKeyboard(Camera&, float);
  Vector3 HandleController(Camera&, float);

  bool IsMissionListButtonPressed() const;

  void GrabItem();
  bool IsJumpButtonPress();
  bool IsInteractButtonPress();
};


#endif