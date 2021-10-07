/******************************************************************************/
/*!
\file   Timer.cpp
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
#include  "pch.h"

std::list<CallBack> Timer::callBacks;
std::stack<CallBack> Timer::toRemove;
static size_t index = 0;

void Timer::Start(float duration, const std::function<void()> &fn)
{
  callBacks.emplace_back(duration, fn);
}

void Timer::Update(float dt)
{
  size_t id = 0;
  for (auto &elem : callBacks)
  {
    elem.durationLeft -= dt;
    elem.id = id++;
    if (elem.durationLeft < 0.0f)
    {
      toRemove.push(elem);
      elem.fn();
    }
  }

  while (!toRemove.empty())
  {
    callBacks.remove(toRemove.top());
    toRemove.pop();
  }
}

bool operator==(const CallBack &lhs, const CallBack &rhs)
{
  return lhs.id == rhs.id;
}
