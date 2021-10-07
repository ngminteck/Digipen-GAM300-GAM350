/******************************************************************************/
/*!
\file   LightManager.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

LightManager::LightManager()
{
  m_camera = &ASC_GMAN.GetCamera();
  m_shaders = &ASC_GMAN.GetShaders();

  m_Lights.reserve(40);
}

LightManager::~LightManager()
{
  glDeleteBuffers(1, &m_lightBuffer);
}

void LightManager::InitGL()
{
  glGenBuffers(1, &m_lightBuffer);
}

void LightManager::Load()
{
}

void LightManager::UnLoad()
{
  while (m_pool.size()) m_pool.pop();
  m_Lights.clear();
  m_components.Clear();
}

void LightManager::Update(float dt)
{
  for (auto &elem : m_components.m_objects)
  {
    GOHandle h = elem.GetGOHandle();
    if (h.IsValid())
    {
   
      LightData& light = m_Lights[elem.handle];
      if (h->GetIsEnable() && elem.Enabled)
      {
        TransformComp* t = ASC_FACTORY.GetComponentPtr<TransformComp>(h);

        light.position.x = t->pos.x;
        light.position.y = t->pos.y;
        light.position.z = t->pos.z;
      }
    }
    /*
    if (ASC_GETCOMP_HANDLE(GRAPHICS, h).IsValid()) 
    {
      GraphicsComp *gCp = ASC_FACTORY.GetComponent<GraphicsComp>(h);
      gCp->tint = light.color;
    }
    */
  }
}

BaseHandle LightManager::Deserialize(const GameObject & gameObject, Deserializer & deserializer)
{
  LightHandle id = m_components.GetNewObject();
  LightComp &tmp = *m_components.GetObj(id);
  tmp.handle = GetLightHdl();
  
  GOHandle h = gameObject.GetSelfHandle();
  m_components[id]->SetGOHandle(h);

  //tmpL.position = gameObject[TRANSFORM];
 // deserializer.Read("IsEnable", tmp.Enabled);
  deserializer.Read("Color", m_Lights[tmp.handle].color);
  deserializer.Read("Type", m_Lights[tmp.handle].position.w);
  deserializer.Read("Intensity", m_Lights[tmp.handle].intensity);
  deserializer.Read("Radius", m_Lights[tmp.handle].radius);
 //deserializer.Read("lightType",m_light)

  return id;
}

void LightManager::Serialize(Serializer &serializer, BaseHandle index)
{
  LightComp &tmp = *m_components.GetObj(LightHandle(index));
  LightData &l = m_Lights[tmp.handle];

  serializer.Write("IsEnable", tmp.Enabled);
  serializer.Write("Color", l.color);
  serializer.Write("Type", l.position.w);
  serializer.Write("Intensity", l.intensity);
  serializer.Write("Radius", l.radius);
}

void LightManager::Reflect(BaseHandle id, const Reflector & r)
{
  LightHandle lhdl(id);
  if (lhdl.Val() > m_components.m_objects.size()) return;

  //idx is index to component
  LightComp &lComp = *m_components[lhdl];
  LightData &lData = m_Lights[lComp.handle];
  
  bool val = lComp.Enabled;
  if (ImGui::Checkbox("Is Enable##light", &val))
  {
    lComp.Enabled = val;
  }
  r.ColorPicker("Color", lData.color.m);

  int type = static_cast<int>(lData.position.w);
  ImGui::SliderInt("Type", &type, -1, 1, type >= 0 ? type == 0 ? "Directional" : "Point" : "Disabled");
  lData.position.w = static_cast<float>(type);
  //std::cout << type << std::endl; 

  ImGui::DragFloat("Intensity", &lData.intensity, 0.01f, 0.01f, 10.0f);

  if (type >= 0 && type != 0)
    ImGui::DragFloat("Radius", &lData.radius, 0.1f, 0.1f, 100.0f);
}

BaseHandle LightManager::CreateComponent()
{
  LightHandle hdl = m_components.GetNewObject();
  LightComp &lComp = *m_components[hdl];
  lComp.handle = GetLightHdl();

  return hdl;
}

