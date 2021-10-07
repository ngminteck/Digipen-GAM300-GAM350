#include "pch.h"

Hint::Hint(): ScriptCommonIF(ASC_GETSCRIPTNAME(Hint)), player(nullptr), targetScript(nullptr)
{

}
Hint::~Hint()
{

}

void Hint::OnStart()
{
	if(goHdl.IsValid())
		player = ASC_GETSCRIPT_PTR(PlayerController, goHdl);
	hintText = ASC_FACTORY.GetGameObjectByName("hintText");
	if (!hintText.IsValid())
		ASC_FACTORY.CreateGOfromPB("hintText");

	dialogBG = ASC_FACTORY.GetGameObjectByName("dialogBg");
	showHint = false;
	

}
void Hint::Update(float dt)
{
	HandleKeyboard(dt);
	if (!showHint)
	{
		hintText->GetHdl<UITextComp>()->Enabled = false;
		hintText->GetHdl<UITextComp>()->tint.w = 1.0f;
		hintText->GetHdl<UITextComp>()->offset.x = 0;
		hintText->GetHdl<UITextComp>()->scale = 0.8f;
	}
	else
	{
		hintText->GetHdl<UITextComp>()->Enabled = true;
	}
	//For scent

	if (targetScript != nullptr && showHint)
	{
		//Play particles if any

		PickupInteract* pp = ASC_GETSCRIPT_PTR(PickupInteract, targetScript->GetGOHandle());
		if (pp != nullptr)
		{
			pp->AssignSmells();
			targetScript->UseParticles(3.0f, false);
		}

		if (numLines < targetScript->hint.size() && time <= 0.0)
		{
			hintText->GetHdl<UITextComp>()->SetText(targetScript->hint[numLines]);
			float offsetMult;
			/*if (targetScript->hint[numLines].length() > 20)
			{
				hintText->GetHdl<UITextComp>()->scale = 0.8f;
				offsetMult = 5.0f;
			}
				
			else if (targetScript->hint[numLines].length() > 40)
			{
				hintText->GetHdl<UITextComp>()->scale = 0.6f;
				offsetMult = 20.0f;
			}
			else
			{
				hintText->GetHdl<UITextComp>()->scale = 1.0f;
				offsetMult = 1.f;

			}*/
			//hintText->GetHdl<UITextComp>()->offset.x = -((targetScript->hint[numLines].length() - 1) / offsetMult);
			hintText->GetHdl<UITextComp>()->offset.x = -(targetScript->hint[numLines].length() * 0.047f);
			hintText->GetHdl<UITextComp>()->tint.w = 255.0f;
			size_t f = targetScript->hint[numLines].find("happy");

			if (f != std::string::npos)
				happy = true;
			else if (targetScript->hint[numLines].find("sad") != std::string::npos)
			{
				sad = true;
			}

			dialogBG->GetHdl<GraphicsComp>()->SetVisible(true);
			time = 3.0;
			numLines++;
		}
		else
		{
			//Timer
			if (time > 0.0)
			{
				time -= (dt);
				hintText->GetHdl<UITextComp>()->tint.w = ((time / 3.0f));
				if(numLines == targetScript->hint.size())
					dialogBG->GetHdl<GraphicsComp>()->tint.w = ((time / 3.0f));

				if (happy && time < 1.5f)
				{
					happy = false;
					ASC_PLAY("SFX_Pant.ogg");
				}
				else if (sad && time < 1.5f)
				{
					sad = false;
					ASC_PLAY("SFX_Whine.ogg");
				}
					
			}
			if(numLines == targetScript->hint.size() && time <= 0.0f)
			{
				showHint = false;
				numLines = 0;
				time = 0.0;
				hintText->GetHdl<UITextComp>()->SetText("");
				dialogBG->GetHdl<GraphicsComp>()->SetVisible(false);
			}
			
		}
	}
	


	
}
void Hint::OnDestroy()
{

}
void Hint::Serialize(Serializer& sr, LogicHandle idx)
{
}
void Hint::Deserialize(Deserializer& d)
{
}
void Hint::Reflect(const Reflector& r)
{
}



void Hint::HandleMessage(MsgData_Base* m)
{
}

void Hint::HandleKeyboard( float)
{
	if (ASC_KEYTRIGGERED(ASC_BUTTON_RIGHTMOUSE) || ASC_CONTROLLER1.GetButtonTrigger(GAMEPAD_INPUT::Y))
	{

		if (player->lastInteract.IsValid() && player->lastInteract->GetHdl<GraphicsComp>()->isHighlighted)
		{

			targetScript = ASC_GETSCRIPT_INTERACT(player->lastInteract);
			numLines = 0;
		
			showHint = true;
			time = 0.0;
			ASC_PLAY("SFX_Sniff.ogg");

			Waypointer* w = ASC_GETSCRIPT_PTR(Waypointer, targetScript->GetGOHandle());
			if (w && w->scent)
			{
				Scent* s = ASC_GETSCRIPT_PTR(Scent, goHdl);
				if (s != nullptr)
				{

				//if we did not have any scent or changing scent
					if (s->w == nullptr || s->w != w)
					{
						s->ResetTrail();
						s->w = w;
					}
				}
			}
		}

		//hintText->GetHdl<UITextComp>()->SetText()
	}

}
void Hint::HandleController( float)
{
}

