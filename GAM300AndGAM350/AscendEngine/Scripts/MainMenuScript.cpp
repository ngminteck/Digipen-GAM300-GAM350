/******************************************************************************/
/*!
\file   MainMenuScript.cpp
\author
\par    email: \@digipen.edu
\par    Digipen login:
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

MainMenuScript::MainMenuScript() :
    ScriptCommonIF(ASC_GETSCRIPTNAME(MainMenuScript))
{
    ASC_SCRIPT_REGISTER_MSG(mTYPE_SYSTEM_QUIT, HandleMessage);
    ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_PAUSE, HandleMessage);
    ASC_SCRIPT_REGISTER_MSG(mTYPE_GAME_QUIT_MENU, HandleMessage);
}

MainMenuScript::~MainMenuScript()
{
}

void MainMenuScript::ShowQuitDialog()
{
}

void MainMenuScript::OnStart()
{
    //Background Music
    ASC_LOOP("BGM_ambience_01.ogg");

    Camera& currcam = ASC_GMAN.GetCamera();
    currcam.SetPosition(Vec3(0, 0, 10));
    currcam.LookAt(Vec3());
    Fade = ASC_FACTORY.GetGameObjectByName("FadeScreen");
    //puppyCollected = 0;
    VolumeSlider = ASC_FACTORY.GetGameObjectByName("VolumeSlider");

    BrightnessSlider = ASC_FACTORY.GetGameObjectByName("BrightnessSlider");

    PawSelector = ASC_FACTORY.GetGameObjectByName("PawSelector");

    float vol = ASC_SOUNDMAN.GetGlobalVolume();
    float percentVol = vol / MAX_RANGE;
    VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x = percentVol * slider_length + slider_x_min;
    float gamma = ASC_GMAN.GetGamma();
    float percentGamma = gamma / MAX_RANGE;
    BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x = percentGamma * slider_length + slider_x_min;

    //Main Menu Buttons
    StartButton = ASC_FACTORY.GetGameObjectByName("StartButton");
    OptionsButton = ASC_FACTORY.GetGameObjectByName("OptionsButton");
    HowToPlayButton = ASC_FACTORY.GetGameObjectByName("HowToPlayButton");
    CreditsButton = ASC_FACTORY.GetGameObjectByName("CreditsButton");
    ExitButton = ASC_FACTORY.GetGameObjectByName("ExitButton");

    ASC_GMAN.GetCamera().SetPosition(Vector3(365.0f, 200.0f, -260.0f));
    ASC_GMAN.GetCamera().RotateForward(-25.0f, 162.0f);

    //push the function needed 

    std::vector<void (MainMenuScript::*)() > temp ;

    //Main
    temp.push_back(&MainMenuScript::Main_Start);
    temp.push_back(&MainMenuScript::Main_Options);
    temp.push_back(&MainMenuScript::Main_HowToPlay);
    temp.push_back(&MainMenuScript::Main_Credits);
    temp.push_back(&MainMenuScript::Main_Exit);
    functionStates.push_back(std::move(temp));
    temp.clear();
    
    //Opitions
    temp.push_back(&MainMenuScript::Blank);
    temp.push_back(&MainMenuScript::Blank);
    functionStates.push_back(std::move(temp));
    temp.clear();

    //HowToPlay
    temp.push_back(&MainMenuScript::HowToPlay_ReturnToMain);
    functionStates.push_back(std::move(temp));
    temp.clear();

    //Credits
    temp.push_back(&MainMenuScript::Credits_ReturnToMain);
    functionStates.push_back(std::move(temp));
    temp.clear();

    //quit
    temp.push_back(&MainMenuScript::Quit_ExitGame);
    temp.push_back(&MainMenuScript::Quit_ReturnToMain);
    functionStates.push_back(std::move(temp));
    temp.clear();
    fadeTime = 1.5f;
}
void MainMenuScript::MouseUpdate( float dt )
{
    if (HoveredState != previousState)
    {
        ASC_PLAY("SFX_Hover.ogg");
        previousState = HoveredState;
    }
        if (CurrentState == STATE::Main)
        {

            ASC_INPUT.DisplayCursor(true);
            ////EDWIN's START BUTTON
            float width = static_cast<float>(ASC_SYSTEM.winWidth);
            float height = static_cast<float>(ASC_SYSTEM.winHeight);

            Vector2 MousePos = ASC_INPUT.GetMousePos();
            MousePos.x = (MousePos.x - width / 2.0f) / width * 2;
            MousePos.y = -(MousePos.y - height / 2.0f) / height * 2;

            Vec3 min, max, pos, offset, size;

            //StartButton
            {
                pos = StartButton->GetHdl<GraphicsComp>()->positionHdl->pos;
                offset = StartButton->GetHdl<GraphicsComp>()->posOffset;
                pos += offset;
                size = ASC_GMAN.m_objLoader.m_objs.find(StartButton->GetHdl<GraphicsComp>()->modelfile)->second[0].size;

                min.x = pos.x - size.x / 1.28f;
                max.x = pos.x + size.x / 5.0f;

                min.y = pos.y - size.y / 4.0f;
                max.y = pos.y + size.y / 3.0f;

                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        //Main_Start();
                        fadeout = true;
                        fadeTime = 2.0f;
                    }
                    else
                    {
                        APPCONSOLE.AddLog("StartButton Hovered");
                        HoveredState = HOVERSTATE::Main;
                        PawSelector->GetHdl<GraphicsComp>()->posOffset.y = offset.y;
                        StartButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
                    }
                }
                else
                {
                    StartButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.0f;
                }
            }
            //OptionsButton
            {
                pos = OptionsButton->GetHdl<GraphicsComp>()->positionHdl->pos;
                offset = OptionsButton->GetHdl<GraphicsComp>()->posOffset;
                pos += offset;
                size = ASC_GMAN.m_objLoader.m_objs.find(OptionsButton->GetHdl<GraphicsComp>()->modelfile)->second[0].size;

                min.x = pos.x - size.x / 1.28f;
                max.x = pos.x + size.x / 5.0f;

                min.y = pos.y - size.y / 4.0f;
                max.y = pos.y + size.y / 3.0f;


                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        Main_Options();

                    }
                    else
                    {
                        APPCONSOLE.AddLog("OptionsButton Hovered");
                        HoveredState = HOVERSTATE::Options;
                        PawSelector->GetHdl<GraphicsComp>()->posOffset.y = OptionsButton->GetHdl<GraphicsComp>()->posOffset.y;
                        OptionsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
                    }
                }
                else
                {
                    OptionsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.00f;
                }
            }

            //HowToPlayButton
            {
                pos = HowToPlayButton->GetHdl<GraphicsComp>()->positionHdl->pos;
                offset = HowToPlayButton->GetHdl<GraphicsComp>()->posOffset;
                pos += offset;
                size = ASC_GMAN.m_objLoader.m_objs.find(HowToPlayButton->GetHdl<GraphicsComp>()->modelfile)->second[0].size;

                min.x = pos.x - size.x / 0.9f;
                max.x = pos.x + size.x / 4.9f;

                min.y = pos.y - size.y / 4.0f;
                max.y = pos.y + size.y / 3.0f;


                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        Main_HowToPlay();

                        return;
                    }
                    else
                    {
                        APPCONSOLE.AddLog("HowToPlayButton Hovered");
                        HoveredState = HOVERSTATE::HowToPlay;
                        PawSelector->GetHdl<GraphicsComp>()->posOffset.y = HowToPlayButton->GetHdl<GraphicsComp>()->posOffset.y;
                        HowToPlayButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
                    }
                }
                else
                {
                    HowToPlayButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.00f;
                }
            }

            //CreditsButton
            {
                pos = CreditsButton->GetHdl<GraphicsComp>()->positionHdl->pos;
                offset = CreditsButton->GetHdl<GraphicsComp>()->posOffset;
                pos += offset;
                size = ASC_GMAN.m_objLoader.m_objs.find(CreditsButton->GetHdl<GraphicsComp>()->modelfile)->second[0].size;

                min.x = pos.x - size.x / 1.28f;
                max.x = pos.x + size.x / 5.0f;

                min.y = pos.y - size.y / 4.0f;
                max.y = pos.y + size.y / 3.0f;


                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        Main_Credits();

                        return;
                    }
                    else
                    {
                        APPCONSOLE.AddLog("CreditsButton Hovered");
                        HoveredState = HOVERSTATE::Credits;
                        //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
                        //PawSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector2_albedo";
                        PawSelector->GetHdl<GraphicsComp>()->posOffset.y = CreditsButton->GetHdl<GraphicsComp>()->posOffset.y;
                        CreditsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
                    }
                }
                else
                {
                    CreditsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.00f;
                }
            }
            //ExitButton
            {
                pos = ExitButton->GetHdl<GraphicsComp>()->positionHdl->pos;
                offset = ExitButton->GetHdl<GraphicsComp>()->posOffset;
                pos += offset;
                size = ASC_GMAN.m_objLoader.m_objs.find(ExitButton->GetHdl<GraphicsComp>()->modelfile)->second[0].size;

                min.x = pos.x - size.x / 1.28f;
                max.x = pos.x + size.x / 5.0f;

                min.y = pos.y - size.y / 4.0f;
                max.y = pos.y + size.y / 3.0f;


                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        Main_Exit();
                    }
                    else
                    {
                        APPCONSOLE.AddLog("ExitButton Hovered");
                        HoveredState = HOVERSTATE::Quit;
                        //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
                        //PawSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector2_albedo";
                        PawSelector->GetHdl<GraphicsComp>()->posOffset.y = ExitButton->GetHdl<GraphicsComp>()->posOffset.y;

                        ExitButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
                    }
                }
                else
                {
                    ExitButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.00f;
                }
            }



            //Animate PawSelector
            if (PawSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
                rotIncreasing = false;
            if (PawSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
                rotIncreasing = true;

            PawSelector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? PawSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : PawSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;

        }
        else if (CurrentState == STATE::Options)
        {
            if (ASC_KEYRELEASED(ASC_KEY_ESCAPE))
            {
                Options_Return();
                return;
            }
            if (ASC_KEYTRIGGERED(ASC_BUTTON_LEFTMOUSE))
            {
                float width = static_cast<float>(ASC_SYSTEM.winWidth);
                float height = static_cast<float>(ASC_SYSTEM.winHeight);

                Vector2 MousePos = ASC_INPUT.GetMousePos();
                MousePos.x = (MousePos.x - width / 2.0f) / width * 2;
                MousePos.y = -(MousePos.y - height / 2.0f) / height * 2;

                Vec2 volume_knob_min = Vec2(VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x - slider_width / 2.0f, VolumeSlider->GetHdl<GraphicsComp>()->posOffset.y - slider_height / 2.0f);
                Vec2 volume_knob_max = Vec2(VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x + slider_width / 2.0f, VolumeSlider->GetHdl<GraphicsComp>()->posOffset.y + slider_height / 2.0f);
                if (MousePos.x > volume_knob_min.x&& MousePos.x < volume_knob_max.x && MousePos.y > volume_knob_min.y&& MousePos.y < volume_knob_max.y)
                {
                    VolumeSliderSelected = true;
                }

                Vec2 brightness_knob_min = Vec2(BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x - slider_width / 2.0f, BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.y - slider_height / 2.0f);
                Vec2 brightness_knob_max = Vec2(BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x + slider_width / 2.0f, BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.y + slider_height / 2.0f);

                if (MousePos.x > brightness_knob_min.x&& MousePos.x < brightness_knob_max.x && MousePos.y > brightness_knob_min.y&& MousePos.y < brightness_knob_max.y)
                {
                    BrightnessSliderSelected = true;
                }
            }
            else if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
            {
                if (VolumeSliderSelected)
                {
                    VolumeSliderSelected = false;
                    ASC_PLAY("SFX_ButtonClick.ogg");
                }

                if (BrightnessSliderSelected)
                {
                    BrightnessSliderSelected = false;
                    ASC_PLAY("SFX_ButtonClick.ogg");
                }

            }
            else if (ASC_KEYDOWN(ASC_BUTTON_LEFTMOUSE))
            {

                if (VolumeSliderSelected)
                {
                    float width = static_cast<float>(ASC_SYSTEM.winWidth);
                    float height = static_cast<float>(ASC_SYSTEM.winHeight);

                    Vector2 MousePos = ASC_INPUT.GetMousePos();
                    MousePos.x = (MousePos.x - width / 2.0f) / width * 2;
                    MousePos.y = -(MousePos.y - height / 2.0f) / height * 2;


                    if (MousePos.x < slider_x_max && MousePos.x > slider_x_min)
                    {
                        VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x = MousePos.x;
                        float currDist = VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x - slider_x_min;
                        float percent = currDist / slider_length;
                        ASC_SOUNDMAN.SetGlobaVolume(percent * 9);
                        std::string log = "Volume: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                    else if (MousePos.x > slider_x_max)
                    {
                        VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x = slider_x_max;
                        float percent = 100.0f;
                        ASC_SOUNDMAN.SetGlobaVolume(9.0f);
                        std::string log = "Volume: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                    else if (MousePos.x < slider_x_min)
                    {
                        VolumeSlider->GetHdl<GraphicsComp>()->posOffset.x = slider_x_min;
                        float percent = 0.0f;
                        ASC_SOUNDMAN.SetGlobaVolume(0.0f);
                        std::string log = "Volume: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                }
                else if (BrightnessSliderSelected)
                {
                    float width = static_cast<float>(ASC_SYSTEM.winWidth);
                    float height = static_cast<float>(ASC_SYSTEM.winHeight);

                    Vector2 MousePos = ASC_INPUT.GetMousePos();
                    MousePos.x = (MousePos.x - width / 2.0f) / width * 2;
                    MousePos.y = -(MousePos.y - height / 2.0f) / height * 2;

                    if (MousePos.x < slider_x_max && MousePos.x > slider_x_min)
                    {
                        BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x = MousePos.x;
                        float currDist = BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x - slider_x_min;
                        float percent = currDist / slider_length;
                        ASC_GMAN.SetGamma(1.0f + (percent * 8));
                        //ASC_GMAN.SetGamma(percent * 9);
                        std::string log = "Brightness: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                    else if (MousePos.x > slider_x_max)
                    {
                        BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x = slider_x_max;
                        float percent = 100.0f;
                        ASC_GMAN.SetGamma(9.0f);
                        std::string log = "Brightness: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                    else if (MousePos.x < slider_x_min)
                    {
                        BrightnessSlider->GetHdl<GraphicsComp>()->posOffset.x = slider_x_min;
                        float percent = 0.0f;
                        ASC_GMAN.SetGamma(1.0f);
                        //ASC_GMAN.SetGamma(0.0f);
                        std::string log = "Brightness: " + std::to_string(percent) + "%";
                        APPCONSOLE.AddLog(log.c_str());
                    }
                }
            }

        }
        else if (CurrentState == STATE::HowToPlay)
        {
            if (ASC_KEYRELEASED(ASC_KEY_ESCAPE))
            {
                HowToPlay_ReturnToMain();
                return;
            }
        }
        else if (CurrentState == STATE::Credits)
        {
            if (ASC_KEYRELEASED(ASC_KEY_ESCAPE))
            {
                Credits_ReturnToMain();
                return;
            }
        }
        else if (CurrentState == STATE::Quit)
        {
            Vector2 MousePos = ASC_INPUT.GetMousePos();
            float width = static_cast<float>(ASC_SYSTEM.winWidth);
            float height = static_cast<float>(ASC_SYSTEM.winHeight);
            MousePos.x = (MousePos.x - width / 2) / width * 2;
            MousePos.y = -(MousePos.y - height / 2) / height * 2;

            GOHandle& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
            GOHandle& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
            GOHandle& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
            GOHandle& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");

            // Display(Unhide) the Selector
            ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(225.0f);

            //YES 
            {
                auto& model_container = ASC_GMAN.m_objLoader.m_models.find(YesButton->GetHdl<GraphicsComp>()->modelfile)->second;
                Vec3 size = model_container[0].maxPos - model_container[0].minPos;
                Vec3 pos = YesButton->GetHdl<GraphicsComp>()->positionHdl->pos + YesButton->GetHdl<GraphicsComp>()->posOffset;
                Vec3 min;
                min.x = pos.x - size.x / 4.2f;
                min.y = pos.y - size.y / 4.0f;

                Vec3 max;
                max.x = pos.x + size.x / 6.5f;
                max.y = pos.y + size.y / 3.0f;
                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        APPCONSOLE.AddLog("YesButton Clicked");
                        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));
                        return;
                    }
                    else
                    {
                        APPCONSOLE.AddLog("YesButton Hovered");
                        HoveredState = HOVERSTATE::Yes;
                        //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
                        YesButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
                        ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = -0.235f;
                        //ConfirmationSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector1_albedo";
                    }
                }
                else
                {
                    //ConfirmationSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector_albedo";
                    YesButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
                }
            }

            //No 
            {
                auto& model_container = ASC_GMAN.m_objLoader.m_models.find(NoButton->GetHdl<GraphicsComp>()->modelfile)->second;
                Vec3 size = model_container[0].size;//maxPos - model_container[0].minPos;
                Vec3 pos = NoButton->GetHdl<GraphicsComp>()->positionHdl->pos + NoButton->GetHdl<GraphicsComp>()->posOffset;
                Vec3 min;
                min.x = pos.x - size.x / 4.7f;
                min.y = pos.y - size.y / 4.0f;
                Vec3 max;
                max.x = pos.x + size.x / 7.5f;
                max.y = pos.y + size.y / 3.0f;
                if (MousePos.x > min.x&& MousePos.x < max.x && MousePos.y > min.y&& MousePos.y < max.y)
                {
                    if (ASC_KEYRELEASED(ASC_BUTTON_LEFTMOUSE))
                    {
                        APPCONSOLE.AddLog("NoButton Clicked");
                        //Hide Dialog
                        ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                        YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                        NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                        ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
                        CurrentState = STATE::Main;

                        ASC_PLAY("SFX_ButtonClick.ogg");

                        return;
                    }
                    else
                    {
                        APPCONSOLE.AddLog("NoButton Hovered");
                        HoveredState = HOVERSTATE::No;
                        //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
                        NoButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
                        ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.08f;
                        //ConfirmationSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector1_albedo";
                    }
                }
                else
                {
                    //ConfirmationSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector_albedo";
                    NoButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
                }
            }

            //Animate PawSelector
            if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
                rotIncreasing = false;
            if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
                rotIncreasing = true;

            ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;

        }
    }

void MainMenuScript::PadUpdate(float dt)
{
    int amt = static_cast<int>(functionStates[static_cast<int>(CurrentState)].size());
    //up
    if (Up())
    {
        index = (index <= 0) ? amt - 1 : index - 1;
        UpdateSelection(dt);
    }
    //down
    if (Down())
    {
        index = (index >= amt - 1) ? 0 : index + 1;
        UpdateSelection(dt);
    }

    switch (CurrentState)
    {
    case MainMenuScript::STATE::Options:
    {
        Options_UpdateSelection(dt);
        if (Accept())
        {
            Options_Return();
        }
        break;
    }
    case MainMenuScript::STATE::Quit:
        if (Left())
        {
            index = (index <= 0) ? amt - 1 : index - 1;
            UpdateSelection(dt);
        }
        //down
        if (Right())
        {
            index = (index >= amt - 1) ? 0 : index + 1;
            UpdateSelection(dt);
        }
        //also run default if you are Quit state
    default:
    {
        if (Accept())
        {
            if (index == 0)
            {
                if (CurrentState == STATE::HowToPlay)
                {
                    HowToPlay_ReturnToMain();
                }
                else if (CurrentState == STATE::Credits)
                {
                    Credits_ReturnToMain();
                }
                else
                {
                    fadeout = true;
                    fadeTime = 2.0f;
                }

                //fadeout = true;
                //fadeTime = 2.0f;
            }
            else
            {

                std::invoke(functionStates[static_cast<int>(CurrentState)][index], this);
                UpdateSelection(dt);
            }
            //index = 0;
            //std::cout << "hi\n";
        }

        break;
    }
    }
}
void MainMenuScript::KeyUpdate(float dt)
{
    int amt = static_cast<int>(functionStates[static_cast<int>(CurrentState)].size());
    //up
    if (UpMouseKey())
    {
        index = (index <= 0) ? amt - 1 : index - 1;
        UpdateSelection(dt);
    }
    //down
    if (DownMouseKey())
    {
        index = (index >= amt - 1) ? 0 : index + 1;
        UpdateSelection(dt);
    }

    switch (CurrentState)
    {
    case MainMenuScript::STATE::Options:
    {
        Options_UpdateSelection(dt);
        if (AcceptMouseKey())
        {
            Options_Return();
        }
        break;
    }
    case MainMenuScript::STATE::Quit:
        if (LeftMouseKey())
        {
            index = (index <= 0) ? amt - 1 : index - 1;
            UpdateSelection(dt);
        }
        //down
        if (RightMouseKey())
        {
            index = (index >= amt - 1) ? 0 : index + 1;
            UpdateSelection(dt);
        }
        //also run default if you are Quit state
    default:
    {
        if (AcceptMouseKey())
        {
            if (index == 0)
            {

                if (CurrentState == STATE::HowToPlay)
                {
                    HowToPlay_ReturnToMain();
                }
                else if (CurrentState == STATE::Credits)
                {
                    Credits_ReturnToMain();
                }
                else
                {
                    fadeout = true;
                    fadeTime = 2.0f;
                }

            }
            else
            {

                std::invoke(functionStates[static_cast<int>(CurrentState)][index], this);
                UpdateSelection(dt);
            }
            //index = 0;
            //std::cout << "hi\n";
        }

        break;
    }
    }
}

void MainMenuScript::CheckInput(float dt )
{
  //Animate PawSelector
  if (PawSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
    rotIncreasing = false;
  if (PawSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
    rotIncreasing = true;

  PawSelector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? PawSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : PawSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;

  
  
 // if(ASC_KEYTRIGGERED)
}

void MainMenuScript::Update(float dt)
{
    if (fadein)
    {
        if (fadeTime >= 0.0f)
            Fade->GetHdl<GraphicsComp>()->SetTransparencyVal(fadeTime/2);
        else
        {
            fadein = false;
            Fade->GetHdl<GraphicsComp>()->SetVisible(false);
        }
        fadeTime -= dt;
    }
    else
    {
        CheckInput(dt);
        /* MouseUpdate(dt);
         return;*/
        if (fadeout)
        {
            if (fadeTime >= 0.0f)
                Fade->GetHdl<GraphicsComp>()->SetTransparencyVal(1 - (fadeTime / 2));
            else
            {
                fadeout = false;
                Fade->GetHdl<GraphicsComp>()->SetVisible(true);
                if(ASC_ENGINE.GetUsingMouse())
                    Main_Start();
                else
                {
                    std::invoke(functionStates[static_cast<int>(CurrentState)][index], this);
                    UpdateSelection(dt);
                }
            }
            fadeTime -= dt;
        }
        else
        {
            if (ASC_ENGINE.GetUsingMouse())
            {
                MouseUpdate(dt);
                KeyUpdate(dt);
            }
            else
            {
                PadUpdate(dt);
            }
        }
    }
}
void MainMenuScript::UpdateSelection(float dt)
{
  switch (CurrentState)
  {
  case MainMenuScript::STATE::Main:
    Main_UpdateSelection(dt);
    break;
  case MainMenuScript::STATE::Options:
    Options_UpdateSelection(dt);
    break;
  case MainMenuScript::STATE::Quit:
    Exit_UpdateSelection(dt);
    break;
  default:
    break;
  }
  ASC_PLAY("SFX_Hover.ogg");
}
void MainMenuScript::Main_UpdateSelection(float)
{
  //first make all not highlight
  {
    StartButton    ->GetHdl<GraphicsComp>()->uvOffset.x =0.00f;
    OptionsButton  ->GetHdl<GraphicsComp>()->uvOffset.x =0.00f;
    HowToPlayButton->GetHdl<GraphicsComp>()->uvOffset.x =0.00f;
    CreditsButton  ->GetHdl<GraphicsComp>()->uvOffset.x =0.00f;
    ExitButton     ->GetHdl<GraphicsComp>()->uvOffset.x =0.00f;

  }
  switch (index)
  {
    case 0://STart
    {
      auto offset = StartButton->GetHdl<GraphicsComp>()->posOffset;
      //APPCONSOLE.AddLog("StartButton Hovered");
      PawSelector->GetHdl<GraphicsComp>()->posOffset.y = offset.y;
      StartButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
      break;
    }
    case 1://Opition
    {
      APPCONSOLE.AddLog("OptionsButton Hovered");
      PawSelector->GetHdl<GraphicsComp>()->posOffset.y = OptionsButton->GetHdl<GraphicsComp>()->posOffset.y;
      OptionsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
      break;
    }
    case 2://HowToPlay
    {
      APPCONSOLE.AddLog("HowToPlayButton Hovered");
      PawSelector->GetHdl<GraphicsComp>()->posOffset.y = HowToPlayButton->GetHdl<GraphicsComp>()->posOffset.y;
      HowToPlayButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
      break;
    }
    case 3://Credits
    {
      APPCONSOLE.AddLog("CreditsButton Hovered");
      //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
      //PawSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector2_albedo";
      PawSelector->GetHdl<GraphicsComp>()->posOffset.y = CreditsButton->GetHdl<GraphicsComp>()->posOffset.y;
      CreditsButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
      break;//Quit
    }
    case 4: //quit
    {
      APPCONSOLE.AddLog("ExitButton Hovered");
      //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
      //PawSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector2_albedo";
      PawSelector->GetHdl<GraphicsComp>()->posOffset.y = ExitButton->GetHdl<GraphicsComp>()->posOffset.y;

      ExitButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.33f;
      break;
    }
  }
 
}
void MainMenuScript::Options_UpdateSelection(float)
{
  switch (index)
  {
    case 0:
    {
      auto percent =  UpdateSlider(VolumeSlider);
      ASC_SOUNDMAN.SetGlobaVolume(percent * 9);
      std::string log = "Volume: " + std::to_string(percent) + "%";
      APPCONSOLE.AddLog(log.c_str());

      break;
    }
    case 1:
    {
      auto percent = UpdateSlider(BrightnessSlider);
      ASC_GMAN.SetGamma(1.0f + (percent * 8));
      //ASC_GMAN.SetGamma(percent * 9);
      std::string log = "Brightness: " + std::to_string(percent) + "%";
      APPCONSOLE.AddLog(log.c_str());
      break;
    }
    default:
      break;
  }
}
void MainMenuScript::Exit_UpdateSelection(float dt)
{
  GOHandle& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  GOHandle& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  GOHandle& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  GOHandle& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");
  // Display(Unhide) the Selector
  ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(225.0f);
  if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation > maxRot)
    rotIncreasing = false;
  if (ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation < minRot)
    rotIncreasing = true;
  ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation = rotIncreasing ? ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation + rotSpeed * dt : ConfirmationSelector->GetHdl<GraphicsComp>()->zRotation - rotSpeed * dt;

  //reset all highlights
  {
    YesButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
    NoButton->GetHdl<GraphicsComp>()->uvOffset.x = -0.11f;
  }
 
  switch (index)
  {
    case 0:
    {
      APPCONSOLE.AddLog("YesButton Hovered");
      //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
      YesButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
      ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = -0.235f;
      //ConfirmationSelector->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = "UI_paw_selector1_albedo";
      break;
    }
    case 1:
    {
      APPCONSOLE.AddLog("NoButton Hovered");
      //ASC_INPUT.SetHoveredCursor("HoveredCursor_albedo.cur");
      NoButton->GetHdl<GraphicsComp>()->uvOffset.x = 0.22f;
      ConfirmationSelector->GetHdl<GraphicsComp>()->posOffset.x = 0.08f;
      break;
    }

  }
 

  return;
}
void MainMenuScript::Main_Start()
{

  APPCONSOLE.AddLog("Loading Level Block Out 3");
  ASC_MSGSYS.SendMsg(Msg_Change_Scene("Lv1_Realest"));
  ASC_PLAY("SFX_ButtonClick.ogg");
  index = 0;
}
void MainMenuScript::Main_Options()
{
  APPCONSOLE.AddLog("OptionsButton Clicked");
  GOHandle& OptionScreen = ASC_FACTORY.GetGameObjectByName("OptionScreen");
  OptionScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255);
  VolumeSlider->GetHdl<GraphicsComp>()->SetTransparencyVal(255);
  BrightnessSlider->GetHdl<GraphicsComp>()->SetTransparencyVal(255);
  index = 0;
  ASC_PLAY("SFX_ButtonClick.ogg");

  CurrentState = STATE::Options;
}
void MainMenuScript::Main_HowToPlay()
{
  APPCONSOLE.AddLog("HowToPlayButton Clicked");
  GOHandle& htpScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayScreen");
  htpScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  CurrentState = STATE::HowToPlay;

  ASC_PLAY("SFX_ButtonClick.ogg");
  index = 0; 
}
void MainMenuScript::Main_Credits()
{
  APPCONSOLE.AddLog("CreditsButton Clicked");
  GOHandle& creditScreen = ASC_FACTORY.GetGameObjectByName("CreditScreen");
  creditScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);
  CurrentState = STATE::Credits;

  ASC_PLAY("SFX_ButtonClick.ogg");
  index = 0; 
}
void MainMenuScript::Main_Exit()
{
  APPCONSOLE.AddLog("ExitButton Clicked");
  //ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));//Display the confirmation first

  GOHandle& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  GOHandle& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  GOHandle& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(255.0f);

  ASC_PLAY("SFX_ButtonClick.ogg");

  CurrentState = STATE::Quit;
  index = 0; 
}

