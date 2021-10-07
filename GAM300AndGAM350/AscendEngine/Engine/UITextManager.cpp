/******************************************************************************/
/*!
\file   UITextManager.cpp
\author Goh zihe
\par    email: Zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for UITextManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


#include "pch.h"
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/IMGUI/imstb_truetype.h"
#endif
/*#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/IMGUI/imstb_truetype.h"*/
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

static struct
{
  struct
  {
    const char* font = R"(
            #version 420 core
            layout (location = 0) in vec3 position;
	          layout (location = 1) in vec2 texCoord0;
            uniform mat4 worldMatrix;
            uniform mat4 viewProjMatrix;
	          out vec2 uv0;
              void main()
	          {
	              gl_Position = viewProjMatrix * worldMatrix * vec4(position, 1.0);
	              uv0.x = texCoord0.x;
	              uv0.y = 1 - texCoord0.y;
	          }
        )";
  } vertex;

  struct
  {
    const char* font = R"(
          #version 420 core
          uniform sampler2D mainTex;
          uniform vec4 tint;
          in vec2 uv0;
          out vec4 fragColor;
          void main()
          {
            vec4 c = texture(mainTex, uv0);  
	          fragColor = tint * vec4(c.r, c.r, c.r, c.r);
          }
	    )";
  } fragment;
} shaders;

UITextManager::UITextManager(HandleManager<TransformComp>& transform_comps) :
  m_UImatrix(nullptr), shaderProgram(nullptr), m_transformComps(transform_comps), 
  vao(0), vertexBuffer(0), uvBuffer(0), isCheckingInput(true)
{
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_STARTCUTSCENE, &UITextManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_GAME_ENDCUTSCENE, &UITextManager::HandleMsg, this);

  std::vector<Shader> tmp;
  tmp.reserve(2);
  tmp.push_back(Shader(shaders.vertex.font, GL_VERTEX_SHADER)); // load vertex
  tmp.push_back(Shader(shaders.fragment.font, GL_FRAGMENT_SHADER)); // load fragment
  shaderProgram = new ShaderProgram(tmp); // link shaders

  const float fsquad[] = {
        /*Position*/   /*UV Coord*/
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f
  };

  size_t sz = sizeof(fsquad);
  int stride = sizeof(Vec3) + sizeof(Vec2);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vertBuffer;
  glGenBuffers(1, &vertBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
  /* Copy vertex attributes to GPU */
  glBufferData(GL_ARRAY_BUFFER, sz, fsquad, GL_STATIC_DRAW);

  /* Binding Position */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);

  /* Binding UV */
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(Vec3)));

  glBindVertexArray(0);
  glDeleteBuffers(1, &vertBuffer);
}

UITextManager::~UITextManager()
{
  delete shaderProgram;
  glDeleteVertexArrays(1, &vao);

  for (auto &elem : m_components.m_objects)
  {
    glDeleteTextures(1, &elem.fontTextureID);
  }
}

void UITextManager::Load()
{
  m_UImatrix = &ASC_GMAN.GetUIMatrix();
}

