/******************************************************************************/
/*!
\file   GUI.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains multiple definitions required for GUIManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

//#include "IMGUI/imgui.cpp"
//#include "IMGUI/imgui_draw.cpp"
//#include "IMGUI/ImGuizmo.cpp"

#define Imgui ImGui

static int          m_ShaderHandle = 0, m_VertHandle = 0, m_FragHandle = 0;
static int          m_AttribLocationTex = 0, m_AttribLocationProjMtx = 0;
static int          m_AttribLocationPosition = 0, m_AttribLocationUV = 0, m_AttribLocationColor = 0;
static unsigned int m_VboHandle, m_VaoHandle = 0, m_ElementsHandle = 0;

ImguiWrapper::ImguiWrapper(HWND hWnd)
{
  Init(hWnd);
  CreateDeviceObjects();

  m_GUIstyle = &(ImGui::GetStyle());
  //Set the GUI to non round edges
  m_GUIstyle->WindowRounding = 1.0f;
  //m_GUIstyle->Colors[0] 
}

ImguiWrapper::~ImguiWrapper()
{
  Shutdown();
}

void ImguiWrapper::Update(float dt)
{
  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  //RECT rect;
  //GetClientRect(g_hWnd, &rect);
  //io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
  io.DisplaySize = ImVec2((float)ASC_SYSTEM.winWidth, (float)ASC_SYSTEM.winHeight);

  // Setup time step
  INT64 current_time;
  QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
  currTime = (double)current_time;
  io.DeltaTime = dt;

  // Read keyboard modifiers inputs
  io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
  io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
  io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
  // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
  // io.MousePos : filled by WM_MOUSEMOVE events
  // io.MouseDown : filled by WM_*BUTTON* events
  // io.MouseWheel : filled by WM_MOUSEWHEEL events

  // Hide OS mouse cursor if ImGui is drawing it
  //SetCursor(io.MouseDrawCursor ? NULL : LoadCursorFromFile("./Assets/Textures/Cursor_albedo.cur"));// LoadCursor(NULL, IDC_ARROW));
}

bool ImguiWrapper::Init(HWND hwnd)
{
  m_hWnd = hwnd;

  ImGuiIO& io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
  io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
  io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
  io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
  io.KeyMap[ImGuiKey_Home] = VK_HOME;
  io.KeyMap[ImGuiKey_End] = VK_END;
  io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
  io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
  io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
  io.KeyMap[ImGuiKey_A] = 'A';
  io.KeyMap[ImGuiKey_C] = 'C';
  io.KeyMap[ImGuiKey_V] = 'V';
  io.KeyMap[ImGuiKey_X] = 'X';
  io.KeyMap[ImGuiKey_Y] = 'Y';
  io.KeyMap[ImGuiKey_Z] = 'Z';

  io.RenderDrawListsFn = RenderDrawLists;  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
  //io.ImeWindowHandle = m_hWnd;

  return true;
}

void ImguiWrapper::RenderDrawLists(ImDrawData* draw_data)
{
  // Backup GL state
  GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
  GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
  GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glActiveTexture(GL_TEXTURE0);

  // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
  ImGuiIO& io = ImGui::GetIO();
  float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Setup orthographic projection matrix
  const float ortho_projection[4][4] =
  {
    { 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
    { 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { -1.0f, 1.0f, 0.0f, 1.0f },
  };
  glUseProgram(m_ShaderHandle);
  glUniform1i(m_AttribLocationTex, 0);
  glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
  glBindVertexArray(m_VaoHandle);

  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

    for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
    {
      if (pcmd->UserCallback)
      {
        pcmd->UserCallback(cmd_list, pcmd);
      }
      else
      {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  glUseProgram(last_program);
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  glBindVertexArray(last_vertex_array);
  glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  glBlendFunc(last_blend_src, last_blend_dst);
  if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
}

void ImguiWrapper::CreateGUIFont()
{
  ImGuiIO& io = ImGui::GetIO();

  // Build texture atlas
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

  // Create OpenGL texture
  glGenTextures(1, &m_FontTexture);
  glBindTexture(GL_TEXTURE_2D, m_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)m_FontTexture;

  // Cleanup (don't clear the input data if you want to append new fonts later)
  io.Fonts->ClearInputData();
  io.Fonts->ClearTexData();
}

bool ImguiWrapper::CreateDeviceObjects()
{
  // Backup GL state
  GLint last_texture, last_array_buffer, last_vertex_array;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

  const GLchar *vertex_shader =
    "#version 420\n"
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "  Frag_UV = UV;\n"
    "  Frag_Color = Color;\n"
    "  gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

  const GLchar* fragment_shader =
    "#version 420\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "  Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";

  m_ShaderHandle = glCreateProgram();
  m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
  m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_VertHandle, 1, &vertex_shader, 0);
  glShaderSource(m_FragHandle, 1, &fragment_shader, 0);
  glCompileShader(m_VertHandle);
  glCompileShader(m_FragHandle);
  glAttachShader(m_ShaderHandle, m_VertHandle);
  glAttachShader(m_ShaderHandle, m_FragHandle);
  glLinkProgram(m_ShaderHandle);

  m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
  m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
  m_AttribLocationPosition = glGetAttribLocation(m_ShaderHandle, "Position");
  m_AttribLocationUV = glGetAttribLocation(m_ShaderHandle, "UV");
  m_AttribLocationColor = glGetAttribLocation(m_ShaderHandle, "Color");

  glGenBuffers(1, &m_VboHandle);
  glGenBuffers(1, &m_ElementsHandle);

  glGenVertexArrays(1, &m_VaoHandle);
  glBindVertexArray(m_VaoHandle);
  glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
  glEnableVertexAttribArray(m_AttribLocationPosition);
  glEnableVertexAttribArray(m_AttribLocationUV);
  glEnableVertexAttribArray(m_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
  glVertexAttribPointer(m_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(m_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(m_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

  CreateGUIFont();

  // Restore modified GL state
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindVertexArray(last_vertex_array);

  return true;
}

void ImguiWrapper::Shutdown()
{
  if (m_VaoHandle) glDeleteVertexArrays(1, &m_VaoHandle);
  if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
  if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
  m_VaoHandle = m_VboHandle = m_ElementsHandle = 0;

  glDetachShader(m_ShaderHandle, m_VertHandle);
  glDeleteShader(m_VertHandle);
  m_VertHandle = 0;

  glDetachShader(m_ShaderHandle, m_FragHandle);
  glDeleteShader(m_FragHandle);
  m_FragHandle = 0;

  glDeleteProgram(m_ShaderHandle);
  m_ShaderHandle = 0;

  if (m_FontTexture)
  {
    glDeleteTextures(1, &m_FontTexture);
    ImGui::GetIO().Fonts->TexID = 0;
    m_FontTexture = 0;
  }
 // ImGui::Shutdown();
  ImGui::DestroyContext();
}


IMGUI_API LRESULT GUIWndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  switch (msg)
  {
  case WM_LBUTTONDOWN:
    io.MouseDown[0] = true;
    return true;
  case WM_LBUTTONUP:
    io.MouseDown[0] = false;
    return true;
  case WM_RBUTTONDOWN:
    io.MouseDown[1] = true;
    return true;
  case WM_RBUTTONUP:
    io.MouseDown[1] = false;
    return true;
  case WM_MBUTTONDOWN:
    io.MouseDown[2] = true;
    return true;
  case WM_MBUTTONUP:
    io.MouseDown[2] = false;
    return true;
  case WM_MOUSEWHEEL:
    io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
    ASC_INPUT.SetMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
    return true;
  case WM_MOUSEMOVE:
    io.MousePos.x = (signed short)(lParam);
    io.MousePos.y = (signed short)(lParam >> 16);
    return true;
  case WM_KEYDOWN:
    if (wParam < 256)
      io.KeysDown[wParam] = 1;
    return true;
  case WM_KEYUP:
    if (wParam < 256)
      io.KeysDown[wParam] = 0;
    return true;
  case WM_CHAR:
    // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
    if (wParam > 0 && wParam < 0x10000)
      io.AddInputCharacter((unsigned short)wParam);
    return true;
  }
  return 0;
}

