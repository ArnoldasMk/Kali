#include "thirdperson.h"
#include "antiaim.h"

#include "../settings.h"
#include "../interfaces.h"

static bool buttonToggle = false;

void ThirdPerson::OverrideView(CViewSetup *pSetup)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if(!localplayer)
		return;

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

	if (activeWeapon && activeWeapon->GetCSWpnData() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
	{
		input->m_fCameraInThirdPerson = false;
		return;
	}

	if(localplayer->GetAlive() && Settings::ThirdPerson::enabled && !engine->IsTakingScreenshot())
	{
		/* Code For Toggle
			if (inputSystem->IsButtonDown(Settings::ThirdPerson::toggleThirdPerson) && buttonToggle == false)
				Settings::ThirdPerson::toggled = true;
			else if ( !inputSystem->IsButtonDown(Settings::ThirdPerson::toggleThirdPerson ) && Settings::ThirdPerson::toggled == true)
				Settings::ThirdPerson::toggled = false;
		*/

		/*
		* Button Toggel Code :)
		*/
		if ( inputSystem->IsButtonDown(Settings::ThirdPerson::toggleThirdPerson) && buttonToggle == false )
		{
			buttonToggle = true;
			Settings::ThirdPerson::toggled = !Settings::ThirdPerson::toggled;
		}
		else if ( !inputSystem->IsButtonDown(Settings::ThirdPerson::toggleThirdPerson ) && buttonToggle == true)
			buttonToggle = false;
		
		// END

		if (Settings::ThirdPerson::toggled)
		{
			QAngle viewAngles;
			engine->GetViewAngles(viewAngles);
			trace_t tr;
			Ray_t traceRay;
			Vector eyePos = localplayer->GetEyePosition();

			Vector camOff = Vector(cos(DEG2RAD(viewAngles.y)) * Settings::ThirdPerson::distance,
							   		sin(DEG2RAD(viewAngles.y)) * Settings::ThirdPerson::distance,
							   		sin(DEG2RAD(-viewAngles.x)) * Settings::ThirdPerson::distance);

			traceRay.Init(eyePos, (eyePos - camOff));
			CTraceFilter traceFilter;
			traceFilter.pSkip = localplayer;
			trace->TraceRay(traceRay, MASK_SOLID, &traceFilter, &tr);

       	 	input->m_fCameraInThirdPerson = true;
			input->m_vecCameraOffset = Vector(viewAngles.x, viewAngles.y, Settings::ThirdPerson::distance * ((tr.fraction < 1.0f) ? tr.fraction : 1.0f) );
		}
		else
		{
			input->m_fCameraInThirdPerson = false;
			input->m_vecCameraOffset = Vector(0.f, 0.f, 0.f);
		}
		
	}
	else if(input->m_fCameraInThirdPerson)
	{
		input->m_fCameraInThirdPerson = false;
		input->m_vecCameraOffset = Vector(0.f, 0.f, 0.f);
	}

	
}


void ThirdPerson::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage == ClientFrameStage_t::FRAME_RENDER_START && engine->IsInGame())
	{
		C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

		if (localplayer && localplayer->GetAlive() && Settings::ThirdPerson::enabled && input->m_fCameraInThirdPerson)
		{
			if (Settings::AntiAim::Yaw::enabled)
				*localplayer->GetVAngles() = AntiAim::realAngle;
		}
	}
}