void MainMenuScript::Options_Return()
{
  GOHandle& OptionScreen = ASC_FACTORY.GetGameObjectByName("OptionScreen");
  OptionScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  VolumeSlider->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  BrightnessSlider->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  CurrentState = STATE::Main;

  ASC_PLAY("SFX_ButtonClick.ogg");
  index = 1;
 // ASC_INPUT.ClearBuffer();
}

float MainMenuScript::UpdateSlider(GOHandle slider)
{
  auto pos = slider->GetHdl<GraphicsComp>()->posOffset.x;
  auto val = 0.0f;
  if (Right())
  {
    val += 0.05f;
  }
  if (Left())
  {
    val -= 0.05f;
  }
  if (pos + val > slider_x_max)
  {
    val = 0;
  }
  else if (pos + val < slider_x_min)
  {
    val = 0;
  }
  pos += val;

  slider->GetHdl<GraphicsComp>()->posOffset.x = pos; // MousePos.x;
  float currDist = slider->GetHdl<GraphicsComp>()->posOffset.x - slider_x_min;
  float percent = currDist / slider_length;
  return percent;
}
void MainMenuScript::HowToPlay_ReturnToMain()
{

    GOHandle& HowToPlayScreen = ASC_FACTORY.GetGameObjectByName("HowToPlayScreen");
    HowToPlayScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
    CurrentState = STATE::Main;
   // ASC_INPUT.ClearBuffer();
    index = 2;
    UpdateSelection(0);
    ASC_PLAY("SFX_ButtonClick.ogg");
}
void MainMenuScript::Credits_ReturnToMain()
{
  GOHandle& creditScreen = ASC_FACTORY.GetGameObjectByName("CreditScreen");
  creditScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  CurrentState = STATE::Main;
  //ASC_INPUT.ClearBuffer();
  index = 3;
  UpdateSelection(0);
  ASC_PLAY("SFX_ButtonClick.ogg");

}

