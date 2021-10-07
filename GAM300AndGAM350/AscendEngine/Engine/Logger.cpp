/******************************************************************************/
/*!
\file   Logger.cpp
\author Goh Zihe
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for Logger

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

int Print(char *buf, int val)
{
  int offset = std::sprintf(buf, "%d ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char * buf, unsigned int val)
{
  int offset = std::sprintf(buf, "%u ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, float val)
{
  int offset = std::sprintf(buf, "%f ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, double val)
{
  int offset = std::sprintf(buf, "%f ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, DWORD val)
{
  int offset = std::sprintf(buf, "%lu ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, const Vector2 val)
{
  int offset = std::sprintf(buf, "{%f, %f} ", val.x, val.y);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, const Vector3 val)
{
  int offset = std::sprintf(buf, "{%f, %f, %f} ", val.x, val.y, val.z);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, const Vector4 val)
{
  int offset = std::sprintf(buf, "{%f, %f, %f, %f} ", val.x, val.y, val.z, val.w);
  buf[offset] = ' ';
  return offset;
}

int Print(char * buf, std::string val)
{
  int offset = std::sprintf(buf, "%s ", val.c_str());
  buf[offset] = ' ';
  return offset;
}

int Print(char *buf, char *val)
{
  int offset = std::sprintf(buf, "%s ", val);
  buf[offset] = ' ';
  return offset;
}

int Print(char * buf, bool val)
{
  static char *boolean[] = { "false", "true" };
  int offset = std::sprintf(buf, "%s ", boolean[val]);
  buf[offset] = ' ';
  return offset;
}

int Print(char* buf, const char *val)
{
  int offset = std::sprintf(buf, "%s ", val);
  buf[offset] = ' ';
  return offset;
}
