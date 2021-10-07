/******************************************************************************/
/*!
\file   Main.cpp
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

#include <DynLinker.h>
#include "pch.h"

#include <ostream>

void EnableMemoryLeakChecking(int breakAlloc = -1)
{
#if defined(_RELEASE)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //_crtBreakAlloc = 81672;
#endif

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
#ifndef _SUBMISSION
  EnableMemoryLeakChecking();
#endif
  bool showConsole = true;
  bool fullscreen = false;

#ifdef _SUBMISSION
  fullscreen = true;
#endif

  System sys(hInstance, SW_SHOW, false , showConsole);
  sys.SetRenderSize(1280, 720);
  sys.SetWindowSize(1280, 720);
  int val = sys.Begin(fullscreen);
  BaseShareHandle::clearData();
  return val;
}