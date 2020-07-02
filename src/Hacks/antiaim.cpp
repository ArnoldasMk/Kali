#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../Hacks/legitbot.h"
#include "../Hacks/autowall.h"
#include "../Hacks/autowall.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

#ifndef LessThan
    #define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
    #define IsEqual(x, y) (x == x)
#endif

QAngle AntiAim::LastTickViewAngle;
static bool needToFlick = false;


float AntiAim::GetMaxDelta( CCSGOAnimState *animState) 
{
    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}

static C_BasePlayer* GetClosestEnemy (CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BasePlayer* closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	for (int i = engine->GetMaxClients(); i > 1; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
	    	|| player == localplayer
	    	|| player->GetDormant()
	    	|| !player->GetAlive()
	    	|| player->GetImmune())
	    	continue;

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
	   	 	continue;

		Vector cbVecTarget = player->GetAbsOrigin();
		
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		
		if (prevFOV == 0.f)
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
		else if ( cbFov < prevFOV )
		{
			return player;
		}
		else 
			break;
	}
	return closestPlayer;
}

static float GetBestHeadAngle(QAngle &angle, CUserCmd* cmd)
{    
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	
    if (!localplayer)
		return angle.y;
    
    if (!localplayer->GetAlive())
        return angle.y;
    
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return angle.y;
    if (activeWeapon->GetInReload())
        return angle.y;

    C_BasePlayer* target = GetClosestEnemy(cmd);

    if (!target)
        return angle.y;
    if (!target->GetAlive())
        return angle.y;
	
	// we will require these eye positions
	const Vector &eye_position = localplayer->GetEyePosition();
	// lets set up some basic values we need
	int best_damage = 100.f;
	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
	const float angle_step = 45.f;
	// our result
	float yaw = 0.0f;
    Vector HeadPosition = ( cos( CreateMove::lastTickViewAngles.y ) + eye_position.x,
							sin( CreateMove::lastTickViewAngles.y ) + eye_position.y,
							eye_position.z);

    AutoWall::FireBulletData data;
	data.src = target->GetEyePosition();
	data.filter.pSkip = target;

    QAngle angles = Math::CalcAngle(data.src, HeadPosition);
	Math::AngleVectors(angles, data.direction);
    data.direction = data.direction.Normalize();

    C_BaseCombatWeapon* targetWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(target->GetActiveWeapon());
	
    if (AutoWall::SimulateFireBullet(targetWeapon, true, data) && data.current_damage >= localplayer->GetHealth())
        return CreateMove::lastTickViewAngles.y + 180.f;
        

    // iterate through 45.0f deg angles  
	for( float n = 0.0f; n < 360.f; n += angle_step )
	{
		// shoutout to aimtux for headpos calc
		Vector head_position( cos( n ) + eye_position.x,
							sin( n ) + eye_position.y,
							eye_position.z);

        QAngle angles = Math::CalcAngle(data.src, head_position);
	    Math::AngleVectors(angles, data.direction);
        data.direction = data.direction.Normalize();
        data.current_damage = 0.f;
		if( AutoWall::SimulateFireBullet(targetWeapon, true, data) && data.current_damage <= best_damage )
		{
			best_damage = data.current_damage;
			yaw = n;
		}
	}
   
	return yaw;
}

static void LBYBreak(float offset, QAngle& angle,C_BasePlayer* localplayer)
{
    static bool lbyBreak = false;
    static float lastCheck = 0.f;
    float vel2D = localplayer->GetVelocity().Length2D();
    if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        } 
        else {
            if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                AntiAim::realAngle.y = angle.y -= offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                AntiAim::realAngle.y = angle.y -= offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
}

// Funtion for Rage Anti AIm
static void FakeArroundRealAntiAim(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    
    if (Settings::AntiAim::HeadEdge::enabled)    {
        const float &BestHeadAngle =  GetBestHeadAngle(angle, cmd);
        if (BestHeadAngle){
            if (AntiAim::bSend){
                static bool invert = false;
                invert = !invert;
                AntiAim::realAngle.y = angle.y = invert ? BestHeadAngle - maxDelta : BestHeadAngle + maxDelta;
            }else { // Not Sending Actual Body
                AntiAim::fakeAngle.y = angle.y = BestHeadAngle;
            }
            
            return;
        }
    }
    
    bool buttonToggle = false;

    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);

    static bool bFlip = false;
    if (CreateMove::sendPacket)
        bFlip = !bFlip;
        
    if( AntiAim::bSend )
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE :
                break;
            case AntiAimFakeType_y::Static:
                *localplayer->GetLowerBodyYawTarget() = angle.y;
                angle.y = inverted ? AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Jitter:
                angle.y = bFlip ?  AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Randome:
                if (inverted)
                    angle.y = AntiAim::realAngle.y - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/maxDelta);
                else
                    angle.y = AntiAim::realAngle.y + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/maxDelta);
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    } 
            
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::NONE :
                break;
            case AntiAimRealType_Y::Static :
                angle.y -= 180.f;
                break;
            case AntiAimRealType_Y::Jitter :
                angle.y = bFlip ? (ViewAngle.y-180.f) + GetPercentVal(180.f, JitterPercent) : (ViewAngle.y-180.f) - GetPercentVal(180.f, JitterPercent);
                break;
            case AntiAimRealType_Y::Randome : 
                float percent  = GetPercentVal(180.f, JitterPercent);
                angle.y = (ViewAngle.y-180.f) + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/percent);
                break;
        }
        LBYBreak(angle.y + 45.f, angle, localplayer);
        AntiAim::realAngle.y = angle.y;
    }

}

