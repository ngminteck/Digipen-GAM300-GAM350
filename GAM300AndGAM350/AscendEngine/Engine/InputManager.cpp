/******************************************************************************/
/*!
\file   InputManager.cpp
\author James Leong
\par    email: james.leong\@digipen.edu
\par    Digipen login: james.leong
\date   02/02/2020
\brief
Contains definitions required for InputManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

InputManager::InputManager() :
  m_Gamepad({Gamepad(1), Gamepad(2), Gamepad(3), Gamepad(4)}),
  m_isCursorVisible(true),
  m_anyKeyPressed(false),
  m_inputState(true), m_prevmouseWheel(0),
  m_mouseWheel(0)
{
}

void InputManager::Initialize()
{
  DisplayCursor(true);
  m_anyKeyPressed = false;
  m_inputState = true;
  m_prevmouseWheel = m_mouseWheel = 0;
}

void InputManager::Update(float dt)
{
  if (!m_inputState)
    return;

  m_anyKeyPressed = false;

  if (m_prevmouseWheel != 0)
    m_mouseWheel = 0;
  m_prevmouseWheel = m_mouseWheel;


  /* Start of Gamepad Update */
  for (auto &elem : m_Gamepad) elem.Update();
  /* End of Gamepad Update*/

  for (int i = 0; i < ASC_MAX_KEYS; ++i)
  {
    // copy current state to previous state
    m_previousKeys[i] = m_currentKeys[i];

    // then update the current state
    if (GetAsyncKeyState(i) & 0x8000)
    {
      // if key is pressed currently, update value to 1
      m_currentKeys[i] = true;
      m_anyKeyPressed = true;
    }
    else
    {
      m_currentKeys[i] = false;
    }
  }
}

void InputManager::Unload()
{
  ClearBuffer();
}

void InputManager::ClearBuffer()
{
  m_mouseWheel = 0;
  for (int i = 0; i < ASC_MAX_KEYS; ++ i)
    m_previousKeys[i] = m_currentKeys[i] = 0;
}

bool InputManager::GetKeyTriggered(unsigned char key)
{
  if (!m_inputState) return false;
  // Check if key is between 0 and 255 before using it to make it safe
  return (m_currentKeys[key] && !m_previousKeys[key]);
}

bool InputManager::GetKeyDown(unsigned char key)
{
  if (!m_inputState) return false;
  // Check if key is between 0 and 255 before using it to make it safe
  return m_currentKeys[key]; // as long as key is held down, its pressed!
}

bool InputManager::GetKeyReleased(unsigned char key)
{
  if (!m_inputState) return false;
  // if key is held up in current frame and not held up in previous frame, then its released!
  return (!m_currentKeys[key] && m_previousKeys[key]);
}

void InputManager::DisplayCursor(bool mode)
{
  // display/hide mouse cursor
  if (mode != m_isCursorVisible)
  {
    ShowCursor(mode);
    m_isCursorVisible = mode;
  }
}

void InputManager::EnableInput(bool state)
{
  m_inputState = state;
}

int& InputManager::GetMouseWheel()
{
  return m_mouseWheel;
}

void InputManager::CentreMouse()
{
  int x, y;
  ASC_SYSTEM.GetWindowPos(x, y);
  x += ASC_SYSTEM.winWidth / 2;
  y += ASC_SYSTEM.winHeight / 2;
  SetCursorPos((int)x, (int)y);
}

Gamepad& InputManager::GetGamepad(int gamepadIndex)
{
  return m_Gamepad[gamepadIndex - 1];
}

int InputManager::GetMouseX()
{
  GetCursorPos(&m_mousePos); // get x and y coordinates of mouse cursor
  ScreenToClient(GetForegroundWindow(), &m_mousePos); // convert mouse cursor coords to current window coords
  return static_cast<int>(m_mousePos.x);
}

int InputManager::GetMouseY()
{
  GetCursorPos(&m_mousePos); // get x and y coordinates of mouse cursor
  ScreenToClient(GetForegroundWindow(), &m_mousePos); // convert mouse cursor coords to current window coords
  return static_cast<int>(m_mousePos.y);
}

bool InputManager::IsCursorVisible() const
{
  return m_isCursorVisible;
}

bool InputManager::AnyKeyPressed() const
{
  return m_anyKeyPressed;
}

bool InputManager::NoKeyPressed() const
{
  return !m_anyKeyPressed;
}

bool InputManager::InputEnabled() const
{
  return m_inputState;
}

int InputManager::GetMouseWheel() const
{
  return m_mouseWheel;
}

void InputManager::SetMouseWheel(short delta_val)
{
  //m_mouseWheel += delta_val;
  m_mouseWheel = delta_val;
  //Log("wheel", m_mouseWheel);
}

Vec2 InputManager::GetMousePos()
{
  GetCursorPos(&m_mousePos); // get x and y coordinates of mouse cursor
  ScreenToClient(GetForegroundWindow(), &m_mousePos); // convert mouse cursor coords to current window coords
  return Vec2(m_mousePos.x, m_mousePos.y);
}


void InputManager::SetHoveredCursor(std::string imageFile)
{
    std::string text = "./Assets/Textures/" + imageFile;

    //std::string text = "./Assets/Textures/HoveredCursor_albedo.cur";
    HCURSOR cursor = (imageFile == "") ? LoadCursor(NULL, IDC_ARROW) : LoadCursorFromFile(text.c_str());
    SetCursor(cursor);
}