void LightManager::DestroyComponent(BaseHandle hdl)
{
  LightHandle lhdl(hdl);
  LightComp& lc = *m_components.GetObj(lhdl);
  ReleaseHdl(lc.handle);
  m_components.ReleaseObj(lhdl);
}

//void LightManager::InitComponent(const GameObject & go, Handle hdl)
//{
//  m_components[hdl]->SetGOHandle(go.GetSelfHandle());
//}

void LightManager::HandleMsg(MsgData_Base *)
{
}

void LightManager::BindLightBuffer(const std::string& shader)
{
  if (m_Lights.empty()) return;

  unsigned int bindingPt = 0;
  unsigned int prog = (*m_shaders)[shader]->Object();

  GLuint uniformLightDataBinding = glGetUniformBlockIndex(prog, "LightBlock");
  glUniformBlockBinding(prog, uniformLightDataBinding, bindingPt);

  // binding to index n, the glsl code needs to specify this same
  // index using 'layout(binding=n)'
  glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * m_Lights.size(), &m_Lights[0], GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPt, m_lightBuffer);

  //Unbind
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

size_t LightManager::NumOfLights()
{
  return m_Lights.size();
}

void LightManager::DebugLights(Mesh& md)
{
  /* Draw Lights */
  // Now render all light cubes with forward rendering as we'd normally do
  Matrix4 lightMat;
  lightMat.m00 = lightMat.m11 = lightMat.m22 = 0.5f;

  ShaderProgram* def = (*m_shaders)["Light"];

  def->BindShader();
  for (auto &elem : m_Lights)
  {
    if (elem.position.w < 0) continue;

    elem.color.a = 0.5f;
    lightMat.m30 = elem.position.x;
    lightMat.m31 = elem.position.y;
    lightMat.m32 = elem.position.z;
    def->SetUniform("MVP", lightMat * m_camera->GetVP());
    def->SetUniform("newColor", elem.color);

    glBindVertexArray(md.vao);
    glDrawArrays(GL_TRIANGLES, 0, md.triCount);

    ASC_LINERENDER.AddSphere(elem.position.xyz, elem.radius * elem.intensity, Color::YELLOW);
  }

  ShaderProgram::UnbindShader();
}

void LightManager::SetCamera(Camera *cam)
{
  m_camera = cam;
}

void LightManager::CopyComponentToComponent(LightComp & target, const LightComp & source)
{
  target.handle = GetLightHdl();
  LightData &lhs = m_Lights[target.handle];
  LightData &rhs = m_Lights[source.handle];
  lhs = rhs;
}

void LightManager::AddSkyLights(const Vec4 & pos, const Vec4 & color, float intensity)
{
  GameObject& dirLight = ASC_FACTORY.AddObject();
  dirLight.name = "Sky Light";
  ASC_FACTORY.AddComponent(dirLight, LIGHT);

  GOHandle hdl = dirLight.GetSelfHandle();

  TransHandle tc = hdl->GetHdl<TransformComp>();
  tc->pos = pos.xyz;

  LightHandle lc = hdl->GetHdl<LightComp>();
  LightData& ld = m_Lights[lc->handle];
  ld.color = color;
  ld.position = pos;
  ld.intensity = intensity;
}

LightData& LightManager::GetLightData(const LightHandle & hdl)
{
  assert(hdl.IsValid());
  LightComp &lComp = *m_components[hdl];
  return m_Lights[lComp.handle];
}

size_t LightManager::GetLightHdl()
{
  size_t idx = m_Lights.size();
  if (m_pool.empty()) m_Lights.push_back(LightData());
  else {
    idx = m_pool.top(); 
    m_pool.pop();
    m_Lights[idx].SetType(LightType::DISABLE);
  }
  return idx;
}

void LightManager::ReleaseHdl(size_t idx)
{
  m_Lights[idx].SetType(LightType::DISABLE);
  m_pool.push(idx);
}
