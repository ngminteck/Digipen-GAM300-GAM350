/******************************************************************************/
/*!
\file   GUI.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
  Declarations for GUI

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef GUI_H
#define GUI_H

class ImguiWrapper
{
public:
  ImguiWrapper(HWND);
  ~ImguiWrapper();

  void Update(float dt);
  
private:

  HWND m_hWnd;
  double currTime;
  GLuint       m_FontTexture;
  ImGuiStyle  *m_GUIstyle;

  bool Init(HWND hwnd);
  static void RenderDrawLists(ImDrawData* draw_data);

  void CreateGUIFont();
  bool CreateDeviceObjects();

  void Shutdown();
};

IMGUI_API LRESULT GUIWndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
