/******************************************************************************/
/*!
\file   GraphicsManager.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for GraphicsManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

#define vertexShader "#version 420 core\n\
layout(location = 0) in vec3 pos;\n\
layout(location = 1) in vec4 color;\n\
uniform mat4 MVP;\n\
out vec4 vertCol;\n\
void main()\n\
{\n\
  vertCol = color;\n\
  gl_Position = MVP * vec4(pos, 1.0);\n\
}"

#define fragmentShader "#version 420 core\n\
in vec4 vertCol;\n\
out vec4 outColor;\n\
void main()\n\
{\n\
  outColor = vertCol;\n\
}"

#define LightVS "#version 420 core\n\
layout(location = 0) in vec3 pos;\n\
layout (location = 1) in vec3 nrm;\n\
layout(location = 2) in vec3 uv;\n\
layout(location = 4) in vec3 tan;\n\
layout(location = 5) in vec3 biTan;\n\
uniform mat4 MVP;\n\
void main()\n\
{\n\
  gl_Position = MVP * vec4(pos, 1.0);\n\
}"

#define LightFS "#version 420 core\n\
uniform vec4 newColor;\n\
out vec4 outColor;\n\
void main()\n\
{\n\
  outColor = newColor;\n\
}"


const float fsquad[] = {
  /*Position*/ /*UV Coord*/
 -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f,
 -1.0f,  1.0f, 0.0f, 1.0f,
  1.0f,  1.0f, 1.0f, 1.0f
};

unsigned int RGB2Int(unsigned int r, unsigned int g, unsigned int b)
{
  return r + (g * 256) + (b * 256 * 256);
}
void Int2RGB(unsigned int i, float &r, float &g, float &b)
{
  r = ((i & 0x000000FF) >> 0) / 255.0f;
  g = ((i & 0x0000FF00) >> 8) / 255.0f;
  b = ((i & 0x00FF0000) >> 16) / 255.0f;
}

#define GETRANS(x) *m_position.GetObj(x)
#define PICKINGOFFSET 1

GraphicsManager::GraphicsManager(HandleManager<TransformComp>& position)
  : bgColor(0.0f, 0.0f, 0.0f, 1.0f), m_position(position), isDebugDraw(false),
  showMetric(false), lineR(isDebugDraw),
  m_highlightCol(Color::YELLOW.xyz), m_highlightScale(5.0f) {
  ASC_MSGSYS.RegisterCallBack(mTYPE_RESIZE, &GraphicsManager::HandleMsg, this);
  
  InitGL();

  std::vector<Shader> tmp; tmp.reserve(2);
  tmp.push_back(Shader(vertexShader, GL_VERTEX_SHADER)); // load vertex
  tmp.push_back(Shader(fragmentShader, GL_FRAGMENT_SHADER)); // load fragment
  m_shaders["DEFAULT"] = new ShaderProgram(tmp); // link shaders

  tmp.clear();
  tmp.push_back(Shader(LightVS, GL_VERTEX_SHADER)); // load vertex
  tmp.push_back(Shader(LightFS, GL_FRAGMENT_SHADER)); // load fragment
  m_shaders["Light"] = new ShaderProgram(tmp); // link shaders

  SetupGBuffers();
  SetupFSQuad();
  SetupSkyBox();

  m_texLoader.Load("Blank", ASC_TEXTURES_FLD"Blank.jpg");

  m_cameraVec.reserve(3);
  m_cameraVec = std::vector<Camera>(3, Camera());
}

void GraphicsManager::InitGL()
{
  /* GL3W Initialization */
  if (gl3wInit())
  {
    MessageBox(NULL, (LPCSTR)"Failed to initialize OpenGL", (LPCSTR)"Error", MB_OK | MB_ICONEXCLAMATION);
    throw std::exception("Failed to initalize OpenGL");
  }

  if (!gl3wIsSupported(4, 3))
  {
    MessageBox(NULL, (LPCSTR)"OpenGL 4.3 not supported", (LPCSTR)"Error", MB_OK | MB_ICONEXCLAMATION);
    throw std::exception("OpenGL 4.3 not supported");
  }
}

Camera& GraphicsManager::SetCamera(int handle)
{
  //#if !_SUBMISSION
  //  bool outOfRange = handle < 0 || handle >((int)m_cameraVec.size() - 1);
  //  if (outOfRange)
  //  {
  //    handle = AddCamera();
  //    LogWarning("GraphicsManager::SetCamera invalid camera idx!");
  //    //return m_cameraVec[handle];
  //  }
  //  Log("GetCamera: Setting camera to ", handle);
  //#endif
  //  assert(!(handle < 0 || handle >((int)m_cameraVec.size() - 1)));
  assert(handle < m_cameraVec.size());

  m_cam_idx = handle;
  /* Change to reference */
  ASC_LIGHTS.SetCamera(&m_cameraVec[m_cam_idx]);
#if !_SUBMISSION
  //Log("Camera #", m_cam_idx);
#endif
  return m_cameraVec[m_cam_idx];
}

void GraphicsManager::SetHighlightCol(const Vec3 &col)
{
  m_highlightCol = col;
}

void GraphicsManager::SetHighlightScale(float scale)
{
  m_highlightScale = scale;
}

void GraphicsManager::CleanUp()
{
  for (int i = 0; i < m_buffers.size(); ++i)
    glDeleteTextures(1, &m_buffers[i].handle);

  m_buffers.clear();

  glDeleteFramebuffers(1, &m_DeferredFBO);
  glDeleteFramebuffers(1, &m_PickerFBO);
}

int GraphicsManager::GetCameraIdx()
{
  return m_cam_idx;
}

size_t GraphicsManager::GetTotalCamera()
{
  return m_cameraVec.size();
}

Camera & GraphicsManager::GetCamera()
{
  return m_cameraVec[m_cam_idx];
}

Camera & GraphicsManager::GetCamera(int handle)
{
  assert(!(handle < 0 || handle >((int)m_cameraVec.size() - 1)));
  return m_cameraVec[handle];
}

Camera& GraphicsManager::NextCamera()
{
  const int idx_max = (int)m_cameraVec.size() - 1;
  int tmp = m_cam_idx + 1 > idx_max ? 0 : m_cam_idx + 1;

  return SetCamera(tmp);
}

