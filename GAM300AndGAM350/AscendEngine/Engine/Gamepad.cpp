/******************************************************************************/
/*!
\file   Gamepad.cpp
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
  Contains multiple definitions required for Gamepad

  Copyright (C) 2020 DigiPen Institute of Technology.
  Reproduction or disclosure of this file or its contents without the
  prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

XButtonIDs XButtons;

XButtonIDs::XButtonIDs()
{
  A = 0;
  B = 1;
  X = 2;
  Y = 3;

  DPad_Up = 4;
  DPad_Down = 5;
  DPad_Left = 6;
  DPad_Right = 7;

  L_Shoulder = 8;
  R_Shoulder = 9;

  L_Thumbstick = 10;
  R_Thumbstick = 11;

  Start = 12;
  Back = 13;
}

Gamepad::Gamepad(int playerIndex) :
m_iGamepadIndex(playerIndex - 1),
m_ScaleShortValueToFloat(1.0f / 32768),
m_ScaleByteValueToFloat(1.0f / 255)
{
  // Iterate through all gamepad buttons
  for (int i = 0; i < ButtonCount; i++)
  {
    m_IsPreviousDown[i] = false;
    m_IsCurrentDown[i] = false;
  }
}

XINPUT_STATE Gamepad::GetState() const
{
  XINPUT_STATE GamepadState;

  ZeroMemory(&GamepadState, sizeof(XINPUT_STATE));

  XInputGetState(m_iGamepadIndex, &GamepadState);

  return GamepadState;
}

int Gamepad::GetIndex() const
{
  return m_iGamepadIndex;
}

bool Gamepad::Connected()
{
  ZeroMemory(&m_State, sizeof(XINPUT_STATE));

  DWORD Result = XInputGetState(m_iGamepadIndex, &m_State);

  if (Result == ERROR_SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Gamepad::Update()
{
  if (Connected())
  {
    RefreshState();

    m_State = GetState();

    for (int i = 0; i < ButtonCount; ++i)
    {
      // Sets the button state for the current frame
      m_IsCurrentDown[i] = ((m_State.Gamepad.wButtons & XINPUT_Buttons[i]) == XINPUT_Buttons[i]);
    }
  }
}

void Gamepad::RefreshState()
{
  memcpy(m_IsPreviousDown, m_IsCurrentDown, sizeof(m_IsPreviousDown));
}

bool Gamepad::LStick_InDeadzone() const
{
  //short sX = m_State.Gamepad.sThumbLX;
  //short sY = m_State.Gamepad.sThumbLY;

  //// Check if the axes are outside the deadzone
  //if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
  //  sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
  //{
  //  return false;
  //}

  //if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
  //  sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
  //{
  //  return false;
  //}

  //// One (or both axes) axis is inside the deadzone
  //return false;
  float Lx = fabs(LeftStick_X());
  float Ly = fabs(LeftStick_Y());
  return (Lx < 0.3f && Ly < 0.3f);
}

bool Gamepad::RStick_InDeadzone() const
{
  //short sX = m_State.Gamepad.sThumbRX;
  //short sY = m_State.Gamepad.sThumbRY;

  //// Check if the axes are outside the deadzone
  //if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
  //  sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
  //{
  //  return false;
  //}

  //if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
  //  sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
  //{
  //  return false;
  //}

  //// One (or both axes) axis is inside the deadzone
  //return false;
  float Rx = fabs(RightStick_X());
  float Ry = fabs(RightStick_Y());
  return (Rx < 0.3f && Ry < 0.3f);
}

float Gamepad::LeftStick_X() const
{
  short sX = m_State.Gamepad.sThumbLX;

  return (static_cast<float>(sX)* m_ScaleShortValueToFloat);
}

float Gamepad::LeftStick_Y() const
{
  short sY = m_State.Gamepad.sThumbLY;

  return (static_cast<float>(sY)* m_ScaleShortValueToFloat);
}

float Gamepad::RightStick_X() const
{
  short sX = m_State.Gamepad.sThumbRX;

  return (static_cast<float>(sX)* m_ScaleShortValueToFloat);
}

float Gamepad::RightStick_Y() const
{
  short sY = m_State.Gamepad.sThumbRY;

  return (static_cast<float>(sY)* m_ScaleShortValueToFloat);
}

int Gamepad::TranslateGamepadInput(GAMEPAD_INPUT input) const
{
  switch (input)
  {
  case GAMEPAD_INPUT::A:
    return XButtons.A;
  case GAMEPAD_INPUT::B:
    return XButtons.B;
  case GAMEPAD_INPUT::X:
    return XButtons.X;
  case GAMEPAD_INPUT::Y:
    return XButtons.Y;
  case GAMEPAD_INPUT::DPAD_UP:
    return XButtons.DPad_Up;
  case GAMEPAD_INPUT::DPAD_DOWN:
    return XButtons.DPad_Down;
  case GAMEPAD_INPUT::DPAD_LEFT:
    return XButtons.DPad_Left;
  case GAMEPAD_INPUT::DPAD_RIGHT:
    return XButtons.DPad_Right;
  case GAMEPAD_INPUT::L_SHOULDER:
    return XButtons.L_Shoulder;
  case GAMEPAD_INPUT::R_SHOULDER:
    return XButtons.R_Shoulder;
  case GAMEPAD_INPUT::START:
    return XButtons.Start;
  case GAMEPAD_INPUT::BACK:
    return XButtons.Back;
  case GAMEPAD_INPUT::L_THUMBSTICK:
    return XButtons.L_Thumbstick;
  case GAMEPAD_INPUT::R_THUMBSTICK:
    return XButtons.R_Thumbstick;
  }

  return 0; // Not Supposed to happen
}

float Gamepad::LeftTrigger() const
{
  BYTE Trigger = m_State.Gamepad.bLeftTrigger;

  if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
  {
    return Trigger * m_ScaleByteValueToFloat;
  }

  return 0.0f;
}

float Gamepad::RightTrigger() const
{
  BYTE Trigger = m_State.Gamepad.bRightTrigger;

  if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
  {
    return Trigger * m_ScaleByteValueToFloat;
  }

  return 0.0f;
}

bool Gamepad::GetButtonDown(GAMEPAD_INPUT input) const
{
  if (m_State.Gamepad.wButtons & XINPUT_Buttons[TranslateGamepadInput(input)])
  {
    return true; // Button down
  }
  return false; // Button up
}

bool Gamepad::GetButtonUp(GAMEPAD_INPUT input) const
{
  if (m_State.Gamepad.wButtons & XINPUT_Buttons[TranslateGamepadInput(input)])
  {
    return false; // Button down
  }
  return true; // Button up
}

bool Gamepad::GetButtonTrigger(GAMEPAD_INPUT input) const
{
  if (!m_IsPreviousDown[TranslateGamepadInput(input)] &&
    m_IsCurrentDown[TranslateGamepadInput(input)])
  {
    return true;
  }
  return false;
}

bool Gamepad::GetButtonReleased(GAMEPAD_INPUT input) const
{
  if (m_IsPreviousDown[TranslateGamepadInput(input)] &&
    !m_IsCurrentDown[TranslateGamepadInput(input)])
  {
    return true;
  }
  return false;
}

bool Gamepad::isAnyButtonDown() const
{
  for (int i = 0; i < ButtonCount; ++i)
  {
    if (m_IsCurrentDown[i]) return true;
  }

  return false;
}

void Gamepad::Rumble(const float &a_fLeftMotor = 0.0f, const float &a_fRightMotor = 0.0f) const
{
  XINPUT_VIBRATION VibrationState;

  ZeroMemory(&VibrationState, sizeof(XINPUT_VIBRATION));

  VibrationState.wLeftMotorSpeed = (WORD) int(a_fLeftMotor * 65535.0f);
  VibrationState.wRightMotorSpeed = (WORD) int(a_fRightMotor * 65535.0f);

  XInputSetState(m_iGamepadIndex, &VibrationState);
}