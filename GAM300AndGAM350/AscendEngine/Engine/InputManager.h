/******************************************************************************/
/*!
\file   InputManager.h
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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#define ASC_INPUT InputManager::Inst()

#define ASC_MAX_KEYS 256

#define ASC_CONTROLLER1 ASC_INPUT.GetGamepad(1)
#define ASC_LEFTTRIGGER1 ASC_INPUT.GetGamepad(1).LeftTrigger()
#define ASC_RIGHTTRIGGER1 ASC_INPUT.GetGamepad(1).RightTrigger()

#define ASC_CONTROLLER2 ASC_INPUT.GetGamepad(2)
#define ASC_LEFTTRIGGER2 ASC_INPUT.GetGamepad(2).LeftTrigger()
#define ASC_RIGHTTRIGGER2 ASC_INPUT.GetGamepad(2).RightTrigger()

#define ASC_KEYTRIGGERED(x) ASC_INPUT.GetKeyTriggered(x)
#define ASC_KEYDOWN(x) ASC_INPUT.GetKeyDown(x)
#define ASC_KEYRELEASED(x) ASC_INPUT.GetKeyReleased(x)
#define ASC_ANYKEYPRESSED() ASC_INPUT.AnyKeyPressed() || ASC_CONTROLLER1.isAnyButtonDown()
#define ASC_NOKEYPRESSED() ASC_INPUT.NoKeyPressed() || !ASC_CONTROLLER1.isAnyButtonDown()

class InputManager : public Singleton<InputManager>
{
public:
  InputManager();
  ~InputManager() {}

  void Initialize();
  void Unload();
  void Update(float dt);

  void ClearBuffer();
  bool GetKeyTriggered(unsigned char key);
  bool GetKeyDown(unsigned char key);
  bool GetKeyReleased(unsigned char key);
  void DisplayCursor(bool mode);
  void EnableInput(bool state);

  // Setters
  int& GetMouseWheel();
  void CentreMouse();


  // Gettors
  Gamepad& GetGamepad(int gamepadIndex);
  Vec2 GetMousePos();
  int GetMouseX();
  int GetMouseY();
  bool IsCursorVisible() const;
  bool AnyKeyPressed() const; // check if any key is pressed
  bool NoKeyPressed() const;
  bool InputEnabled() const;
  int GetMouseWheel() const; // Returns in multiples of 120
  void SetMouseWheel(short val);

  void SetHoveredCursor(std::string imageFile= "");


private:
  std::vector<Gamepad> m_Gamepad;
  POINT m_mousePos;
  bool m_isCursorVisible;
  bool m_anyKeyPressed;
  bool m_inputState;
  int m_prevmouseWheel;
  int m_mouseWheel;

  bool m_previousKeys[ASC_MAX_KEYS]; // hold state of all keys in previous frame
  bool m_currentKeys[ASC_MAX_KEYS];  // hold state of all keys in current frame
};

#endif