Camera & GraphicsManager::PrevCamera()
{
  const int idx_max = (int)m_cameraVec.size() - 1;
  int tmp = m_cam_idx - 1 < 0 ? idx_max : m_cam_idx - 1;

  return SetCamera(tmp);
}

std::string GraphicsManager::SetAndReflectModel(const Reflector& r, const std::string& name)
{
  int select = 0;
  for (select = 0; select < m_models.size(); ++select)
    if (m_models[select].find(name) == 0) break;

  r.Reflect("Model", m_models, &select);

  if (select >= m_models.size()) select = 0;

  std::string mdl = m_models[select];
  return mdl.substr(0, mdl.find(".model"));
}

std::string GraphicsManager::SetAndReflectAlbedo(const Reflector & r, const std::string & name)
{
  std::string ret;
  int select = 0;
  for (select = 0; select < m_albedoTextures.size(); ++select)
    if (m_albedoTextures[select].find(name) == 0) break;

  r.Reflect("AlbedoMap", m_albedoTextures, &select);

  if (select < m_albedoTextures.size() && !m_albedoTextures.empty())
  {
    std::string albedoTex = m_albedoTextures[select];
    ret = albedoTex.substr(0, albedoTex.find(ASC_TEXTURE_EXT_DOT));
  }
  return ret;
}

int GraphicsManager::AddCamera()
{
  m_cameraVec.push_back(Camera());
  m_cam_idx = (int)m_cameraVec.size() - 1;
  return m_cam_idx;
}

void GraphicsManager::RemoveCamera(int handle)
{
  auto iter = m_cameraVec.begin();
  std::advance(iter, handle);
  m_cameraVec.erase(iter);

  const int size = (int)m_cameraVec.size() - 1;
  m_cam_idx = m_cam_idx + 1 > size ? 0 : m_cam_idx + 1;
}

unsigned int GraphicsManager::GetPickedObject()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_DeferredFBO);
  glReadBuffer(GL_COLOR_ATTACHMENT3);
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // Ultra-mega-over slow too, even for 1 pixel, 
  // because the framebuffer is on the GPU.
  unsigned char data[3];
  Vec2 mousePos = ASC_INPUT.GetMousePos();
  mousePos.y = ASC_SYSTEM.winHeight - mousePos.y;
  glReadPixels((int)mousePos.x, (int)mousePos.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
  unsigned int index = RGB2Int(data[0], data[1], data[2]);
  return index / PICKINGOFFSET;
}

void GraphicsManager::LoadShader(const std::string& name, const std::string& vs, const std::string& fs)
{
  if (m_shaders.find(name) != m_shaders.end()) return;

  std::vector<Shader> tmp; tmp.reserve(2);
  tmp.push_back(Shader::Load(vs, GL_VERTEX_SHADER)); // load vertex
  tmp.push_back(Shader::Load(fs, GL_FRAGMENT_SHADER)); // load fragment
  m_shaders[name] = new ShaderProgram(tmp); // link shaders
}

GraphicsManager::~GraphicsManager()
{
  for (auto &elem : m_shaders)
    delete elem.second;

  CleanUp();

  for (auto& elem : m_objLoader.m_objs)
  {
      for(size_t i =0; i < elem.second.size(); ++i)
        glDeleteVertexArrays(1, &elem.second[i].vao);
  }
  m_objLoader.m_objs.clear();

  glDeleteVertexArrays(1, &m_FSQuadVAO);
}

void GraphicsManager::Load()
{
  m_particles = &ASC_PARTICLES;

  //Loading of model
  m_models = GetFilesInDir(ASC_MODELS_FLD, "model");
  for (auto &filepath : m_models) {
    std::string name = filepath.substr(0, filepath.find(".model"));
    m_objLoader.LoadModel(name, ASC_MODELS_FLD + filepath);
  }

  //Loading of textures
  m_albedoTextures = GetFilesInDir(ASC_TEXTURES_FLD, ASC_WILDCARD, "", "_albedo");
  for (auto& str : m_albedoTextures)
  {
      str = str.substr(0, str.find("."));
      //just load all diffuse texture in the texture loader
      m_texLoader.Load(str, ASC_TEXTURES_FLD + texturesMap[str]);
  }
  m_albedoTextures.push_back("Blank");

  m_ambientTextures = GetFilesInDir(ASC_TEXTURES_FLD, ASC_WILDCARD, "", "_ambient");
  for (auto &str : m_ambientTextures) str = str.substr(0, str.find("."));
  m_ambientTextures.push_back("Blank");

  m_specularTextures = GetFilesInDir(ASC_TEXTURES_FLD, ASC_WILDCARD, "", "_specular");
  for (auto &str : m_specularTextures) str = str.substr(0, str.find("."));
  m_specularTextures.push_back("Blank");

  m_normalTextures = GetFilesInDir(ASC_TEXTURES_FLD, ASC_WILDCARD, "", "_normal");
  for (auto &str : m_normalTextures) str = str.substr(0, str.find("."));
  m_normalTextures.push_back("Blank");

  texturesMap.clear();
  textures = GetFilesInDir(ASC_TEXTURES_FLD, ASC_WILDCARD);
  for (auto& elem : textures)
  {
    std::string name = elem.substr(0, elem.find("."));
    texturesMap[name] = elem;
  }

  auto& objs = m_components.m_objects;
  for (GraphicsComp& elem : objs)
  {
      /*
    std::string albedo = elem.material.AlbedoMap;
    std::string ao = elem.material.AOMap;
    std::string GlossMap = elem.material.GlossMap;
    std::string TangentMap = elem.material.TangentMap;
    */
      
      for (size_t index = 0; index < m_objLoader.m_objs[elem.modelfile].size(); ++index)
      {
          if (elem.materialList.empty() || index >= elem.materialList.size())
          {
              MeshMaterial tempMat;
              elem.materialList.push_back(tempMat);
          }
          std::string albedo = elem.materialList[index].AlbedoMap;
          std::string ao = elem.materialList[index].AOMap;
          std::string GlossMap = elem.materialList[index].GlossMap;
          std::string TangentMap = elem.materialList[index].TangentMap;
          m_texLoader.Load(albedo, ASC_TEXTURES_FLD + texturesMap[albedo]);
          m_texLoader.Load(ao, ASC_TEXTURES_FLD + texturesMap[ao]);
          m_texLoader.Load(GlossMap, ASC_TEXTURES_FLD + texturesMap[GlossMap]);
          m_texLoader.Load(TangentMap, ASC_TEXTURES_FLD + texturesMap[TangentMap]);
      }
  }
  auto& particleList = m_particles->m_components.m_objects;
  for (auto& elem : particleList)
    m_texLoader.Load(elem.textureName, ASC_TEXTURES_FLD + texturesMap[elem.textureName]);

  /*for (auto &filepath : allTextures) {
    std::string name = filepath.substr(0, filepath.find("."));
    m_texLoader.Load(name, ASC_TEXTURES_FLD + filepath);
  }*/

  //Loading of Shaders
  std::vector<std::string> shaders = GetFilesInDir(ASC_SHADER_FLD, "vs");
  for (auto &filepath : shaders) {
    std::string name = filepath.substr(0, filepath.find("."));
    LoadShader(name, ASC_SHADER_FLD + name + ".vs", ASC_SHADER_FLD + name + ".fs");
  }
}

