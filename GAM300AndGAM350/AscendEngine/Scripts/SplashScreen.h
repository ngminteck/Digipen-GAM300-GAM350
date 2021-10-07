/******************************************************************************/
/*!
\file   SplashScreen.h
\author Zihe Goh
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

class SplashScreen : public SCRIPTING::ScriptCommonIF<SplashScreen>
{
public:
  SplashScreen();
  void RotateUI(GOHandle const& handle, float dt, float min=0.0f, float max=25.0f, bool toggle=false);
private:
	float duration;
	float currentTime;
	void OnStart() override;
	void Update(float dt) override;
	void Serialize(Serializer &sr, LogicHandle idx) override;
	void Deserialize(Deserializer &d) override;
	void Reflect(const Reflector& r) override;

  GOHandle Fade;
  GOHandle background;
  GOHandle digipenlogo;
  GOHandle teamlogo;
  GOHandle pressAny;
  GOHandle pressCharley;
  GOHandle pressBarley;
  GOHandle pressHarley;
  bool isdplogo=true;
  bool isteamlogo=false;
  bool fadeout = false;
  float fadeTime = 2.0f;

  float rotSpeed = 50.0f;
  bool rotIncreasing = true;
};