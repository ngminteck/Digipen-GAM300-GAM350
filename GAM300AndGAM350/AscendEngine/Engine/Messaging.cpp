/******************************************************************************/
/*!
\file   Messaging.cpp
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

MessagingSystem::MessagingSystem()
{
}

MessagingSystem::~MessagingSystem()
{
  while (!inbox.empty())
  {
    MsgData_Base* msg = inbox.front();
    inbox.pop();
    delete msg;
  }
}

void MessagingSystem::Update()
{
  while (!inbox.empty())
  {
    MsgData_Base* msg = inbox.front();

    if (observerMap.find(msg->type) != observerMap.end())
    {
      NotifyObservers(&observerMap[msg->type], msg);
    }

    inbox.pop();
    delete msg;
  }
  //auto goList = ASC_FACTORY.GetGameObjectList();
}

MessagingSystem::ObserverList::const_iterator MessagingSystem::Add_Observer(MsgType type, Observer fn)
{
  ObserverList* list_ptr = &observerMap[type];

  //does not check for duplicates
  observerMap[type].push_back(fn);

  return --(list_ptr->end());
}

void MessagingSystem::Remove_Observer(MsgType type, ObserverList::const_iterator iter)
{
  if (observerMap.find(type) != observerMap.end())
  {
    //erase using iterator given back to client from Add_Observer
    observerMap[type].erase(iter);
    return;
  }
  std::cerr << "MessagingSystem::Remove_Observer : Type not found, ABORT\n";
  std::abort();
}

void MessagingSystem::Remove_Observer(ReturnToken & token)
{
  Remove_Observer(token.first, token.second);
}

void MessagingSystem::Remove_Observer(ReturnTokenList & tokenList)
{
  for (auto token : tokenList)
  {
    Remove_Observer(token.first, token.second);
  }
}

void MessagingSystem::Remove_All(MsgType type)
{
  observerMap[type].clear();
}


void MessagingSystem::TestCallBack(MsgData_Base* m)
{
  std::cout << "CALLBACK called : " << m->type << "\n";
}

void MessagingSystem::TestCallBack2(MsgData_Base * m)
{
  std::cout << "CALLBACK2 called : " << m->type << "\n";
}

void MessagingSystem::NotifyObservers(ObserverList * list, MsgData_Base* msg)
{
  //if crash here, did you register using ASC_SCRIPT_REGISTER_MSG in script constructor?
  //Do not use RegisterCallBack
  for (auto& fn : *list)
  {
    fn(msg);
  }
}