void GraphicsManager::Update(float dt)
{
  start = Clock::now();

  m_cameraVec[m_cam_idx].Update(dt);
  m_objLoader.Update();


//#ifndef _SUBMISSION
  if (ASC_KEYTRIGGERED(ASC_KEY_F3))
    isDebugDraw = !isDebugDraw;

 /* if (ASC_KEYTRIGGERED(ASC_KEY_F2))
    showMetric = !showMetric;*/
//#endif

  if (ASC_KEYTRIGGERED(ASC_KEY_PRINTSCREEN))
    SOIL_save_screenshot("screenshot.bmp", SOIL_SAVE_TYPE_BMP, 0, 0, ASC_SYSTEM.winWidth, ASC_SYSTEM.winHeight);

  glViewport(0, 0, ASC_SYSTEM.winWidth, ASC_SYSTEM.winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

  glEnable(GL_CULL_FACE); //Enable back face culling
  glCullFace(GL_BACK);

  PrePass();

  glBindFramebuffer(GL_FRAMEBUFFER, m_DeferredFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);// Accept fragment if it closer to the camera than the former one

  DrawSkybox();

  //Draw all objects
  GeometryPass();

  //Final Merge
  LightPass();

  glEnable(GL_BLEND);

  //Draw transparent objects
  ForwardPass();

  HighlightsPass();

  static auto hideUI = false;

#if CHEATS 
  if (ASC_KEYDOWN(ASC_KEY_LCTRL) || ASC_KEYDOWN(ASC_KEY_RCTRL))
  {
    if (ASC_KEYTRIGGERED(ASC_KEY_F12)) hideUI = !hideUI;
  }
#endif

  if (!hideUI)
  {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //RenderTrail();
    InterfacePass();
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
  }

  glDisable(GL_BLEND);

  if (isDebugDraw)
  {
#ifndef _SUBMISSION
    extern RigidBodies rb;
    for (size_t i = 0; i < rb.numCount; ++i)
      if (rb.shape[i]) rb.shape[i]->DrawDebug();

    ASC_WP.DrawWayPoints();
   // ASC_LIGHTS.DebugLights(m_objLoader["Sphere"][0]);

  
    //this must be last
    DebugLine();
  

#endif

    lineR.ClearLines();
  }

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  duration = Clock::now() - start;
}

BaseHandle GraphicsManager::Deserialize(const GameObject &gameObject, Deserializer &deserializer)
{
  GrapHandle id = m_components.GetNewObject();
  GraphicsComp &tmp = *m_components.GetObj(id);
  tmp.SetGOHandle(gameObject.GetSelfHandle());
  tmp.positionHdl = Handle<TransformComp>(gameObject[TRANSFORM]);

  deserializer.Read("IsEnable", tmp.Enabled);

  deserializer.Read("Color", tmp.tint);
  deserializer.Read("isInterface", tmp.isUI);
  deserializer.Read("FromWorld", tmp.mapFromWorld);
  deserializer.Read("UIautoScale", tmp.autoScale);
  int materialSize = 1;
  deserializer.Read("MaterialSize", materialSize);
  /*deserializer.Read("DiffuseMap", tmp.material.AlbedoMap);
      deserializer.Read("AOMap", tmp.material.AOMap);
      deserializer.Read("GlossMap", tmp.material.GlossMap);
      deserializer.Read("NormalMap", tmp.material.TangentMap);
      deserializer.Read("Roughness", tmp.material.Roughness);
      deserializer.Read("Metalic", tmp.material.Metalness);*/
  std::vector <MeshMaterial> matList;
  for (int i = 0; i < materialSize; ++i)
  {
      std::string index = std::to_string(i);
      MeshMaterial temp;
      /*if (i == 0)
      {
          deserializer.Read("DiffuseMap", temp.AlbedoMap);
          deserializer.Read("AOMap", temp.AOMap);
          deserializer.Read("GlossMap", temp.GlossMap);
          deserializer.Read("NormalMap", temp.TangentMap);
          deserializer.Read("Roughness", temp.Roughness);
          deserializer.Read("Metalic", temp.Metalness);
      }
      else*/
      {
          deserializer.Read(std::string("DiffuseMap" + index).c_str(), temp.AlbedoMap);
          deserializer.Read(std::string("AOMap" + index).c_str(), temp.AOMap);
          deserializer.Read(std::string("GlossMap" + index).c_str(), temp.GlossMap);
          deserializer.Read(std::string("NormalMap" + index).c_str(), temp.TangentMap);
          deserializer.Read(std::string("Roughness" + index).c_str(), temp.Roughness);
          deserializer.Read(std::string("Metalic" + index).c_str(), temp.Metalness);
      }
    matList.push_back(temp);
      
  }
  tmp.materialList = matList;
  deserializer.Read("Model", tmp.modelfile);
  deserializer.Read("PositionOffset", tmp.posOffset);
  deserializer.Read("ScaleOffset", tmp.scaleOffset);
  deserializer.Read("UVOffset", tmp.uvOffset);
  deserializer.Read("Tiled", tmp.tile);
  deserializer.Read("zRotation", tmp.zRotation);

  return id;
}

void GraphicsManager::Serialize(Serializer &serializer, BaseHandle index)
{
  //if (index >= m_graphicsComp.size() || m_graphicsComp.empty()) return;
  if (m_components.m_objects.empty()) return;

  // Get the current transform to serialize
  GraphicsComp &gComp = *m_components.GetObj(GrapHandle(index));// [index.Val()];

  serializer.Write("IsEnable", gComp.Enabled);

  serializer.Write("Color", gComp.tint);
  serializer.Write("isInterface", gComp.isUI);
  serializer.Write("FromWorld", gComp.mapFromWorld);
  serializer.Write("UIautoScale", gComp.autoScale);
  serializer.Write("MaterialSize", (int)gComp.materialList.size());
  for (size_t i = 0; i < gComp.materialList.size(); ++i)
  {

      std::string index = std::to_string(i);
      serializer.Write(std::string("DiffuseMap" + index).c_str(), gComp.materialList[i].AlbedoMap);
      serializer.Write(std::string("AOMap" + index).c_str(), gComp.materialList[i].AOMap);
      serializer.Write(std::string("GlossMap" + index).c_str(), gComp.materialList[i].GlossMap);
      serializer.Write(std::string("NormalMap" + index).c_str(), gComp.materialList[i].TangentMap);
      serializer.Write(std::string("Roughness" + index).c_str(), gComp.materialList[i].Roughness);
      serializer.Write(std::string("Metalic" + index).c_str(), gComp.materialList[i].Metalness);
  }
  serializer.Write("Model", gComp.modelfile);
  serializer.Write("PositionOffset", gComp.posOffset);
  serializer.Write("ScaleOffset", gComp.scaleOffset);
  serializer.Write("UVOffset", gComp.uvOffset);
  serializer.Write("Tiled", gComp.tile);
  serializer.Write("zRotation", gComp.zRotation);
}

void GraphicsManager::Reflect(BaseHandle id, const Reflector &reflector)
{
  GraphicsComp &gComp = *m_components.GetObj(GrapHandle(id));
  bool val = gComp.Enabled;
  if (ImGui::Checkbox("Is Enable##enableGraphic", &val))
  {
    gComp.Enabled = !gComp.Enabled;
  }


  int model_select = 0;

  for (model_select = 0; model_select < m_models.size(); ++model_select)
      if (m_models[model_select].find(gComp.modelfile) == 0)
          break;

  int albedo_select = 0;

  int ambient_select = 0;

  int specular_select = 0;

  int normal_select = 0;

  reflector.ColorPicker("Color", gComp.tint.m);
  reflector.Reflect("UI", gComp.isUI);
  if (gComp.isUI)
  {
    reflector.Reflect("UiAutoScale", gComp.autoScale);
    reflector.Reflect("MapFromWorld", gComp.mapFromWorld);
    ImGui::DragFloat2("PositionOffset", gComp.posOffset.m, 0.01f, 0.f, 0.f, "%.4f");
    ImGui::DragFloat("DepthOffset", &gComp.posOffset.m[2], 0.01f, -1.5f, 0.5f, "%.4f");
    ImGui::DragFloat("Rotation", &gComp.zRotation, 0.1f, 0.0f, 0.0f, "%.4f");
  }
  else
  {
    reflector.Reflect("IsHighlighted", gComp.isHighlighted);
    if (gComp.isHighlighted)
      reflector.Reflect("HighlightSize", m_highlightScale);
    reflector.Reflect("PositionOffset", gComp.posOffset);
  }

  reflector.Reflect("ScaleOffset", gComp.scaleOffset);
  reflector.Reflect("UVOffset", gComp.uvOffset);
  reflector.Reflect("Tile", gComp.tile);

  reflector.Reflect("Model", m_models, &model_select);
  for (size_t i = 0; i < gComp.materialList.size(); ++i)
  {
      std::string index = std::to_string(i);
      for (albedo_select = 0; albedo_select < m_albedoTextures.size(); ++albedo_select)
          if (m_albedoTextures[albedo_select].find(gComp.materialList[i].AlbedoMap) == 0)
              break;
      for (ambient_select = 0; ambient_select < m_ambientTextures.size(); ++ambient_select)
          if (m_ambientTextures[ambient_select].find(gComp.materialList[i].AOMap) == 0)
              break;

      for (specular_select = 0; specular_select < m_specularTextures.size(); ++specular_select)
          if (m_specularTextures[specular_select].find(gComp.materialList[i].GlossMap) == 0)
              break;
      for (normal_select = 0; normal_select < m_normalTextures.size(); ++normal_select)
          if (m_normalTextures[normal_select].find(gComp.materialList[i].TangentMap) == 0)
              break;

      ImGui::Separator();
      reflector.Reflect(std::string("AlbedoMap" + index).c_str(), m_albedoTextures, &albedo_select);
      reflector.Reflect(std::string("AmbientMap" + index).c_str(), m_ambientTextures, &ambient_select);
      reflector.Reflect(std::string("SpecularMap" + index).c_str(), m_specularTextures, &specular_select);
      reflector.Reflect(std::string("NormalMap" + index).c_str(), m_normalTextures, &normal_select);
      reflector.Reflect(std::string("Roughness" + index).c_str(), gComp.materialList[i].Roughness, 0.0f, 5.0f, 0.01f);
      reflector.Reflect(std::string("Metalic" + index).c_str(), gComp.materialList[i].Metalness, 0.0f, 1.0f, 0.01f);
      ImGui::Separator();
      if (m_models.size())
      {
         

          //if (albedo_select >= m_albedoTextures.size()) albedo_select = 0;
          if (albedo_select < m_albedoTextures.size() && !m_albedoTextures.empty())
          {
              std::string albedoTex = m_albedoTextures[albedo_select];
              gComp.materialList[i].AlbedoMap = albedoTex.substr(0, albedoTex.find(ASC_TEXTURE_EXT_DOT));
          }

          //if (ambient_select >= m_ambientTextures.size()) ambient_select = 0;
          if (ambient_select < m_ambientTextures.size() && !m_ambientTextures.empty())
          {
              std::string ambientTex = m_ambientTextures[ambient_select];
              gComp.materialList[i].AOMap = ambientTex.substr(0, ambientTex.find(ASC_TEXTURE_EXT_DOT));
          }
          //if (specular_select >= m_specularTextures.size()) specular_select = 0;
          if (specular_select < m_specularTextures.size() && !m_specularTextures.empty())
          {
              std::string specularTex = m_specularTextures[specular_select];
              gComp.materialList[i].GlossMap = specularTex.substr(0, specularTex.find(ASC_TEXTURE_EXT_DOT));
          }
          //if (normal_select >= m_normalTextures.size()) normal_select = 0;
          if (normal_select < m_normalTextures.size() && !m_normalTextures.empty())
          {
              std::string normalTex = m_normalTextures[normal_select];
              gComp.materialList[i].TangentMap = normalTex.substr(0, normalTex.find(ASC_TEXTURE_EXT_DOT));
          }
      }
      else
      {
          reflector.Reflect("Loading Model ...");
      }
  
  }
  if (m_models.size())
  {
      if (model_select >= m_models.size()) model_select = 0;

      std::string mdl = m_models[model_select];
      gComp.modelfile = mdl.substr(0, mdl.find(".model"));
  }
  else
  {
      reflector.Reflect("Loading Model ...");
  }
  ImGui::DragFloat("Gamma value", &gammaValue, 0.01f, 0.0f, 9.0f, "%.3f");
}

void GraphicsManager::InitComponent(const GameObject & go, BaseHandle hdl)
{
  GraphicsComp* newcomp = m_components[GrapHandle(hdl)];
  newcomp->SetGOHandle(go.GetSelfHandle());
  //set the transform component of the new comp game object
  newcomp->positionHdl = Handle<TransformComp>(go[TRANSFORM]);
}

void GraphicsManager::CopyComponentToComponent(GraphicsComp & target, const GraphicsComp & source)
{
  target = source;
}

void GraphicsManager::HandleMsg(MsgData_Base* msg)
{
  switch (msg->type)
  {
    case mTYPE_RESIZE: {
      Msg_Resize *t = reinterpret_cast<Msg_Resize*>(msg);

      if (t->x && t->y)
      {
        for (auto& cam : m_cameraVec)
          cam.Resize(t->x, t->y);

        CleanUp();
        SetupGBuffers();

        /*float invAsp = ((float)t->y / (float)t->x);

        if (m_uiSMat.m00 == 1.0f)
          originalScale = invAsp;

        m_uiSMat.m00 = invAsp;*/
      }
    } break;

    default: break;
  };
}

void GraphicsManager::SetupSkyBox()
{
  std::vector<std::string>list = {
    {"Daylight_rt.tga"},
    {"Daylight_lf.tga"},
    {"Daylight_up.tga"},
    {"Daylight_dn.tga"},
    {"Daylight_bk.tga"},
    {"Daylight_ft.tga"},
  };
  skyboxID = m_texLoader.LoadCubeMap(ASC_TEXTURES_FLD, list);
}

void GraphicsManager::DrawSkybox()
{
  //Disable writing to depth buffer
  glDepthMask(GL_FALSE);

  ShaderProgram *sb = m_shaders["skybox"];
  float aspRatio = (float)ASC_SYSTEM.winWidth / (float)ASC_SYSTEM.winHeight;

  sb->BindShader();
  sb->SetUniform("viewMat", m_cameraVec[m_cam_idx].GetV());
  sb->SetUniform("projMat", m_cameraVec[m_cam_idx].GetP());
  sb->SetUniform("aspRatio", aspRatio);

  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
  glBindVertexArray(m_FSQuadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray(0);

  ShaderProgram::UnbindShader();

  //Enable writing to depth buffer
  glDepthMask(GL_TRUE);
}

void GraphicsManager::SetupGBuffers()
{
  int width = ASC_SYSTEM.winWidth;
  int height = ASC_SYSTEM.winHeight;
  m_buffers.reserve(4);

  unsigned int idx = (unsigned int)m_buffers.size();
  glGenFramebuffers(1, &m_DeferredFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_DeferredFBO);

  //Position (location 0)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::RGB;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, (GLsizei)width, (GLsizei)height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_buffers[idx++].handle, 0);
  //m_buffers[0] = m_texLoader.GenFrameBuffer(TextureData::RGB, col);

  //Normal (location 1)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::RGB;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_buffers[idx++].handle, 0);

  //Albedo / Diffuse & Specular (location 2)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::RGBA;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  //glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, width, height);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_buffers[idx++].handle, 0);

  //Picking Buffer (location 3)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::RGB;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_buffers[idx++].handle, 0);

  //BRDF Buffer (location 4)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::RGBA;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  //glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, width, height);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, m_buffers[idx++].handle, 0);

  //Depth attachment (location 5)
  m_buffers.push_back(TextureData());
  m_buffers[idx].width = width;
  m_buffers[idx].height = height;
  m_buffers[idx].numChannel = TextureData::DEPTH;
  glGenTextures(1, &m_buffers[idx].handle);
  glBindTexture(GL_TEXTURE_2D, m_buffers[idx].handle);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, width, height);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_buffers[idx++].handle, 0);

  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (Status != GL_FRAMEBUFFER_COMPLETE)
    LogError("GBuffer Error Status:", Status);

  static std::vector<unsigned int> buffer = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
  };

  glDrawBuffers((int)buffer.size(), &buffer[0]);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsManager::SetupFSQuad()
{
  size_t sz = sizeof(fsquad);
  int stride = sizeof(Vec2) + sizeof(Vec2);

  /* Generate Mesh VAO */
  glGenVertexArrays(1, &m_FSQuadVAO);
  glBindVertexArray(m_FSQuadVAO);

  GLuint vertBuffer;
  glGenBuffers(1, &vertBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
  /* Copy vertex attributes to GPU */
  glBufferData(GL_ARRAY_BUFFER, sz, fsquad, GL_STATIC_DRAW);

  /* Binding Position */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);

  /* Binding UV */
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(Vec2)));

  glBindVertexArray(0);
}