void UITextManager::Update(float dt)
{
  start = Clock::now();

#if CHEATS
  static auto hidden = false;
  if (ASC_KEYDOWN(ASC_KEY_LCTRL) || ASC_KEYDOWN(ASC_KEY_RCTRL))
  {
    if (ASC_KEYTRIGGERED(ASC_KEY_F12))
    {
      hidden = !hidden;
    }
  }

  if (hidden) return;
#endif

  for (auto &ui : m_components.m_objects)
    ui.currCol = ui.tint;
  if (isCheckingInput) CheckInput();

  
  //Log(isPaused);
  if (isCutScene && isPaused)
    fadeVal = ASC_RANGE(fadeVal - (dt * 4.0f), 0.0f, 1.0f);
  else
    fadeVal = ASC_RANGE(fadeVal + dt, 0.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);

  glDisable(GL_DEPTH_TEST);
  //glDepthMask(GL_FALSE);

  shaderProgram->BindShader();

  Matrix4 projMat = ASC_GMAN.GetCamera().GetVP();

  float asp = ASC_GMAN.GetCamera().GetASP();
  static const float defaultASP = 1920.0f / 1200.0f;
  float ASP_FIX = asp / defaultASP;

  Vec4 tmpCol;
  glBindVertexArray(vao);
  for (auto &ui : m_components.m_objects)
  {
    if (ui.Enabled)
    {
      Matrix4 offset;
      offset.m30 = ui.offset.x;// *ASP_FIX;
      offset.m31 = ui.offset.y;
      offset.m32 = ui.offset.z;

      Mat4 viewProjMatrix = ui.is2D ? (*m_UImatrix) : projMat;
      shaderProgram->SetUniform("viewProjMatrix", viewProjMatrix);

      tmpCol = ui.currCol;
      tmpCol.a = tmpCol.a * fadeVal;

      shaderProgram->SetUniformTexture("mainTex", ui.fontTextureID);
      shaderProgram->SetUniform("tint", tmpCol);

      auto& trans = *m_transformComps[ui.GetTransHdl()];
      Vec3 newScale = trans.scale * ui.scale * (ui.text.size() / 6.0f);
      //newScale.x *= ASP_FIX;

      Vec3 offsetPos = trans.pos +Vec3(newScale.x, -newScale.y, 0.0f);
      //offsetPos.x *= ASP_FIX;

      Matrix4 modelMat = Matrix4::ConcatModelMat(offsetPos, newScale, trans.rotation);
      shaderProgram->SetUniform("worldMatrix", modelMat * offset);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }
  glBindVertexArray(0);

  //glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);

  ShaderProgram::UnbindShader();
  duration = Clock::now() - start;
}

BaseHandle UITextManager::Deserialize(const GameObject& gameObject, Deserializer &deserializer)
{
  UIHandle id(CreateComponent());
  UITextComp &ui = *GetComponent(id);
  ui.SetGOHandle(gameObject.GetSelfHandle());
  ui.SetTransHdl(Handle<TransformComp>(gameObject[TRANSFORM]));

  deserializer.Read("IsEnable", ui.Enabled);
  deserializer.Read("Offset", ui.offset);
  deserializer.Read("Scale", ui.scale);
  deserializer.Read("Color", ui.tint);
  deserializer.Read("Highlight", ui.highlight);
  deserializer.Read("is2d", ui.is2D);
  deserializer.Read("Text", ui.text);
  deserializer.Read("Collider", ui.collider);

  InitFont(ui);
  return id;
}

void UITextManager::Serialize(Serializer &serializer, BaseHandle index)
{
  //if (index >= Transforms.size() || Transforms.empty()) return;
  if (m_components.m_objects.empty()) return;

  // Get the current transform to serialize
  UITextComp &ui = *GetComponent(index);
  serializer.Write("IsEnable", ui.Enabled);

  serializer.Write("Offset", ui.offset);
  serializer.Write("Scale", ui.scale);
  serializer.Write("Color", ui.tint);
  serializer.Write("Highlight", ui.highlight);
  serializer.Write("is2d", ui.is2D);
  serializer.Write("Text", ui.text);
  serializer.Write("Collider", ui.collider);
}

void UITextManager::Reflect(BaseHandle id, const Reflector &rf)
{
  UITextComp &ui = *GetComponent(id);
  bool val = ui.Enabled;
  if (ImGui::Checkbox("Is Enable##uiEnable", &val))
  {
    ui.Enabled =! ui.Enabled;
  }
  if (rf.Reflect("Text", ui.text))
  {
    InitFont(ui);
  }

  if (ui.is2D)
  {
    ImGui::DragFloat2("Position Offset", ui.offset.m, 0.01f, 0.f, 0.f, "%.4f");
    ImGui::DragFloat("DepthOffset", &ui.offset.m[2], 0.01f, -1.5f, 0.5f, "%.4f");
  }
  else
    rf.Reflect("PositionOffset", ui.offset);

  rf.Reflect("Scale", ui.scale);
  rf.Reflect("Color", ui.tint);
  rf.Reflect("Highlight", ui.highlight);
  rf.Reflect("is2d", ui.is2D);

  if (ui.is2D) 
  {
    rf.Reflect("Collider", ui.collider);
  }
}

auto ReadFile(const char* path) -> std::vector<uint8_t>
{
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open())
    LogError("Failed to open file ", path);

  auto size = file.tellg();
  file.seekg(0, std::ios::beg);
  auto bytes = std::vector<uint8_t>(size);
  file.read(reinterpret_cast<char*>(&bytes[0]), size);
  file.close();
  return bytes;
}

BaseHandle UITextManager::CreateComponent()
{
  UIHandle hdl = m_components.GetNewObject();
  UITextComp &uiTextComp = *m_components[hdl];
  glGenTextures(1, &uiTextComp.fontTextureID);
  uiTextComp.text = "Text";
  uiTextComp.is2D = false;
  InitFont(uiTextComp);
  return hdl;
}

void UITextManager::DestroyComponent(BaseHandle hdl)
{
  UIHandle uihdl(hdl);
  glDeleteTextures(1, &m_components[uihdl]->fontTextureID);
  //UITextComp& uiTextComp = *m_components[hdl];
  m_components.ReleaseObj(uihdl);
}