void MainMenuScript::Quit_ExitGame()
{
  APPCONSOLE.AddLog("YesButton Clicked");
  ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));
  index = 4;
}
void MainMenuScript::Quit_ReturnToMain()
{
  GOHandle& ConfirmationScreen = ASC_FACTORY.GetGameObjectByName("ConfirmationScreen");
  GOHandle& YesButton = ASC_FACTORY.GetGameObjectByName("YesButton");
  GOHandle& NoButton = ASC_FACTORY.GetGameObjectByName("NoButton");
  GOHandle& ConfirmationSelector = ASC_FACTORY.GetGameObjectByName("ConfirmationSelector");

  APPCONSOLE.AddLog("NoButton Clicked");
  //Hide Dialog
  ConfirmationScreen->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  YesButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  NoButton->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  ConfirmationSelector->GetHdl<GraphicsComp>()->SetTransparencyVal(0.0f);
  CurrentState = STATE::Main;

  ASC_PLAY("SFX_ButtonClick.ogg");
  index = 4;
}
void MainMenuScript::OnDestroy()
{

}

void MainMenuScript::Serialize(Serializer& sr, LogicHandle idx)
{
    sr.Write(GetName());
}

void MainMenuScript::Deserialize(Deserializer& d)
{
}

void MainMenuScript::Reflect(const Reflector& r)
{
}