void GraphicsManager::DebugGBuffers()
{
#if 0
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_DeferredFBO);

  GLsizei fbWidth = ASC_SYSTEM.fbWidth;
  GLsizei fbHeight = ASC_SYSTEM.fbHeight;
  GLsizei hWidth = (GLsizei)(ASC_SYSTEM.winWidth * 0.5f);
  GLsizei hHeight = (GLsizei)(ASC_SYSTEM.winHeight * 0.5f);

  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight,
    0, 0, hWidth, hHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight,
    0, hHeight, hWidth, hHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  glReadBuffer(GL_COLOR_ATTACHMENT2);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight,
    hWidth, hHeight, hWidth, hHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  glReadBuffer(GL_DEPTH_ATTACHMENT);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight,
    hWidth, 0, hWidth, hHeight, GL_DEPTH_BUFFER_BIT, GL_LINEAR);

  static unsigned int index = GL_COLOR_ATTACHMENT0;
  static unsigned int bit = GL_COLOR_BUFFER_BIT;
  if (ASC_KEYTRIGGERED(KEY_7)) { index = GL_COLOR_ATTACHMENT0; bit = GL_COLOR_BUFFER_BIT; }
  if (ASC_KEYTRIGGERED(KEY_8)) { index = GL_COLOR_ATTACHMENT1; bit = GL_COLOR_BUFFER_BIT; }
  if (ASC_KEYTRIGGERED(KEY_9)) { index = GL_COLOR_ATTACHMENT2; bit = GL_COLOR_BUFFER_BIT; }
  if (ASC_KEYTRIGGERED(KEY_0)) { index = GL_COLOR_ATTACHMENT3; bit = GL_COLOR_BUFFER_BIT; }
  //if (ASC_KEYTRIGGERED(KEY_0)) { index = GL_DEPTH_ATTACHMENT; bit = GL_DEPTH_BUFFER_BIT; }

  glReadBuffer(index);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight, 0, 0, fbWidth, fbHeight, bit, GL_LINEAR);