void UITextManager::InitComponent(const GameObject& go, BaseHandle hdl)
{
  UIHandle uihdl(hdl);
  m_components[uihdl]->SetGOHandle(go.GetSelfHandle());
  UITextComp &ui = *m_components[uihdl];
  ui.SetTransHdl(Handle<TransformComp>(go[TRANSFORM]));
}

void UITextManager::CopyComponentToComponent(UITextComp& target, const UITextComp& source)
{
  target = source;
  glGenTextures(1, &target.fontTextureID);
  InitFont(target);
}

void UITextManager::HandleMsg(MsgData_Base * msg)
{
  switch (msg->type)
  {
  case mTYPE_GAME_STARTCUTSCENE: isCutScene = true; break;
  case mTYPE_GAME_ENDCUTSCENE: isCutScene = false; break;

  case mTYPE_GAME_PAUSE: isPaused = false; break;
  case mTYPE_GAME_RESUME: isPaused = true; break;

  default: break;
  }
}

void UITextManager::InitFont(UITextComp &uiTextComp) const
{
  const std::string &text = uiTextComp.text;
  uiTextComp.textureWidth = static_cast<uint32_t>(text.size()) *  static_cast<uint32_t>(uiTextComp.size);
  uiTextComp.textureHeight = uiTextComp.textureWidth;
  auto fontData = ReadFile(ASC_FONT_FILE(Lato_Regular.ttf));
  
  stbtt_fontinfo fontInfo;
  stbtt_InitFont(&fontInfo, fontData.data(), 0);
  
  int ascent, descent, lineGap, x = 0;
  stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

  auto scale = stbtt_ScaleForPixelHeight(&fontInfo, uiTextComp.size);
  ascent = static_cast<int>(static_cast<float>(ascent) * scale);
  //uiTextComp.scale = text.size() / 4.0f;

  auto pixels = std::make_unique<uint8_t[]>(uiTextComp.textureWidth * uiTextComp.textureHeight);

  // Generate text
  for (size_t i = 0; i < text.size(); ++i)
  {
    int x1, y1, x2, y2;
	stbtt_GetCodepointBitmapBox(&fontInfo, text[i], scale, scale, &x1, &y1, &x2, &y2);

    auto y = ascent + y1;

    int byteOffset = x + y  * uiTextComp.textureWidth;

    stbtt_MakeCodepointBitmap(&fontInfo, pixels.get() + byteOffset, x2 - x1, y2 - y1, uiTextComp.textureWidth, scale, scale, text[i]);

    int ax;
    stbtt_GetCodepointHMetrics(&fontInfo, text[i], &ax, nullptr);
    x += static_cast<int>(static_cast<float>(ax) * scale);

    if (i != text.size() - 1)
    {
      auto kern = stbtt_GetCodepointKernAdvance(&fontInfo, text[i], text[i + 1]);
      x += static_cast<int>(static_cast<float>(kern) * scale);
    }
  }

  //glDeleteTextures(1, &uiTextComp.textureID);
  //glGenTextures(1, &uiTextComp.textureID);
  glBindTexture(GL_TEXTURE_2D, uiTextComp.fontTextureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, uiTextComp.textureWidth, uiTextComp.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.get());
  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
  glActiveTexture(GL_TEXTURE0);
}

void UITextManager::CheckInput()
{
  float aspRatio = ASC_GMAN.GetCamera().GetASP();
  Vec2 ratio = ASC_GMAN.GetCamera().GetMouseScreenRatio();
  ratio.x *= (orthResolution * aspRatio);
  ratio.y *= orthResolution;

  for (auto &ui : m_components.m_objects)
  {
    if (ui.collider.x == 0.0f && ui.collider.y == 0.0f || ui.tint.a <= 0.0f)
      continue;

    //auto &trans = *m_transformComps[ui.GetTransHdl()];
    Vec3 aabbmin = -Vec3(ui.collider * 0.5f);
    Vec3 aabbmax = Vec3(ui.collider * 0.5f);
    Vec3 colliderOffset = ui.offset + Vec3(aabbmax.x, aabbmin.y, 0);
    aabbmin += colliderOffset;
    aabbmax += colliderOffset;

    bool hit = false;
    if (ratio.x > aabbmin.x && ratio.x < aabbmax.x &&
      ratio.y > aabbmin.y && ratio.y < aabbmax.y)
    {
      ui.currCol = ui.highlight;
      ui.isMouseOver = true;
    }
    else
      ui.isMouseOver = false;
  }
}

void UITextManager::EnableInputCheck(bool toggle)
{
  isCheckingInput = toggle;
}
