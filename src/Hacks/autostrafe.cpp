#include "autostrafe.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/math.h"
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
#define M_RADPI 57.295779513082f
static void LegitStrafe(C_BasePlayer* localplayer, CUserCmd* cmd)
{
	if (localplayer->GetFlags() & FL_ONGROUND)
		return;

	if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
		return;

	if (cmd->mousedx <= 1 && cmd->mousedx >= -1)
		return;

	switch (Settings::AutoStrafe::type)
	{
		case AutostrafeType::AS_FORWARDS:
			cmd->sidemove = cmd->mousedx < 0.f ? -250.f : 250.f;
			break;
		case AutostrafeType::AS_BACKWARDS:
			cmd->sidemove = cmd->mousedx < 0.f ? 250.f : -250.f;
			break;
		case AutostrafeType::AS_LEFTSIDEWAYS:
			cmd->forwardmove = cmd->mousedx < 0.f ? -250.f : 250.f;
			break;
		case AutostrafeType::AS_RIGHTSIDEWAYS:
			cmd->forwardmove = cmd->mousedx < 0.f ? 250.f : -250.f;
			break;
		default:
			break;
	}
}
       void vector_angles(const Vector& forward, Vector& angles)
        {
                Vector view;

                if (!forward[0] && !forward[1])
                {
                        view[0] = 0.0f;
                        view[1] = 0.0f;
                }
                else
                {
                        view[1] = atan2(forward[1], forward[0]) * 180.0f / M_PI;

                        if (view[1] < 0.0f)
                                view[1] += 360.0f;

                        view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
                        view[0] = atan2(forward[2], view[2]) * 180.0f / M_PI;
                }

                angles[0] = -view[0];
                angles[1] = view[1];
                angles[2] = 0.f;
        }