#endif
}

void GraphicsManager::PrePass()
{
  m_uiList.clear();
  m_highlightList.clear();
  m_deferredList.clear();
  m_forwardList.clear();

  Camera& cam = this->GetCamera();
  Vec3 camFw = cam.GetForward();
  Vec3 camPos = cam.GetPos();

  unsigned cullstuff = 0;
  Mat4& viewMat = cam.GetV();
  int compSize = (int)m_components.m_objects.size();
  auto& objs = m_components.m_objects;
  for (int i = 0; i < compSize; ++i)
  {
      GraphicsComp& elem = objs[i];
      TransformComp& tComp = GETRANS(elem.positionHdl);

      if (elem.isUI) {
          m_uiList.push_back(i);
          continue;
      }

      //Todo : Ritters sphere method to check if object is within camera
      std::vector<Mesh>& md = m_objLoader[elem.modelfile];
      
      for (size_t j = 0; j < md.size(); ++j)
      {
          if (md[j].isEmpty()) 
              continue;

          float maxscale = Vec3::MaxElem(tComp.scale);
          Vec3 spPosOffset = md[j].m_bs.m_pos * maxscale;
          spPosOffset = tComp.rotation * spPosOffset;
          Vec3 spPos = tComp.pos +
              spPosOffset
              + elem.posOffset;
          float spScale = maxscale * md[j].m_bs.m_r;
          //ASC_LINERENDER.AddSphere(spPos, spScale);
          Vec4 newspPos = viewMat * Vec4(spPos, 1.0f);
          //GetCamera().DrawFrustumAsViewInWorld();
          if (cam.FrustumCulling(newspPos, spScale))
          {
              ++cullstuff;
              continue;
          }

          if (objs[i].tint.a < 1.0f)
          {
              if (objs[i].tint.a > 0.0f)
                  m_forwardList.push_back(i);
          }
          else
              m_deferredList.push_back(i);

          if (objs[i].isHighlighted)
              m_highlightList.push_back(i);
      }
      //Log(cullstuff);

      std::sort(m_forwardList.begin(), m_forwardList.end(),
          [&](int lhs, int rhs)
          {
              GraphicsComp& lhsElem = m_components.m_objects[lhs];
              Vec3 lhsPos = camPos - lhsElem.positionHdl->pos;

              GraphicsComp& rhsElem = m_components.m_objects[rhs];
              Vec3 rhsPos = camPos - rhsElem.positionHdl->pos;

              return (lhsPos.MagnitudeSq() > rhsPos.MagnitudeSq());
          });

      std::sort(m_uiList.begin(), m_uiList.end(),
          [&](int lhs, int rhs)
          {
              GraphicsComp& lhsElem = m_components.m_objects[lhs];
              float lhsZ = lhsElem.posOffset.z;

              GraphicsComp& rhsElem = m_components.m_objects[rhs];
              float rhsZ = rhsElem.posOffset.z;

              return (lhsZ < rhsZ);
          });
  }
}

