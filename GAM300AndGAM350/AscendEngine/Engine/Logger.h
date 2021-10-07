/******************************************************************************/
/*!
\file   Logger.h
\author Goh zihe
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef ASC_LOGGER_H
#define ASC_LOGGER_H

#define ASC_LOG_BUFFER_SZ (8192)

static char buf[ASC_LOG_BUFFER_SZ] = {0};
union Vector2;
union Vector3;
union Vector4;

int Print(char *buf, int val);
int Print(char *buf, unsigned int val);
int Print(char *buf, float val);
int Print(char *buf, double val);
int Print(char *buf, DWORD val);
int Print(char *buf, Vector2 val);
int Print(char *buf, Vector3 val);
int Print(char *buf, Vector4 val);
int Print(char *buf, std::string val);
int Print(char *buf, char *val);
int Print(char *buf, bool val);
//int Print(char *buf, std::string &val);
//int Print(char *buf, std::string &&val);
int Print(char *buf, const char *val);


template<typename T, typename ... Args>
int Print(char *buf, T &&val, Args && ... args)
{
  int offset = Print(buf, val);
  offset = Print(buf + offset, std::forward<Args>(args)...);
  return offset;
}

template<typename ... Args>
void Log(Args && ... args)
{
  //char buf[ASC_LOG_BUFFER_SZ];
  ZeroMemory(buf, ASC_LOG_BUFFER_SZ);
  strcpy_s(buf, "[Log] ");
  buf[6] = 1;
  buf[ASC_LOG_BUFFER_SZ - 1] = '\0';
  Print(buf + 6, std::forward<Args>(args)...);
  APPCONSOLE.AddLog(buf);
}

template<typename ... Args>
void LogWarning(Args && ... args)
{
  //char buf[ASC_LOG_BUFFER_SZ];
  ZeroMemory(buf, ASC_LOG_BUFFER_SZ);
  strcpy_s(buf, "[Warning] ");
  buf[10] = 1;
  buf[ASC_LOG_BUFFER_SZ - 1] = '\0';
  Print(buf + 10, std::forward<Args>(args)...);
  APPCONSOLE.AddLog(buf);
}

template<typename ... Args>
void LogError(Args && ... args)
{
  //char buf[ASC_LOG_BUFFER_SZ];
  ZeroMemory(buf, ASC_LOG_BUFFER_SZ);
  strcpy_s(buf, "[Error] ");
  buf[8] = 1;
  buf[ASC_LOG_BUFFER_SZ - 1] = '\0';
  Print(buf + 8, std::forward<Args>(args)...);
  APPCONSOLE.AddLog(buf);
}

#endif