static void RealArroundFakeAntiAim(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    
    if (Settings::AntiAim::HeadEdge::enabled)    {
        const float &BestHeadAngle =  GetBestHeadAngle(angle, cmd);
        if (BestHeadAngle){
            if (AntiAim::bSend){
                static bool invert = false;
                invert = !invert;
                AntiAim::fakeAngle.y = angle.y = invert ? BestHeadAngle - maxDelta : BestHeadAngle + maxDelta;
            }else { // Not Sending Actual Body
                AntiAim::realAngle.y = angle.y = BestHeadAngle;
            }
            return;
        }
    }

    static bool buttonToggle = false;
    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    
    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);
    
    static bool bFlip = false;
    if (CreateMove::sendPacket)
        bFlip = !bFlip;

    if( AntiAim::bSend )
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::Static :
                angle.y = ViewAngle.y - 180.f;
                break;
            case AntiAimFakeType_y::Jitter :
                angle.y = bFlip ? ViewAngle.y - 180.f - GetPercentVal(180.f, JitterPercent) : ViewAngle.y - 180.f + GetPercentVal(180.f, JitterPercent);
                break;
            case AntiAimFakeType_y::Randome:
                 static float percent  = GetPercentVal(180.f, JitterPercent);
                angle.y = ViewAngle.y + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/percent);
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    } 
            
    else
    {            
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            static float percent = GetPercentVal(maxDelta, AntiAImPercent);
            case AntiAimRealType_Y::Static:
                angle.y = inverted ? AntiAim::fakeAngle.y - percent : AntiAim::fakeAngle.y + percent;
                break;
            case AntiAimRealType_Y::Jitter:
                angle.y = bFlip ? AntiAim::fakeAngle.y + percent : AntiAim::fakeAngle.y - percent;
                break;
            case AntiAimRealType_Y::Randome:
                 if (inverted)
                    angle.y = ViewAngle.y - 180.f - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/percent);
                else
                    angle.y = ViewAngle.y - 180.f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/percent);
                break;
            default:
                break;
        }
        
        LBYBreak(angle.y + 45.f, angle, localplayer);    
        AntiAim::realAngle.y = angle.y;
    }        
}

// Function to set the pitch angle
static void DoAntiAimX(QAngle& angle)   { AntiAim::realAngle.x = AntiAim::fakeAngle.x = angle.x = 89.f; }



// Function For Legit AntiAim
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool& bSend, CUserCmd* cmd)
{
    if (!localplayer->GetAlive() || !localplayer)
        return;

    using namespace Settings::AntiAim::LegitAntiAim;

    static bool buttonToggle = false;
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    QAngle ViewAngle;
        engine->GetViewAngles(ViewAngle);

    static float lastLBY = 0.f;
    if (inverted){
        if (AntiAim::bSend) {
            lastLBY = *localplayer->GetLowerBodyYawTarget();
            AntiAim::fakeAngle = angle;
        }else   {
            // angle.y += maxDelta;
            *localplayer->GetLowerBodyYawTarget() = lastLBY ;
            AntiAim::realAngle.y = angle.y -= maxDelta;
            LBYBreak(angle.y - maxDelta, angle, localplayer);
        }
    }else {
        if (AntiAim::bSend) {
            lastLBY = *localplayer->GetLowerBodyYawTarget() ;
            AntiAim::fakeAngle = angle;
        }else   {
            // angle.y -= maxDelta;
            lastLBY = *localplayer->GetLowerBodyYawTarget();
            AntiAim::realAngle.y = angle.y += maxDelta;
            LBYBreak(angle.y + maxDelta, angle, localplayer);
        }
    }
    
    
    
}