void GraphicsManager::ForwardPass()
{
  glDepthMask(GL_FALSE);

  auto& particleList = m_particles->sortedList;

  if (m_forwardList.empty() && particleList.empty()) return;

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  TextureData* blank = m_texLoader["BLANK"];
  ShaderProgram* flight = m_shaders["forward_lighting"];
  flight->BindShader();

  ASC_LIGHTS.BindLightBuffer("forward_lighting");

  Matrix4& camVMat = m_cameraVec[m_cam_idx].GetV();
  Matrix4& camPMat = m_cameraVec[m_cam_idx].GetP();

  flight->SetUniform("nrmMat", Matrix4::IDENTITY);
  flight->SetUniform("vMat", camVMat);
  flight->SetUniform("pMat", camPMat);

  flight->SetUniform("showMetric", (int)showMetric);
  flight->SetUniformTexture("tex_metric", m_texLoader["1M"]->handle);

  flight->SetUniform("viewMat", m_cameraVec[m_cam_idx].GetV());
  flight->SetUniform("numLights", (int)(ASC_LIGHTS.NumOfLights()));
  flight->SetUniform("exposure", m_cameraVec[m_cam_idx].GetExposure());
  flight->SetUniform("gamma", gammaValue);

  TextureData *Diffuse = nullptr;
  TextureData *AmbOcc = nullptr;
  TextureData *Gloss = nullptr;
  TextureData *Normal = nullptr;
  Matrix4 offset;

  for (auto i : m_forwardList)
    {
    GraphicsComp& elem = m_components.m_objects[i];
    TransformComp& tComp = GETRANS(elem.positionHdl);
    
    std::vector<Mesh>& md = m_objLoader[elem.modelfile];
    for (size_t j = 0; j < md.size(); ++j)
    {
        MeshMaterial& mat = elem.materialList[j];
        Diffuse = m_texLoader[mat.AlbedoMap];
        AmbOcc = m_texLoader[mat.AOMap];
        Gloss = m_texLoader[mat.GlossMap];
        Normal = m_texLoader[mat.TangentMap];

        flight->SetUniformTexture("tex_diffuse", Diffuse->handle);
        flight->SetUniformTexture("tex_gloss", Gloss->handle);
        flight->SetUniformTexture("tex_ao", AmbOcc->handle);
        flight->SetUniformTexture("tex_normal", Normal->handle);

        flight->SetUniform("roughness", mat.Roughness);
        flight->SetUniform("metalness", mat.Metalness);

        flight->SetUniform("uvOffset", elem.uvOffset);
        flight->SetUniform("tile", elem.tile);

        offset.m30 = elem.posOffset.x;
        offset.m31 = elem.posOffset.y;
        offset.m32 = elem.posOffset.z;

        flight->SetUniform("mMat", tComp.mat * offset);
        flight->SetUniform("tint", elem.tint);


        glBindVertexArray(md[j].vao);
        glDrawArrays(GL_TRIANGLES, 0, md[j].triCount);
    }
  }

  flight = m_shaders["Particle"];
  flight->BindShader();
  flight->SetUniform("vpMat", m_cameraVec[m_cam_idx].GetVP());
  for (auto& elem : particleList)
  {
    Diffuse = m_texLoader[elem->textureName];
    flight->SetUniformTexture("tex_diffuse", Diffuse->handle);
    elem->Render();
  }

  glDepthMask(GL_TRUE);
  ShaderProgram::UnbindShader();

  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsManager::GeometryPass()
{
  ShaderProgram* defGeom = m_shaders["deferred_geom"];
  defGeom->BindShader();

  defGeom->SetUniform("nrmMat", Matrix4::IDENTITY);
  defGeom->SetUniform("vMat", m_cameraVec[m_cam_idx].GetV());
  defGeom->SetUniform("pMat", m_cameraVec[m_cam_idx].GetP());

  defGeom->SetUniform("showMetric", (int)showMetric);
  defGeom->SetUniformTexture("tex_metric", m_texLoader["1M"]->handle);
  defGeom->SetUniform("gamma", gammaValue);

  TextureData *Albedo = nullptr;
  TextureData *AmbOcc = nullptr;
  TextureData *Gloss = nullptr;
  TextureData *Normal = nullptr;

  Vec3 v;
  for (auto i : m_deferredList)
  {
    
    GraphicsComp& elem = m_components.m_objects[i];
    TransformComp& tComp = GETRANS(elem.positionHdl);
    auto& gameObj = elem.GetGOHandle();
    
    if (gameObj->isEnable && elem.Enabled)
    {
      //Matrix4 newMat;
      //newMat = Matrix4::ConcatModelMat(tComp.pos - elem.posOffset, tComp.scale * elem.scaleOffset, tComp.rotation);
      std::vector<Mesh>& md = m_objLoader[elem.modelfile];

      for (size_t j = 0; j < md.size(); ++j)
      {
        Matrix4 offset;
        offset.m30 = elem.posOffset.x;
        offset.m31 = elem.posOffset.y;
        offset.m32 = elem.posOffset.z;

        
        if (elem.materialList.empty() || elem.materialList.size() < md.size())
        {
            MeshMaterial& mat = elem.material;
            Albedo = m_texLoader[mat.AlbedoMap];
            AmbOcc = m_texLoader[mat.AOMap];
            Gloss = m_texLoader[mat.GlossMap];
            Normal = m_texLoader[mat.TangentMap];

            defGeom->SetUniform("roughness", mat.Roughness);
            defGeom->SetUniform("metalness", mat.Metalness);
            elem.materialList.push_back(elem.material);
        }
        else
        {
            MeshMaterial& mat = elem.materialList[j];
            Albedo = m_texLoader[mat.AlbedoMap];
            AmbOcc = m_texLoader[mat.AOMap];
            Gloss = m_texLoader[mat.GlossMap];
            Normal = m_texLoader[mat.TangentMap];

            defGeom->SetUniform("roughness", mat.Roughness);
            defGeom->SetUniform("metalness", mat.Metalness);
        }
        
        

        defGeom->SetUniformTexture("tex_diffuse", Albedo->handle);
        defGeom->SetUniformTexture("tex_gloss", Gloss->handle);
        defGeom->SetUniformTexture("tex_ao", AmbOcc->handle);
        defGeom->SetUniformTexture("tex_normal", Normal->handle);


        defGeom->SetUniform("uvOffset", elem.uvOffset);
        defGeom->SetUniform("tile", elem.tile);

        defGeom->SetUniform("mMat", tComp.mat * offset);
        defGeom->SetUniform("tint", elem.tint);

        Int2RGB(i * PICKINGOFFSET, v.x, v.y, v.z);
        defGeom->SetUniform("pickCol", v);

        glBindVertexArray(md[j].vao);
        glDrawArrays(GL_TRIANGLES, 0, md[j].triCount);
      }
    }
  }

  //
  ShaderProgram::UnbindShader();

  //Copy depth value from GBuffer to normal depth buffer
  //using BlitFramebuffer
  GLsizei fbWidth = ASC_SYSTEM.winWidth;
  GLsizei fbHeight = ASC_SYSTEM.winHeight;

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_DeferredFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, fbWidth, fbHeight, 0, 0, fbWidth, fbHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GraphicsManager::LightPass()
{
  glDepthMask(GL_FALSE);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glClear(GL_COLOR_BUFFER_BIT);

  ShaderProgram* defLight = m_shaders["deferred_lighting"];
  defLight->BindShader();

  ASC_LIGHTS.BindLightBuffer("deferred_lighting");

  defLight->SetUniform("viewMat", m_cameraVec[m_cam_idx].GetV());
  defLight->SetUniform("numLights", (int)(ASC_LIGHTS.NumOfLights()));
  defLight->SetUniformTexture("tex_position", m_buffers[0].handle);
  defLight->SetUniformTexture("tex_normal", m_buffers[1].handle);
  defLight->SetUniformTexture("tex_diffuse", m_buffers[2].handle);
  defLight->SetUniformTexture("tex_specular", m_buffers[3].handle);
  defLight->SetUniformTexture("tex_BRDF", m_buffers[4].handle);
  defLight->SetUniformTexture("tex_depth", m_buffers[5].handle);
  defLight->SetUniform("exposure", m_cameraVec[m_cam_idx].GetExposure());
  defLight->SetUniform("gamma", gammaValue);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
  glBindVertexArray(m_FSQuadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  ShaderProgram::UnbindShader();
  glDepthMask(GL_TRUE);
}

void GraphicsManager::InterfacePass()
{
  if (m_uiList.empty()) return;

  ShaderProgram* def = m_shaders["UI"];
  def->BindShader();
  std::vector<Mesh>& md = m_objLoader["Plane"];
 
      Matrix4 matrix;

      TextureData* Albedo;


      Mat4& vpMat = GetCamera().GetVP();
      float asp = GetCamera().GetASP();
      float invASP = 1.0f / asp;
      static const float defaultASP = 1920.0f / 1200.0f;
      const float ASP_FIX = asp / defaultASP;

    for (auto i : m_uiList)
    {
          
        GraphicsComp& elem = m_components.m_objects[i];
        md = m_objLoader[elem.modelfile];
        for (size_t j = 0; j < md.size(); ++j)
        {
            if (md[j].isEmpty()) md = m_objLoader["Plane"];
            if (elem.materialList.empty())
            {
                MeshMaterial& mat = elem.material;
                Albedo = m_texLoader[mat.AlbedoMap];
                elem.materialList.push_back(mat);
            }
            else
            {

                MeshMaterial& mat = elem.materialList[0];
                Albedo = m_texLoader[mat.AlbedoMap];
            }
            Vec3 newPos;

            //for mapping to UI space
            if (elem.mapFromWorld)
            {
                Vec4 pos = vpMat * Vec4(elem.positionHdl->pos, 1.0f);

                //perspective division 
                Vec3 newos = (pos * (1.0f / pos.w)).xyz;
                //z visible range is -1.5f to 0.5f
                newPos = Vec3(newos.x, newos.y, newos.z - 0.5f);
                elem.posOffset = newPos;
            }
            else
            {
                newPos = elem.posOffset;
                //newPos.x *= invASP * ASP_FIX;
            }

            Vec2 aspCorrection = Vector2::Vec2FromAngle(elem.zRotation) * invASP;
            //Log("asp Correction : ", elem.zRotation, " : ", aspCorrection);

            Vec3 newScale(invASP, 1.0f, 1.0f);
            if (elem.autoScale) newScale.x *= Albedo->aspRatio;
            newScale = newScale * elem.scaleOffset;

            newScale.x *= ASP_FIX;

            matrix = Matrix4::ConcatModelMat(newPos, newScale, Vec3::ZAXIS, -elem.zRotation);

            def->SetUniform("finalMat", matrix);
            def->SetUniform("tint", elem.tint);

            def->SetUniformTexture("tex_diffuse", Albedo->handle);
            def->SetUniform("uvOffset", elem.uvOffset);
            def->SetUniform("tile", elem.tile);
            def->SetUniform("gamma", gammaValue);
            glBindVertexArray(md[j].vao);
            glDrawArrays(GL_TRIANGLES, 0, md[j].triCount);
        }
    }
  ShaderProgram::UnbindShader();
}

void GraphicsManager::HighlightsPass()
{
  if (m_highlightList.empty()) return;

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-3.0f, -3.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(m_highlightScale);

  glCullFace(GL_FRONT);
  Matrix4 scaleMat, offset;
  //scaleMat.m00 = scaleMat.m11 = scaleMat.m22 = m_highlightScale;

  ShaderProgram* def = m_shaders["Highlight"];
  def->BindShader();

  def->SetUniform("vpMat", m_cameraVec[m_cam_idx].GetVP());
  def->SetUniform("sMat", scaleMat);
  def->SetUniform("tint", m_highlightCol);

  std::vector<Mesh> md;

  for (auto i : m_highlightList)
  {
    GraphicsComp& elem = m_components.m_objects[i];
    md = m_objLoader[elem.modelfile];
    for (size_t j = 0; j < md.size(); ++j)
    {
        offset.m30 = elem.posOffset.x;
        offset.m31 = elem.posOffset.y;
        offset.m32 = elem.posOffset.z;



        TransformComp& tComp = GETRANS(elem.positionHdl);
        def->SetUniform("mMat", tComp.mat * offset);

        glBindVertexArray(md[j].vao);
        glDrawArrays(GL_TRIANGLES, 0, md[j].triCount);
    }
  }

  ShaderProgram::UnbindShader();

  glCullFace(GL_BACK);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GraphicsManager::DebugLine()
{
  ShaderProgram* def = m_shaders["DEFAULT"];
  def->BindShader();

  def->SetUniform("MVP", this->m_cameraVec[m_cam_idx].GetVP());
  lineR.RenderLines();

  ShaderProgram::UnbindShader();
}

void GraphicsManager::RenderTrail()
{
  ShaderProgram* def = m_shaders["DEFAULT"];
  def->BindShader();
  def->SetUniform("MVP", this->m_cameraVec[m_cam_idx].GetVP());
  trailR.Render();

  ShaderProgram::UnbindShader();
}

void GraphicsManager::SetGamma(float val)
{
    gammaValue = val;
}

float GraphicsManager::GetGamma()const
{
    return gammaValue;
}