void MainMenuScript::HandleMessage(MsgData_Base* m)
{
    switch (m->type)
    {

    case  mTYPE_GAME_PAUSE:
    {
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_RESUME));
    } break;

    case  mTYPE_SYSTEM_QUIT:
    {
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_RESUME));
    }
    break;

    case  mTYPE_GAME_QUIT_MENU:
    {
        ShowQuitDialog();
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_RESUME));
    }
    break;

    default: break;
    }
}

bool MainMenuScript::Up()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_UP);
   
}
bool MainMenuScript::Down()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_DOWN);
  
}
bool MainMenuScript::Left()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_LEFT);
}
bool MainMenuScript::Right()
{
  return  ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::DPAD_RIGHT);
}
bool MainMenuScript::Accept()
{
  return ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::B);

}

bool MainMenuScript::UpMouseKey()
{
    std::cout << ASC_CONTROLLER1.LeftStick_Y() << std::endl;
    return ASC_KEYTRIGGERED(ASC_KEY_UP) ||
        ASC_KEYTRIGGERED(ASC_KEY_W);
}
bool MainMenuScript::DownMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_DOWN) ||
        ASC_KEYTRIGGERED(ASC_KEY_S);

}
bool MainMenuScript::LeftMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_LEFT) ||
        ASC_KEYTRIGGERED(ASC_KEY_A);
}
bool MainMenuScript::RightMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_RIGHT) ||
        ASC_KEYTRIGGERED(ASC_KEY_D);
}
bool MainMenuScript::AcceptMouseKey()
{
    return ASC_KEYTRIGGERED(ASC_KEY_SPACE);

}