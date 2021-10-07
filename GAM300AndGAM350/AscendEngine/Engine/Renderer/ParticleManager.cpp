/******************************************************************************/
/*!
\file   ParticleManager.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
  functions to pEmitter particles

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <pch.h>

#define vertexShader "#version 420 core\n\
layout(location = 0) in vec3 pos;\n\
layout(location = 1) in vec2 uv;\n\
layout(location = 2) in vec4 color;\n\
layout(location = 3) in mat4 modelMat;\n\
uniform mat4 vpMat;\n\
out vec2 uvCoord;\n\
out vec4 vertCol;\n\
void main()\n\
{\n\
  vertCol = color;\n\
  uvCoord = vec2(uv.x, 1 - uv.y);\n\
  gl_Position = vpMat * modelMat * vec4(pos, 1.0);\n\
}"

#define fragmentShader "#version 420 core\n\
in vec2 uvCoord;\n\
in vec4 vertCol;\n\
uniform sampler2D tex_diffuse;\n\
out vec4 outColor;\n\
void main()\n\
{\n\
  vec4 diffuse = texture(tex_diffuse, uvCoord);\n\
  outColor = diffuse * vertCol;\n\
}"

ParticleManager::ParticleManager() : isEditorMode(true), isGamePause(true)
{
  auto& shaders = ASC_GMAN.GetShaders();
  std::vector<Shader> tmp = {
    Shader(vertexShader, GL_VERTEX_SHADER),
    Shader(fragmentShader, GL_FRAGMENT_SHADER) };
  shaders["Particle"] = new ShaderProgram(tmp); // link shaders

  ASC_MSGSYS.RegisterCallBack(mTYPE_PAUSE, &ParticleManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &ParticleManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &ParticleManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_RESUME, &ParticleManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_PAUSE, &ParticleManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_RESUME, &ParticleManager::HandleMsg, this);
}

void ParticleManager::Load()
{
}

void ParticleManager::UnLoad()
{
  m_components.Clear();
}

void ParticleManager::Update(float dt)
{
  sortedList.clear();

  Camera& cam = ASC_GMAN.GetCamera();
  for (auto &elem : m_components.m_objects)
  {
    if (elem.Enabled)
    {


      elem.SetCamera(cam);

      if (isEditorMode || !isGamePause)
        elem.Update(dt);

      if (!isEditorMode && !isGamePause &&
        elem.selfDestruct)
      {
        if (elem.countdown >= 0)
          elem.countdown -= dt;
        else if (elem.activeCount == 0)
          m_destroy.push(elem.GetGOHandle());
      }

      //Vec3 offset = (elem.maxAABB - elem.minAABB) * 0.5;
      //Vec3 pos = (elem.minAABB + elem.maxAABB)* 0.5;
      //ASC_LINERENDER.AddCube(pos, elem.minAABB - pos, elem.maxAABB - pos, Color::GREEN);
      if (elem.minAABB.x != FLT_MAX)
        if (!cam.FrustumCulling(elem.minAABB, elem.maxAABB))
          sortedList.push_back(&elem);
    }
  }

  Vec3 camPos = cam.GetPos();
  std::sort(sortedList.begin(), sortedList.end(),
    [&](ParticleEmitter* lhs, ParticleEmitter* rhs)
  {
    Vec3 lhsPos = camPos - lhs->GetTransHdl()->pos;
    Vec3 rhsPos = camPos - rhs->GetTransHdl()->pos;

    return (lhsPos.MagnitudeSq() > rhsPos.MagnitudeSq());
  });

  while (!m_destroy.empty())
  {
    ASC_FACTORY.DestroyObject(m_destroy.top());
    m_destroy.pop();
  }
}

BaseHandle ParticleManager::Deserialize(const GameObject & gameObject, Deserializer &d)
{
  PEHandle id(CreateComponent());
  ParticleEmitter &pEmitter = *m_components[id];
  pEmitter.SetGOHandle(gameObject.GetSelfHandle());
  pEmitter.SetTransHdl(Handle<TransformComp>(gameObject[TRANSFORM]));

  pEmitter.Deserialize(d);

  return id;
}

void ParticleManager::Serialize(Serializer &s, BaseHandle hdl)
{
  if (m_components.m_objects.empty()) return;
  m_components[PEHandle(hdl)]->Serialize(s);
}

void ParticleManager::Reflect(BaseHandle hdl, const Reflector &r)
{
  static std::vector<std::string> shapes =
  { "POINT", "SPHERE" };

  static std::vector<std::string> pEmitterter =
  { "OVERTIME", "BURST" };

  static std::vector<std::string> movType =
  { "LINEAR", "RANDOM" };

  ParticleEmitter &pEmitter = *m_components[PEHandle(hdl)];

  pEmitter.textureName = ASC_GMAN.SetAndReflectAlbedo(r, pEmitter.textureName);
  r.Reflect("Enabled", pEmitter.Enabled);
  int MaxParticles = pEmitter.GetMaxParticleCount();
  if (r.Reflect("MaxParticles", MaxParticles))
    pEmitter.SetMaxParticle(MaxParticles);

  int eType = (int)pEmitter.emissionType;
  if (r.Reflect("Emission", pEmitterter, &eType))
    pEmitter.emissionType = (ParticleEmitter::EMISSION)eType;
  r.Reflect("LocalParticles", pEmitter.isLocal);

  if (r.Reflect("SpawnDelay", pEmitter.spawnDelay))
  {
    pEmitter.currentDelay = pEmitter.spawnDelay;
    pEmitter.currentTime = pEmitter.spawnRate;
  }
  r.Reflect("spawnOffSet", pEmitter.spawnLocOffSet);
  r.Reflect("SpawnRate", pEmitter.spawnRate);
  r.Reflect("Acceleration", pEmitter.acceleration);
  r.Reflect("SpawnParticles\t", pEmitter.isLooping);
  ImGui::SameLine();
  if (ImGui::Button("Emit")) pEmitter.Emit();

  if (ImGui::TreeNode("Velocity"))
  {
    r.Reflect("Min", pEmitter.minVelocity);
    r.Reflect("Max", pEmitter.maxVelocity);
    ImGui::TreePop();
  }

  //if (ImGui::CollapsingHeader("Scale"))
  if (ImGui::TreeNode("Lifetime"))
  {
    r.Reflect("Min", pEmitter.minLifetime);
    r.Reflect("Max", pEmitter.maxLifetime);

    r.Reflect("SelfDestruct", pEmitter.selfDestruct);
    if(pEmitter.selfDestruct) r.Reflect("Countdown", pEmitter.countdown);

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Rotation"))
  {
    if (!pEmitter.isDoubleSided)
      r.Reflect("Billboarded", pEmitter.isBillboarded);
    if (!pEmitter.isBillboarded)
      r.Reflect("DoubleSided", pEmitter.isDoubleSided);

    if (!pEmitter.isBillboarded) 
      r.Reflect("LookDir", pEmitter.faceDirection);

    r.Reflect("Min", pEmitter.minRotSpeed);
    r.Reflect("Max", pEmitter.maxRotSpeed);

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Scale"))
  {
    if (ImGui::TreeNode("Start"))
    {
      r.Reflect("Min", pEmitter.startMinScale);
      r.Reflect("Max", pEmitter.startMaxScale);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("End"))
    {
      r.Reflect("Min", pEmitter.endMinScale);
      r.Reflect("Max", pEmitter.endMaxScale);
      ImGui::TreePop();
    }

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Color"))
  {
    if (ImGui::TreeNode("Start"))
    {
      r.ColorPicker("Min", &pEmitter.startMinCol.r);
      r.ColorPicker("Max", &pEmitter.startMaxCol.r);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("End"))
    {
      r.ColorPicker("Min", &pEmitter.endMinCol.r);
      r.ColorPicker("Max", &pEmitter.endMaxCol.r);
      ImGui::TreePop();
    }

    r.Reflect("Blend Color", pEmitter.isBlending);

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Drag"))
  {
    if (ImGui::TreeNode("Start"))
    {
      r.Reflect("Min", pEmitter.startMinSpeed);
      r.Reflect("Max", pEmitter.startMaxSpeed);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("End"))
    {
      r.Reflect("Min", pEmitter.endMinSpeed);
      r.Reflect("Max", pEmitter.endMaxSpeed);
      ImGui::TreePop();
    }

    ImGui::TreePop();
  }

  ImGui::Checkbox("Pause simulation", &pEmitter.pauseSimulation);
  ImGui::SameLine();
  ImGui::Text("Active Particles : %d", pEmitter.activeCount);
}

void ParticleManager::InitComponent(const GameObject & go, BaseHandle hdl)
{
  PEHandle pehdl(hdl);
  m_components[pehdl]->SetGOHandle(go.GetSelfHandle());
  //set the transform component of the new comp game object
  ParticleEmitter* newcomp = m_components[pehdl];
  newcomp->activeCount = 0;
  newcomp->SetTransHdl(Handle<TransformComp>(go[TRANSFORM]));
}

void ParticleManager::CopyComponentToComponent(ParticleEmitter & target, const ParticleEmitter & source)
{
  target = source;
}

void ParticleManager::HandleMsg(MsgData_Base *msg)
{
  switch (msg->type)
  {
    case mTYPE_PAUSE: { isEditorMode = true; } break;
    case mTYPE_STOP: { isEditorMode = true; } break;
    case mTYPE_PLAY: { isEditorMode = false; isGamePause = false; } break;
    case mTYPE_RESUME: { isEditorMode = false; } break;
    case mTYPE_GAME_PAUSE: { isGamePause = true; } break;
    case mTYPE_GAME_RESUME: { isGamePause = false; } break;
  default: break;
  };
}
 