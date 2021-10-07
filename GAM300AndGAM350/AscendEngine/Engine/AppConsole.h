/******************************************************************************/
/*!
\file   AppConsole.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once
#if defined(__clang__) || defined(__GNUC__)
#define IM_PRINTFARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define IM_PRINTFARGS(FMT)
#endif
// Demonstrating creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
#define APPCONSOLE AppConsole::Inst()
struct AppConsole : Singleton<AppConsole>
{
  char                  InputBuf[1024];
  ImVector<char*>       Items;
  bool                  ScrollToBottom;
  ImVector<char*>       History;
  int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
  ImVector<const char*> Commands;

  AppConsole();
  ~AppConsole();

  // Portable helpers
  static int Stricmp(const char* str1, const char* str2);
  static int Strnicmp(const char* str1, const char* str2, int n);
  static char* Strdup(const char* str);

  void ClearLog();
  void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);
  void Draw();
  void ExecCommand(const char* command_line);
  static int TextEditCallbackStub(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks

  int TextEditCallback(ImGuiTextEditCallbackData* data);

private:
  int numLogs;
  static const int MAXLOG;
};