/******************************************************************************/
/*!
\file   Timer.h
\author Goh Zihe
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for Timer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

struct CallBack
{
  CallBack(float duration, const std::function<void()> &fn) : id(0), durationLeft(duration), fn(fn) {}
  size_t id;
  float durationLeft;
  std::function<void()> fn;

  friend bool operator==(const CallBack &lhs, const CallBack &rhs);
};

class Timer
{
  static std::list<CallBack> callBacks;
  static std::stack<CallBack> toRemove;

public:
  static void Start(float duration, const std::function<void()> &fn);
  static void Update(float dt);
};

bool operator==(const CallBack &lhs, const CallBack &rhs);