/*
** Settings Manual Anti AIm 
** Simply What we do is change the value of YAxis for in Settings::AntiAIm::Yaw::real
** Basically THis si work as toggle
*/
static bool DoManuaAntiAim(C_BasePlayer* localplayer, const bool& bSend, QAngle& angle)
{

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    static bool bFlip = false;
    static bool Bpressed = false;
    
    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return false;
    
    // Manual anti aim set to back
    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		alignBack = !alignBack;
		alignRight = false;
        alignLeft = false;
	}	

    //END BACK MANUAL

    //SETTING MANUAL FOR RIGHT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		alignBack = false;
		alignRight = !alignRight;
        alignLeft = false;
	}	

    //END MANUAL RIGHT

    // SETTINGS MANUAL ANTIAIM FOR LEFT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
	{
		alignBack = false;
		alignRight = false;
        alignLeft = !alignLeft;
	}
    bool buttonNotPressed = !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton );	
    
    if (buttonNotPressed && Bpressed)
        Bpressed = false;

    AntiAimFakeType_y Fake_aa_type = Settings::AntiAim::Yaw::typeFake;
    // Applying ManualAntiAIm
    if ( !AntiAim::bSend )
    {
        if ( alignBack )
            angle.y += 180.f;
        else if (alignLeft)
            angle.y += 90.f;
        else if (alignRight)
            angle.y -= 90.f;

        AntiAim::realAngle.y = angle.y;
    }
    else
    {
        switch (Fake_aa_type)
        {
            case AntiAimFakeType_y::Static:
                angle.y = inverted ? AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Jitter:
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    }

    return (alignRight || alignLeft || alignBack);
}

// This Method have all the condition where AntiAim Should not work enjoy :)
static bool canMove(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeweapon, CUserCmd* cmd)
{
    if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enable && !Settings::AntiAim::ManualAntiAim::Enable)
        return false;
    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return false;
    if (!localplayer || !localplayer->GetAlive() || !activeweapon)
        return false;
    if (activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeweapon;
        if (csGrenade->GetThrowTime() > 0.f)
            return false;
    }

    if (*activeweapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
    {
        const float& postponTime = activeweapon->GetPostPoneReadyTime();
        if (cmd->buttons & IN_ATTACK2 || postponTime < globalVars->curtime && postponTime > 0.f)
            return false;
    }

    if ( (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK) )
    {
        AntiAim::bSend ? *localplayer->GetLowerBodyYawTarget() = cmd->viewangles.y - AntiAim::GetMaxDelta(localplayer->GetAnimState()) : *localplayer->GetLowerBodyYawTarget() = cmd->viewangles.y + AntiAim::GetMaxDelta(localplayer->GetAnimState());
        return false;
    }

    if ( cmd->buttons & IN_ATTACK2 && activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE )
        return false;
    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return false;

    return true;
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

    if (Settings::FakeLag::enabled)
        AntiAim::bSend = CreateMove::sendPacket;
    else if (globalVars->tickcount % 3)
        AntiAim::bSend = false;
    else 
        AntiAim::bSend = true;

    if ( !canMove(localplayer, activeWeapon, cmd) )
    {
        AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }
        

    QAngle angle = cmd->viewangles;
    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    needToFlick = false;

    

    if( Settings::AntiAim::LBYBreaker::enabled ){
        LBYBreak(Settings::AntiAim::LBYBreaker::offset, angle, localplayer);
    }
    if (Settings::AntiAim::RageAntiAim::enable && !needToFlick) // responsible for reage anti aim or varity of anti aims .. 
    {
        // Check for manual antiaim
        if ( !DoManuaAntiAim(localplayer, AntiAim::bSend, angle)  )
        {
            //Look toword the target that aimbot locked on
            if (Settings::AntiAim::RageAntiAim::atTheTarget)
            {
                C_BasePlayer* lockedTarget = GetClosestEnemy(cmd);
                if (lockedTarget)
                    angle = Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition());
            }
    
            switch (Settings::AntiAim::RageAntiAim::Type)
            {
                case RageAntiAimType::RealArroundFake:
                    RealArroundFakeAntiAim(localplayer, angle, cmd);
                    break;
                case RageAntiAimType::FakeArroundReal:
                    FakeArroundRealAntiAim(localplayer, angle,cmd);
                    break;
                default:
                    break;
            }       
        }
            
        DoAntiAimX(angle); // changing the x View Angle

        if (localplayer->GetFlags() & cmd->upmove == 0.f && cmd->sidemove < 3 && cmd->sidemove > -3)
        {
		    static bool switch_ = false;
		    if (switch_)
			    cmd->sidemove = 2;
		    else
			    cmd->sidemove = -2;
		    switch_ = !switch_;
	    }
    }
    else if (Settings::AntiAim::LegitAntiAim::enable && !needToFlick) // Responsible for legit anti aim activated when the legit anti aim is enabled
    {
        DoLegitAntiAim(localplayer, angle, AntiAim::bSend, cmd);
    }    
    
    CreateMove::sendPacket = AntiAim::bSend;
    Math::NormalizeAngles(angle);
   
    CreateMove::lastTickViewAngles = angle;
    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}