/******************************************************************************/
/*!
\file   Action.cpp
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definitions required for Action

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

void Action::StartAction(const GameObject *go)
{
  auto goHdl = go->GetSelfHandle();
  auto transform = ASC_FACTORY.GetComponentPtr<TransformComp>(goHdl);
  auto frame = ActionFrame{ goHdl, *transform };
  frames.push(frame);
}

void Action::UndoAction()
{
  if (frames.empty())
  {
    return;
  }

  auto lastFrame = frames.top();
  auto &transformComp = *ASC_FACTORY.GetComponentPtr<TransformComp>(lastFrame.goHdl);
  transformComp = lastFrame.trans;
  frames.pop();
}

void Action::RedoAction()
{

}

