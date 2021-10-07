/******************************************************************************/
/*!
\file   SplashScreen.cpp
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
#include "pch.h"

SplashScreen::SplashScreen() 
  : ScriptCommonIF(ASC_GETSCRIPTNAME(SplashScreen)), duration(5.0f), currentTime(0.0f)
{
}

void SplashScreen::OnStart()
{
	currentTime = 0.0f;
    //g->tint.a = 0.0f;

  //auto& bgT = t->FindChild("BG");
  //background = bgT->GetGOHandle();

    Fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");
    digipenlogo = ASC_FACTORY.GetGameObjectByName("DPLOGO");
    teamlogo = ASC_FACTORY.GetGameObjectByName("team_Logo");
    pressAny = ASC_FACTORY.GetGameObjectByName("PressAny");
    pressCharley = ASC_FACTORY.GetGameObjectByName("PressAny_Charley");
    pressBarley = ASC_FACTORY.GetGameObjectByName("PressAny_Barley");
    pressHarley = ASC_FACTORY.GetGameObjectByName("PressAny_Harley");

    ASC_INPUT.DisplayCursor(false);
}
void SplashScreen::RotateUI(GOHandle const& handle, float dt, float min, float max, bool toggle)
{
    if (handle->GetHdl<GraphicsComp>()->zRotation > max)
        rotIncreasing = toggle;
    if (handle->GetHdl<GraphicsComp>()->zRotation < min)
        rotIncreasing = !toggle;

    handle->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? handle->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : handle->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;

}

void SplashScreen::Update(float dt)
{
  /*float& red = g->tint.r;
  float& green = g->tint.g;
  float& blue = g->tint.b;
  float& alpha = g->tint.a;*/
  //Animate PawSelector
    RotateUI(pressCharley,dt);
    RotateUI(pressBarley, dt, -12.5f, 12.5f);
    RotateUI(pressHarley,dt,-25.0f,0.0f);

	if (currentTime > duration)// || ASC_ANYKEYPRESSED())
	{
		//ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
        if (isdplogo)
        {
            if (digipenlogo->GetHdl<GraphicsComp>()->tint.a > 0.0f)
                digipenlogo->GetHdl<GraphicsComp>()->tint.a -= dt;
            else
            {
                digipenlogo->GetHdl<GraphicsComp>()->SetVisible(false);
                isdplogo = false;
                isteamlogo = true;
                ASC_LOOP("BGM_Splash.ogg");
                currentTime = 0.0f;
            }
        }
        else if (isteamlogo)
        {
            if (teamlogo->GetHdl<GraphicsComp>()->tint.a > 0.0f)
                teamlogo->GetHdl<GraphicsComp>()->tint.a -= dt;
            else
            {
                teamlogo->GetHdl<GraphicsComp>()->SetVisible(false);
                
                isteamlogo = false;
                currentTime = 0.0f;
            }
        }
	}
    else
    {
        if (isdplogo)
        {
            if (digipenlogo->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                digipenlogo->GetHdl<GraphicsComp>()->tint.a += dt;

        }
        else if (isteamlogo)
        {
            if (teamlogo->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                teamlogo->GetHdl<GraphicsComp>()->tint.a += dt;
        }
        else
        {
            if (pressAny->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                pressAny->GetHdl<GraphicsComp>()->tint.a += dt;
            if (pressCharley->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                pressCharley->GetHdl<GraphicsComp>()->tint.a += dt;
            if (pressHarley->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                pressHarley->GetHdl<GraphicsComp>()->tint.a += dt;
            if (pressBarley->GetHdl<GraphicsComp>()->tint.a < 1.0f)
                pressBarley->GetHdl<GraphicsComp>()->tint.a += dt;

        }
        currentTime += dt;
    }
    if (!isdplogo && !isteamlogo)
    {
        if (!fadeout)
        {

            ASC_INPUT.DisplayCursor(true);
            if (ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE) || ASC_KEYTRIGGERED(ASC_BUTTON_RIGHTMOUSE) || ASC_INPUT.AnyKeyPressed())
            {
                //using mouse & keyboard for gameplay
                ASC_ENGINE.SetUsingMouse(true);
                fadeTime = 2.0f;
                fadeout = true;
            }
            if (ASC_CONTROLLER1.isAnyButtonDown())
            {
                //so controller is being used , change all ui related to controller image
                ASC_ENGINE.SetUsingMouse(false);
                fadeTime = 2.0f;
                fadeout = true;
            }
        }
        if (fadeTime <= 0.0f && fadeout)
            ASC_MSGSYS.SendMsg(Msg_Change_Scene("MainMenu"));
        else if(fadeout)
        {
            Fade->GetHdl<GraphicsComp>()->SetTransparencyVal(1 - (fadeTime / 2));
        }
        fadeTime -= dt;
    }

  /*red = min(1.0f, red + (dt / duration));
  green = min(1.0f, green + (dt / duration));
  blue = min(1.0f, blue + (dt / duration));
  alpha = min(1.0f, alpha + (dt / duration));*/

  //auto &BGCol = background->GetHdl<GraphicsComp>()->tint;
  //BGCol -= Vec4(1, 1, 1, 0) * (dt / duration);


}

void SplashScreen::Serialize(Serializer & sr, LogicHandle idx)
{
  sr.Write("Duration", duration);
}

void SplashScreen::Deserialize(Deserializer & d)
{
  d.Read("Duration", duration);
}

void SplashScreen::Reflect(const Reflector & r)
{
  r.Reflect("Duration", duration);
  r.Reflect("CurrentTime", currentTime);
}
