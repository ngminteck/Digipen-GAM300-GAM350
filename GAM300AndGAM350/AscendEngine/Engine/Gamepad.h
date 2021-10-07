/******************************************************************************/
/*!
\file   Gamepad.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
  Declarations for gamepad

  Copyright (C) 2020 DigiPen Institute of Technology.
  Reproduction or disclosure of this file or its contents without the
  prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef GAMEPAD_H
#define GAMEPAD_H

enum class GAMEPAD_INPUT
{
  A,
  B,
  X,
  Y,
  DPAD_UP,
  DPAD_DOWN,
  DPAD_LEFT,
  DPAD_RIGHT,
  L_SHOULDER,
  R_SHOULDER,
  START,
  BACK,
  L_THUMBSTICK,
  R_THUMBSTICK
};

class Gamepad
{
public:
  explicit Gamepad(int playerIndex);

  void Update(); // Update gamepad state

  bool LStick_InDeadzone() const;
  bool RStick_InDeadzone() const;

  float LeftStick_X() const;
  float LeftStick_Y() const;
  float RightStick_X() const;
  float RightStick_Y() const;
  float LeftTrigger() const;
  float RightTrigger() const;

  bool GetButtonDown(GAMEPAD_INPUT) const;
  bool GetButtonUp(GAMEPAD_INPUT) const;
  bool GetButtonTrigger(GAMEPAD_INPUT) const;
  bool GetButtonReleased(GAMEPAD_INPUT) const;

  bool isAnyButtonDown() const;

  XINPUT_STATE GetState() const;
  int GetIndex() const;
  bool Connected();

  void Rumble(const float &a_fLeftMotor, const float &a_fRightMotor) const;

private:
  Gamepad& operator=(const Gamepad&) = delete;
  int TranslateGamepadInput(GAMEPAD_INPUT) const;
  void RefreshState();

  XINPUT_STATE m_State;
  int m_iGamepadIndex;

  static const int ButtonCount = 14;
  bool m_IsPreviousDown[ButtonCount];
  bool m_IsCurrentDown[ButtonCount];

  const float m_ScaleShortValueToFloat;
  const float m_ScaleByteValueToFloat;
};

static const WORD XINPUT_Buttons[] = 
{
  XINPUT_GAMEPAD_A,
  XINPUT_GAMEPAD_B,
  XINPUT_GAMEPAD_X,
  XINPUT_GAMEPAD_Y,
  XINPUT_GAMEPAD_DPAD_UP,
  XINPUT_GAMEPAD_DPAD_DOWN,
  XINPUT_GAMEPAD_DPAD_LEFT,
  XINPUT_GAMEPAD_DPAD_RIGHT,
  XINPUT_GAMEPAD_LEFT_SHOULDER,
  XINPUT_GAMEPAD_RIGHT_SHOULDER,
  XINPUT_GAMEPAD_LEFT_THUMB,
  XINPUT_GAMEPAD_RIGHT_THUMB,
  XINPUT_GAMEPAD_START,
  XINPUT_GAMEPAD_BACK
};

struct XButtonIDs
{
  XButtonIDs();

  int A, B, X, Y;
  int DPad_Up, DPad_Down, DPad_Left, DPad_Right;
  int L_Shoulder, R_Shoulder;
  int L_Thumbstick, R_Thumbstick;
  int Start;
  int Back;
};

extern XButtonIDs XButtons;

#endif