/******************************************************************************/
/*!
\file   System.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for System

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef AE_SYS_H
#define AE_SYS_H

#define ASC_MAX_LOADSTRING 100

struct GameState
{
  float x;      // position
  float v;      // velocity
};

#define ASC_SYSTEM System::Inst()
//Handle all dirty window fn calls
class System : public Singleton<System>
{
public:
  System(HINSTANCE, int, bool menu, bool console);
  ~System();

  int Begin(bool isFullscreen);

  void ToggleFullscreen();
  bool GetFullScreen() const;

  void SetWindowSize(int Width, int Height);
  void SetRenderSize(int Width, int Height);
  void GetWindowPos(int& x, int& y) const;

  bool isInFocus, isConsoleActive, isFullScreen;
  int winWidth, winHeight;
  int fbWidth, fbHeight;

  float GetDT() const;


  bool hack_releasemode_submission = false;

private:
  int nCmdShow;

  HINSTANCE hInst;                                // current instance
  MessagingSystem msg_man;
  AppConsole appConsole;

  HDC hDC;
  HWND hWnd;
  char szTitle[ASC_MAX_LOADSTRING];                  // The title bar text
  char szWindowClass[ASC_MAX_LOADSTRING];            // the main window class name
  HACCEL hAccelTable;

  ATOM RegisterWindow(HINSTANCE, bool showMenu) const;
  bool InitInstance(HINSTANCE, int);
  bool InitalizeGL() const;

  void ProcessWindowsMessages(MSG&) const;
  PIXELFORMATDESCRIPTOR pfd;
  unsigned int pixelFormat;

  void Update(GameState &state);
  void Render(const GameState &state);
  static GameState Interpolate(const GameState &current, const GameState &previous, float alpha);

  int fsWidth, fsHeight, defaultWidth, defaultHeight;
  float dt;

};

#endif
