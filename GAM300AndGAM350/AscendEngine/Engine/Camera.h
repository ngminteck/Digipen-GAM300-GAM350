/******************************************************************************/
/*!
\file   Camera.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for Camera and ShakyInfo

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

struct ShakyInfo
{
  Vec3 shakevec;
  float intensity = 1.0f;
  float xfac = 1.0f;
  float yfac = 1.0f;
  float duration = 0.5f;
  float decreaseFac = 1.0f;

  float currIntensity;
  float currDuration;
  Vec3 currOffset;

  void initVal();
};

const float orthResolution = 6.0f;

class Camera
{
public:
  Camera();
  Camera(float fov, float asp, float n, float f);
  Camera(float l, float r, float t, float b, float n, float f);

  void LookAt(const Vector3&);
  void LookAt(const Vector3&, const Vector3&);
  void SetPosition(const Vector3&);
  void SetFOV(float);
  void Resize(int width, int height);

  Vec3 GetBufferSize() { return Vec3((float)sWidth,(float)sHeight,0); }
  Vec3& GetPos() { return m_origin; }
  Vec3& GetForward() { return m_forward; }
  Vec3& GetRight() { return m_right; }
  Vec3& GetUp() { return m_up; }
  void RotateForward(float dAlpha, float dBeta);

  Matrix4& GetVP() { return vp; }
  Matrix4& GetV() { return view; }
  Matrix4& GetP() { return perpProj; }
  Matrix4& GetOrtho() { return orthoProj; }

  float& GetFOV() { return m_fov; }
  float& GetASP() { return m_asp; }
  float& GetNearPlane() { return m_near; }
  float& GetFarPlane() { return m_far; }

  std::array<Vec4, 6>& GetHalfPlanes() { return m_halfPlanes; }

  /* UNTESTED CODE */
  Vec2 GetMouseScreenRatio();
  void GetRayFromMouse(Vec3& start, Vec3&end);

  //For ImGui to manually set the dirty flag
  void SetDirty(bool d, bool p = false) { dirty = d; dproj = p; }

  void Update(float dt);

  float& GetExposure() { return exposureVal; }

  void SetdefaultCamShake(float duration, float intensity, float decreaseFac, float xfac = 1.0f, float yfac = 1.0f);
  void StartCamShake();
  inline bool IsCamShaking() const { return m_camshake.currDuration > 0.f; }

  //for getting UI angle
  //will return the angle (in radian) between the vector map onto cam screen "or plane" and the right vec
  float MapOnCamAndGetAngle(const Vec3& v, float& sqdist, Vec3& Q);
  //will calculate the position in UI on screen edge
  Vec2 GetUIEdgePos(Vec3 const& worldPos, float offsetFromEdge);
  Vec2 GetUIPosFromWorld(Vec3 const& worldPos);
  //takes in a sphere and return whether should cull or not
  bool FrustumCulling(Vec4 const& pos, float radius);
  //takes in a AABB and return whether should cull or not
  bool FrustumCulling(Vec3 const& min, Vec3 const& max);
  //to check if a pt is in frustum
  bool ptInView(Vec3 const& pos);
  //debug testing
  void DrawFrustumAsViewInWorld();
  //unsigned halfPlaneIndex = 0;
private:
  float m_fov, m_asp, m_near, m_far;
  Vec3 m_origin, m_forward, m_right, m_up;
  Matrix4 view, vp;
  Matrix4 perpProj, orthoProj;

  //bool m_startCamShake;
  ShakyInfo m_camshake;

  //Alpha is Pitch, Beta is Yaw
  float m_alpha, m_beta, m_radius;
  int sWidth, sHeight;

  float exposureVal;

  bool dirty, dproj;

  std::array<Vec4, 6> m_halfPlanes;
  void RecalculatePlanes();
  Vec2 UI_DiagPlaneNorm[2];
  void RecalculateUIFuncSetup();
  //camera stuff
  void camshake();
  bool ShakyUpdate(float dt);
};

#endif