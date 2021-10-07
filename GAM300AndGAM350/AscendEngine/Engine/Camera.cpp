/******************************************************************************/
/*!
\file   Camera.cpp
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

#include "pch.h"
const float Alphalimit = 80.0f;

Camera::Camera() : m_fov(60.0f), m_asp(0), m_near(0.2f), m_far(15000.0f),
m_origin(0, 1, 5), m_forward(0, 0, 1), m_right(1, 0, 0), m_up(0, 1, 0), exposureVal(5.0f)
{
  Resize(1280, 720);
  LookAt({ 0, 0, 0 });
  RecalculatePlanes();
  RecalculateUIFuncSetup();
}

Camera::Camera(float fov, float asp, float n, float f) :
  m_fov(fov), m_asp(asp), m_near(n), m_far(f), exposureVal(5.0f) {
  perpProj = Mat4::SymPerspective(fov, asp, n, f);
  orthoProj = Mat4::SymOrthographic(orthResolution * m_asp, orthResolution, -1.0f, m_far);
}

Camera::Camera(float l, float r, float t, float b, float n, float f) :
  m_fov(45.0f), m_asp((r - l) / (t - b)), m_near(n), m_far(f), exposureVal(5.0f) {
  perpProj = Mat4::NonSymPerspective(l, r, t, b, n, f);
  orthoProj = Mat4::SymOrthographic(orthResolution * m_asp, orthResolution, -1.0f, m_far);
}

void Camera::LookAt(const Vector3 & target)
{
  m_forward = (target - m_origin);
  m_radius = m_forward.Magnitude();

  m_forward = m_forward.Normalize();
  m_alpha = ASC_RAD2DEG(asin(m_forward.y));

  Vec2 x(1.0f, 0.0f);
  m_beta = x.GetAngle({ m_forward.x, m_forward.z });
  if (m_forward.z > 0) m_beta = -m_beta;

  m_right = m_forward.Cross(Vec3::YAXIS).Normalize();
  m_up = m_right.Cross(m_forward).Normalize();

  dirty = true;
}
void Camera::LookAt(const Vector3& forward, const Vector3& right)
{
    m_forward = -forward;
    m_radius = m_forward.Magnitude();

    m_forward = m_forward.Normalize();
    m_alpha = ASC_RAD2DEG(asin(m_forward.y));

    Vec2 x(1.0f, 0.0f);
    m_beta = x.GetAngle({ m_forward.x, m_forward.z });
    if (m_forward.z > 0) m_beta = -m_beta;

    m_right = right;
    m_up = right.Cross(m_forward).Normalize();

    dirty = true;
}

void Camera::SetPosition(const Vector3 & pos)
{
  m_origin = pos;
  dirty = true;
}

void Camera::SetFOV(float val)
{
  m_fov = val;
  dproj = true;
}

void Camera::Resize(int sW, int sH)
{
  sWidth = sW; sHeight = sH;
  m_asp = ((float)sW) / ((float)sH);
  dirty = dproj = true;
}

void Camera::RotateForward(float dAlpha, float dBeta)
{
  if (abs(m_alpha + dAlpha) < Alphalimit)
    m_alpha += dAlpha;

  m_beta += dBeta;

  float a = ASC_DEG2RAD(m_alpha), b = ASC_DEG2RAD(m_beta);
  float cosA = cosf(a);

  m_forward.x = cosA*cosf(b);
  m_forward.y = sinf(a);
  m_forward.z = -cosA*sinf(b);

  m_forward = m_forward.Normalize();
  m_right = m_forward.Cross(Vec3::YAXIS).Normalize();
  m_up = m_right.Cross(m_forward).Normalize();

  dirty = true;
}

Vec2 Camera::GetMouseScreenRatio()
{
  Vec2 mousePos = ASC_INPUT.GetMousePos();
  Vec2 screenSpace;
  screenSpace.x = max(0, min(mousePos.x, sWidth));
  screenSpace.y = max(0, min(sHeight - mousePos.y, sHeight));
  screenSpace -= Vec2(sWidth * 0.5f, sHeight * 0.5f);
  screenSpace.x /= (float)sWidth;
  screenSpace.y /= (float)sHeight;

  return screenSpace;
}

void Camera::GetRayFromMouse(Vec3 & start, Vec3 & end)
{
  Vec2 screenSpace = GetMouseScreenRatio();

  Vec3 upVec = m_right.Cross(m_forward).Normalize();
  Vec3 RtOffset = (m_right * screenSpace.x) * 2.f;
  Vec3 UpOffset = (upVec * screenSpace.y) * 2.f;

  Vec3 farPoint = m_origin + (m_forward * m_far);
  float hAngle = tanf(ASC_DEG2RAD(m_fov / 2.f));

  float half_FH = m_far * hAngle;
  float half_FW = m_far * m_asp * hAngle;

  start = m_origin;
  end = farPoint + (RtOffset * half_FW) + (UpOffset * half_FH);
}

void Camera::Update(float dt)
{
  //debug
  //if (ASC_KEYTRIGGERED(ASC_KEY_B))
  //{
  //  ++halfPlaneIndex;
  //  halfPlaneIndex %= 6;
  //  Log(halfPlaneIndex);
  //}

  if (ShakyUpdate(dt)) dirty = true;

  Vec3 start, end;
  GetRayFromMouse(start, end);
  if (dproj)
  {
    perpProj = Mat4::SymPerspective(m_fov, m_asp, m_near, m_far);
    orthoProj = Mat4::SymOrthographic(orthResolution * m_asp, orthResolution, -1.0f, m_far);
    dproj = false;
    dirty = true;
    //recalculate plane
    RecalculatePlanes();
    RecalculateUIFuncSetup();
  }

  if (!dirty) return;

  //Limit the rotation angle of the FPS camera
  if (m_alpha <= -Alphalimit) m_alpha = -Alphalimit;
  else if (m_alpha >= Alphalimit) m_alpha = Alphalimit;

  if (m_beta < 0.0f) m_beta += 360.0f;
  else if (m_beta > 360.0f) m_beta -= 360.0f;

  //m_up = m_right.Cross(m_forward).Normalize();
  view = Mat4::LookAt(m_origin + m_camshake.currOffset, m_forward, m_right);
  vp = view * perpProj;
  dirty = false;
}

void Camera::SetdefaultCamShake(float duration, float intensity, float decreaseFac, float xfac, float yfac)
{
  //check if the camera is still shaking first
  //and if the current intensity is lower than the given intensity
  //if (m_camshake.currDuration > 0.f && m_camshake.currIntensity > intensity) return;

  m_camshake.duration = duration;
  m_camshake.intensity = m_camshake.currDuration > 0.f ?
  (m_camshake.intensity * 0.2f + intensity * 0.8f) : intensity;
  m_camshake.xfac = xfac;
  m_camshake.yfac = yfac;
  m_camshake.decreaseFac = decreaseFac;
  //calculate the vector using right vec and the cross of back and right
  //m_camshake.shakevec = m_right.Cross(m_forward).Normalize();
  //m_camshake.shakevec = (up + m_right).Normalize();
}

void Camera::StartCamShake()
{
  m_camshake.initVal();
}

float Camera::MapOnCamAndGetAngle(const Vec3 & v, float& sqdist, Vec3& Q)
{
  //m_forward is the normal of the cam screen
  //assume the forward is always normalised
  float scalep = m_forward.Dot(v);
  Q = v - (scalep * m_forward);
  sqdist = Q.Dot(Q);
  float angle = acosf(m_right.Dot(Q.Normalize()));
  return angle * (Q.Dot(m_up) > 0.f ? 1 : -1);
}

Vec2 Camera::GetUIEdgePos(Vec3 const & worldPos, float offsetFromEdge)
{
  //DOING ALL IN VIEW SPACE
  Vec3 viewcam_to_pos = (view * Vec4(worldPos, 1.0f)).rgb;
  Vec3 cam_to_pos = viewcam_to_pos;// worldPos - m_origin;
  static const Vec3 viewForward = (-Vec3::ZAXIS);
  float scalep = viewForward.Dot(cam_to_pos);
  Vec3 Q = cam_to_pos - (scalep * viewForward);
  Vec2 Q2(Q.x, Q.y);
  //we need to find the region the worldPos is in
  //0 is x axis, 1 is y axis
  unsigned region = (Q2.Dot(UI_DiagPlaneNorm[0]) > 0.f ? 2 : 0)
    + (Q2.Dot(UI_DiagPlaneNorm[1]) > 0.f ? 1 : 0);
  //if dot with UI diag norm we get the 4 quad region
  //we use x-diag axis positive side with weight 2 
  //y-diag axis positive side with weight 1
  //will get ++:3, +-:2, -+:1, --:0
  //float const offsetFac = 1.0f - offsetFromEdge;
  static const Vec2 norm[4] = { {-1, 0}, { 0, -1},
                              { 0, 1 }, { 1, 0 } };
  //we will now find the intersection between the screen edge and Q
  Vec2 currnorm = norm[region];
  //need to do this since we are mapping asp to -1 to 1
  //Q2.x /= m_asp;
  Vec2 projVec = Q2.Dot(currnorm) * currnorm;
  //we can do just adding up here instead of checking which component to take
  //is because one of the component should be 0;
  //for x axis in UI has to be of asp times of normal

  //this should be correct.. but sry zl.. i fk up the UI so badly
  //currnorm.x *= m_asp; 

  //so just use this fix
  static const float fixedASP = 1920.0f / 1200.0f;
  currnorm.x *= fixedASP;

  //need to multiply with norm region so that we get the direction
  //currnorm.x -= norm[region].x * (1.0f / fixedASP) * offsetFromEdge;
  currnorm.x -= norm[region].x * offsetFromEdge;
  currnorm.y -= norm[region].y * offsetFromEdge;
  //currnorm -= norm[region] * offsetFromEdge;
  float t = (currnorm.x + currnorm.y) / (projVec.x + projVec.y);
  Vec2 ret = Q2 * t;
  return ret;
}

Vec2 Camera::GetUIPosFromWorld(Vec3 const & worldPos)
{
  Vec4 pos = vp * Vec4(worldPos, 1.0f);
  //perspective division 
  Vec3 newos = (pos * (1.0f / pos.w)).xyz;
  Vec2 UIpos(newos.x * m_asp, newos.y);
  //Vec2 UIpos(newos.x, newos.y);
  return UIpos;
}

bool Camera::FrustumCulling(Vec4 const & pos, float radius)
{
  bool cull = false;
  //Vec4 pos(cpos, 1.0f);
  for (int i = 0; i < 6; ++i)
  {
    float value = m_halfPlanes[i].Dot(pos);
    //check against the ritter sphere brought in to the world and view space
    if (value > radius)
    {
      cull = true;
      break;
    }
  }
  return cull;
}

bool Camera::FrustumCulling(Vec3 const & min, Vec3 const & max)
{
  Vec2 pts[3] = { {min.x, max.x}, {min.y, max.y}, {min.z, max.z} };
  //for (int i = 0; i < 2; ++i)
  //  for (int j = 0; j < 2; ++j)
  //    for (int k = 0; k < 2; ++k)
  //      if (ptInView({pts[0].m[i], pts[1].m[j], pts[2].m[k] }))
  //        return false;

  //return true;

  Vec4 allpt[8];
  unsigned count = 0;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      for (int k = 0; k < 2; ++k)
        allpt[count++] = (view * Vec4(pts[0].m[i], pts[1].m[j], pts[2].m[k], 1.0f ));

  //each of the plane
  for (Vec4& plane : m_halfPlanes)
  {
    int i = 0;
    for (; i < 8; ++i)
    {
      float value = plane.Dot(allpt[i]);
      //check against the planes
      if (value <= 0.f)
        break;
    }
    if (i == 8) return true;
  }

  return false;
}

bool Camera::ptInView(Vec3 const & arg_pos)
{
  bool inView = true;
  Vec4 pos(arg_pos, 1.0f);
  pos = view * pos;
  for (int i = 0; i < 6; ++i)
  {
    float value = m_halfPlanes[i].Dot(pos);
    //check against the planes
    if (value > 0.f)
    {
      inView = false;
      break;
    }
  }
  return inView;
}

void Camera::DrawFrustumAsViewInWorld()
{
  static Vec4 const BLUE(0.f, 0.f, 1.0f, 1.0f);
  float fovd2 = m_fov * 0.5f;
  float hd2 = m_far * tan(ASC_DEG2RAD(fovd2));
  float wd2 = m_asp * hd2;
  //starting from min orientate clockwise 
  float notSoFar = -m_far + 10.f;
  Vec3 const farPt[4] = { Vec3(-wd2, -hd2,notSoFar),
    Vec3(wd2, -hd2,notSoFar),
    Vec3(wd2, hd2, notSoFar),
    Vec3(-wd2, hd2,notSoFar) };
  for (int i = 0; i < 4; ++i)
  {
    ASC_LINERENDER.AddLine(Vec3(), farPt[i], BLUE);
    ASC_LINERENDER.AddLine(farPt[i], farPt[(i + 1) % 4], BLUE);
  }
}

void Camera::RecalculatePlanes()
{
  //near, far, left, right, top, bottom
  //near
  m_halfPlanes[0] = Vec4(0.f, 0.f, 1.f, m_near);
  //far
  m_halfPlanes[1] = Vec4(0.f, 0.f, -1.f, -m_far);
  //find height/2 of near plane
  float fovd2 = m_fov * 0.5f;
  float hd2 = m_near * tan(ASC_DEG2RAD(fovd2));
  float wd2 = m_asp * hd2;
  //left
  m_halfPlanes[2] = Vec4(-m_near, 0.f, wd2, 0.f);
  //right
  m_halfPlanes[3] = Vec4(m_near, 0.f, wd2, 0.f);
  //top
  m_halfPlanes[4] = Vec4(0.f, m_near, hd2, 0.f);
  //bottom
  m_halfPlanes[5] = Vec4(0.f, -m_near, hd2, 0.f);

  for (int i = 2; i < 6; ++i) m_halfPlanes[i] = m_halfPlanes[i].Normalize();
}

void Camera::RecalculateUIFuncSetup()
{
  //NO LONGER USING ASP FOR UI!!
  //the other diagonal "refer below"
  UI_DiagPlaneNorm[0] = Vec2(1.0f, m_asp).Normalize();
  //UI_DiagPlaneNorm[0] = Vec2(1.0f, 1.0f).Normalize();
  //the diagonal from min to max of UI screen
  UI_DiagPlaneNorm[1] = Vec2(1.0f, -m_asp).Normalize();
  //UI_DiagPlaneNorm[1] = Vec2(1.0f, -1.0f).Normalize();
}

void Camera::camshake()
{
  m_camshake.currOffset = m_camshake.currIntensity *
    (ASC_RAND_RANGE * m_camshake.yfac * m_up +
      ASC_RAND_RANGE * m_camshake.xfac * m_right);
}

bool Camera::ShakyUpdate(float dt)
{
  m_camshake.currOffset = Vec3();
  if (m_camshake.currDuration > 0.f)
  {
    camshake();
    m_camshake.currDuration -= dt;
    m_camshake.currIntensity *= m_camshake.decreaseFac;
    return true;
  }
  return false;
}

void ShakyInfo::initVal()
{
  currIntensity = intensity;
  currDuration = duration;
}
