/******************************************************************************/
/*!
\file   Action.h
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
#pragma once

class Action
{
public:
  void StartAction(const GameObject *go);

  void UndoAction();
  void RedoAction();

private:
  struct ActionFrame
  {
    GOHandle goHdl;
    TransformComp trans;
  };

  std::stack<ActionFrame> frames;
};