float NormalizeYaw( const float& yaw ) {
	while ( yaw > 180.0f )
	return	yaw - 360.0f;
	while ( yaw < -180.0f )
	return	yaw + 360.0f;
}
static void DirectionalStrafe(C_BasePlayer* localplayer, CUserCmd* cmd){
bool is_bhopping;
bool in_transition;
int calculated_direction;
float true_direction;
float wish_direction;
float step;
int rough_direction;
	// Define rough directions
	enum directions {
		FORWARDS = 0,
		BACKWARDS = 180,
		LEFT = 90,
		RIGHT = -90
	};
 
	// Reset direction when player is not strafing
	is_bhopping = cmd->buttons & IN_JUMP;
	if ( !is_bhopping && localplayer->GetFlags() & FL_ONGROUND ) {
		calculated_direction = directions::FORWARDS;
		in_transition = false;
		return;
	}
 
	// Get true view angles
	QAngle base{ };
	engine->GetViewAngles( base );
 
	// Calculate the rough direction closest to the player's true direction
	auto get_rough_direction = [ & ]( float true_direction ) -> float {
		// Make array with our four rough directions
		std::array< float, 4 > minimum = { directions::FORWARDS, directions::BACKWARDS, directions::LEFT, directions::RIGHT };
		float best_angle, best_delta = 181.f;
 
		// Loop through our rough directions and find which one is closest to our true direction
		for ( size_t i = 0; i < minimum.size( ); ++i ) {
			float rough_direction = base.y + minimum.at( i );
			float delta = fabsf( NormalizeYaw( true_direction - rough_direction ) );
 
			// Only the smallest delta wins out
			if ( delta < best_delta ) {
				best_angle = rough_direction;
				best_delta = delta;
			}
		}
 
		return best_angle;
	};
 
	// Get true direction based on player velocity
	true_direction = localplayer->GetVelocity().y;
 
	// Detect wish direction based on movement keypresses
	if ( cmd->buttons & IN_FORWARD ) {
		wish_direction = base.y + directions::FORWARDS;
	}
	else if ( cmd->buttons & IN_BACK ) {
		wish_direction = base.y + directions::BACKWARDS;
	}
	else if ( cmd->buttons & IN_MOVELEFT ) {
		wish_direction = base.y + directions::LEFT;
	}
	else if ( cmd->buttons & IN_MOVERIGHT ) {
		wish_direction = base.y + directions::RIGHT;
	}
	else {
		// Reset to forward when not pressing a movement key, then fix anti-aim strafing by setting IN_FORWARD
		cmd->buttons |= IN_FORWARD;
		wish_direction = base.y + directions::FORWARDS;
	}
 
	// Calculate the ideal strafe rotation based on player speed (c) navewindre
	float speed_rotation = std::min( RAD2DEG( std::asinh( 30.f / localplayer->GetVelocity().Length2D()) ) * 0.5f, 45.f );
	if ( in_transition ) {
		// Calculate the step by using our ideal strafe rotation
		float ideal_step = speed_rotation + calculated_direction;
		step = fabsf( NormalizeYaw( calculated_direction - ideal_step ) );
 
		// Check when the calculated direction arrives close to the wish direction
		if ( fabsf( NormalizeYaw( wish_direction - calculated_direction ) ) > step ) {
			float add = NormalizeYaw( calculated_direction + step );
			float sub = NormalizeYaw( calculated_direction - step );
 
			// Step in direction that gets us closer to our wish direction
			if ( fabsf( NormalizeYaw( wish_direction - add ) ) >= fabsf( NormalizeYaw( wish_direction - sub ) ) ) {
				calculated_direction -= step;
			}
			else {
				calculated_direction += step;
			}
		}
		else {
			in_transition = false;
		}
	}
	else {
		// Get rough direction and setup calculated direction only when not transitioning
		rough_direction = get_rough_direction( true_direction );
		calculated_direction = rough_direction;
 
		// When we have a difference between our current (rough) direction and our wish direction, then transition
		if ( rough_direction != wish_direction ) {
			in_transition = true;
		}
	}
 
	// Set movement up to be rotated
	cmd->forwardmove = 0.f;
	cmd->sidemove = cmd->command_number % 2 ? 450.f : -450.f;
 
	// Calculate ideal rotation based on our newly calculated direction
	float direction = ( cmd->command_number % 2 ? speed_rotation : -speed_rotation ) + calculated_direction;
 
	// Rotate our direction based on our new, defininite direction
	float rotation = DEG2RAD( base.y - direction );
 
	float cos_rot = cos( rotation );
	float sin_rot = sin( rotation );
 
	float forwardmove = ( cos_rot * cmd->forwardmove ) - ( sin_rot * cmd->sidemove );
	float sidemove = ( sin_rot * cmd->forwardmove ) + ( cos_rot * cmd->sidemove );
 
	// Apply newly rotated movement
	cmd->forwardmove = forwardmove;
	cmd->sidemove = sidemove;

}

static void RageStrafe(C_BasePlayer* localplayer, CUserCmd* cmd)
{

        const auto vel = localplayer->GetVelocity().Length2D();

        if (vel < 1.f)
                return;
        if (localplayer->GetFlags() & FL_ONGROUND)
                return;

        if (cmd->mousedx > 1 || cmd->mousedx < -1)
                cmd->sidemove = cmd->mousedx < 0.f ? -450.f : 450.f;
        else
        {
                cmd->forwardmove = std::clamp(10000.f / vel, -450.0f, 450.0f);
                cmd->sidemove = cmd->command_number % 2 == 0 ? -450.f : 450.f;
        }

}

void AutoStrafe::CreateMove(CUserCmd* cmd)
{
	if (!Settings::AutoStrafe::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	if (!localplayer->GetAlive())
		return;

	if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	switch (Settings::AutoStrafe::type)
	{
		case AutostrafeType::AS_FORWARDS:
			return;
		case AutostrafeType::AS_BACKWARDS:
			return;
		case AutostrafeType::AS_LEFTSIDEWAYS:
			return;
		case AutostrafeType::AS_RIGHTSIDEWAYS:
			LegitStrafe(localplayer, cmd);
			break;
		case AutostrafeType::AS_RAGE:
			RageStrafe(localplayer, cmd);
			break;
		case AutostrafeType::AS_DIRECTIONAL:
			DirectionalStrafe(localplayer, cmd);
			break;
	}
}
