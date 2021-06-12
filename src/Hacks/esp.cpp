#include <iomanip>

#include "../SDK/INetChannel.h"
#include "esp.h"
#include "autowall.h"
#include "../fonts.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../Utils/xorstring.h"
#include "../Hooks/hooks.h"
#include "../SDK/CPlayerResource.h"
#include "resolver.h"
#include "../ATGUI/texture.h"
#include "../ATGUI/atgui.h"
#include "../Resources/tux.h"
#include "antiaim.h"
#include "Tickbase.h"
#include "fakelag.h"
#include "../Utils/patternfinder.h"
#include "global.h"

#include <climits>
#include <deque>
#include <mutex>
#include <ctime>
#define TICK_INTERVAL			(globalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )              ( TICK_INTERVAL *( t ) )

#define M_RADPI 57.295779513082f

/* The engine->WorldToScreenMatrix() function can't be called at all times
 * So this is Updated in the Paint Hook for us */
static float next_break = 0.f;
VMatrix vMatrix;
int spawntime = 0;
int oldtrig = 0;
int currtrig = 0;
Vector2D barsSpacing = Vector2D( 0, 0 );
struct Footstep {
    Footstep( Vector &pos, long expireAt ){
        position = pos;
        expiration = expireAt;
    }
    long expiration;
    Vector position;
};

std::mutex footstepMutex;
std::deque<Footstep> playerFootsteps[64]; // entIndex -> Footstep.

QAngle viewanglesBackup;

const char* ESP::ranks[] = {
		"Unranked",
		"Silver I",
		"Silver II",
		"Silver III",
		"Silver IV",
		"Silver Elite",
		"Silver Elite Master",

		"Gold Nova I",
		"Gold Nova II",
		"Gold Nova III",
		"Gold Nova Master",
		"Master Guardian I",
		"Master Guardian II",

		"Master Guardian Elite",
		"Distinguished Master Guardian",
		"Legendary Eagle",
		"Legendary Eagle Master",
		"Supreme Master First Class",
		"The Global Elite"
};

bool dzShouldDraw(C_BaseEntity* ent, C_BasePlayer* localplayer) // Ghetto way to fix a CTD.
{
	if (!localplayer || !ent || !localplayer->GetAlive())
		return false;
	return !(Settings::ESP::DangerZone::drawDistEnabled &&
			(localplayer->GetVecOrigin().DistTo(ent->GetVecOrigin()) > Settings::ESP::DangerZone::drawDist));
}

static void CheckActiveSounds() {
    static CUtlVector<SndInfo_t> sounds; // this has to be static.
    char buf[PATH_MAX];
    static int lastSoundGuid = 0;  // the Unique sound playback ID's increment. It does not get reset to 0
    sound->GetActiveSounds( sounds );
    for ( int i = 0; i < sounds.Count(); i++ ){
        if( sounds[i].m_nSoundSource <= 0 || sounds[i].m_nSoundSource > 63 ) // environmental sounds or out of bounds.
            continue;
        if( !sounds[i].m_pOrigin ) // no location
            continue;
        if( sounds[i].m_nGuid <= lastSoundGuid ) // same sound we marked last time
            continue;
        if( sounds[i].m_nSoundSource == engine->GetLocalPlayer() )
            continue;

        if( !fileSystem->String( sounds[i].m_filenameHandle, buf, sizeof( buf ) ) )
            continue;
        if ( buf[0] != '~' )
            continue;

        if ( strstr( buf, XORSTR( "player/land" ) ) != nullptr ){
            {
                std::unique_lock<std::mutex> lock( footstepMutex );
                playerFootsteps[sounds[i].m_nSoundSource].emplace_back( *sounds[i].m_pOrigin, ( Util::GetEpochTime( ) + Settings::ESP::Sounds::time ) );
            } // RAII mutex lock
            lastSoundGuid = sounds[i].m_nGuid;
        } else if ( strstr( buf, XORSTR( "footstep" ) ) != nullptr ){
            {
                std::unique_lock<std::mutex> lock( footstepMutex );
                playerFootsteps[sounds[i].m_nSoundSource].emplace_back( *sounds[i].m_pOrigin, ( Util::GetEpochTime( ) + Settings::ESP::Sounds::time ) );
            } // RAII mutex lock
            lastSoundGuid = sounds[i].m_nGuid;
        }
    }
    // GetActiveSounds allocates new memory to our CUtlVector every time for some reason.
    sounds.m_Size = 0; // Setting this to 0 makes it use the same memory each time instead of grabbing more.
}

static void DrawManualAntiaim()
{
static int width, height;
engine->GetScreenSize(width, height);
auto color = Color::FromImColor(Settings::ESP::manualAAColor.Color());
    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (AntiAim::ManualAntiAim::alignLeft) {
           Draw::triangle(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), color);
        } else if (AntiAim::ManualAntiAim::alignBack) {
          Draw::triangle(Vector2D(width / 2, height / 2 + 80), Vector2D(width / 2 - 10, height / 2 + 60), Vector2D(width / 2 + 10, height / 2 + 60), color);
        } else if (AntiAim::ManualAntiAim::alignRight) {
          Draw::triangle(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), color);
        }
    }else if (Settings::AntiAim::LegitAntiAim::enable)
    {
   	if (Settings::AntiAim::LegitAntiAim::inverted) //right
          Draw::triangle(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), color);
	else
	           Draw::triangle(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), color);
   }

}
static float GetArmourHealth(float flDamage, int ArmorValue)
{
	float flCurDamage = flDamage;

	if (flCurDamage == 0.0f || ArmorValue == 0)
		return flCurDamage;

	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	float flNew = flCurDamage * flArmorRatio;
	float flArmor = (flCurDamage - flNew) * flArmorBonus;

	if (flArmor > ArmorValue)
	{
		flArmor = ArmorValue * (1.0f / flArmorBonus);
		flNew = flCurDamage - flArmor;
	}

	return flNew;
}
static void renderRange(){
if (!Settings::ESP::taserrange::enabled)
	return;
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

if (!localplayer)
	return;

C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
if (!activeWeapon)
	return;
	CCSWeaponInfo* weaponInfo = activeWeapon->GetCSWpnData();
if (!input->m_fCameraInThirdPerson)
	return;
	ItemDefinitionIndex itemDefinitionIndex = *activeWeapon->GetItemDefinitionIndex();
if (itemDefinitionIndex != ItemDefinitionIndex::WEAPON_TASER)
	return;

Draw::AddCircle3D(localplayer->GetAbsOrigin(),  weaponInfo->GetRange(), Settings::ESP::taserrange::color.Color(), 32);

}
static bool GetBox( C_BaseEntity* entity, int& x, int& y, int& w, int& h ) {
	// Variables
	Vector vOrigin, min, max;
	Vector flb, brt, blb, frt, frb, brb, blt, flt; // think of these as Front-Left-Bottom/Front-Left-Top... Etc.
	float left, top, right, bottom;

	// Get the locations
//	if (!entity->GetDormant())
	vOrigin = entity->GetVecOrigin();
//	else {
//	for ( auto it = playerFootsteps[entity->GetIndex()].begin(); it != playerFootsteps[entity->GetIndex()].end(); it++ ){
//      if ( !playerFootsteps[entity->GetIndex()].empty() ){
//	vOrigin = it->position;
//	entity->GetVecOrigin() = it->position;
//	}else
//      vOrigin = entity->GetVecOrigin();

//	}
//	}
	min = entity->GetCollideable()->OBBMins() + vOrigin;
	max = entity->GetCollideable()->OBBMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector( min.x, min.y, min.z ),
						Vector( min.x, max.y, min.z ),
						Vector( max.x, max.y, min.z ),
						Vector( max.x, min.y, min.z ),
						Vector( max.x, max.y, max.z ),
						Vector( min.x, max.y, max.z ),
						Vector( min.x, min.y, max.z ),
						Vector( max.x, min.y, max.z ) };

	// Get screen positions
	if ( debugOverlay->ScreenPosition( points[3], flb ) || debugOverlay->ScreenPosition( points[5], brt )
		 || debugOverlay->ScreenPosition( points[0], blb ) || debugOverlay->ScreenPosition( points[4], frt )
		 || debugOverlay->ScreenPosition( points[2], frb ) || debugOverlay->ScreenPosition( points[1], brb )
		 || debugOverlay->ScreenPosition( points[6], blt ) || debugOverlay->ScreenPosition( points[7], flt ) )
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for ( int i = 1; i < 8; i++ ) {
		if ( left > arr[i].x )
			left = arr[i].x;
		if ( bottom < arr[i].y )
			bottom = arr[i].y;
		if ( right < arr[i].x )
			right = arr[i].x;
		if ( top > arr[i].y )
			top = arr[i].y;	}

	// Width / height
	x = ( int ) left;
	y = ( int ) top;
	w = ( int ) ( right - left );
	h = ( int ) ( bottom - top );

	return true;
}


ImColor ESP::GetESPPlayerColor(C_BasePlayer* player, bool visible)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return ImColor(255, 255, 255, 255);

	if (player->GetDormant() && Settings::ESP::showDormant)
		return ImColor(255, 255, 255, 255);

	ImColor playerColor;

	if (player == localplayer)
	{
		playerColor = Settings::ESP::localplayerColor.Color(player);
	}
	else
	{
		if (Settings::ESP::teamColorType == TeamColorType::RELATIVE)
		{
			if (!Entity::IsTeamMate(player, localplayer))
			{
				if (visible)
					playerColor = Settings::ESP::enemyVisibleColor.Color(player);
				else
					playerColor = Settings::ESP::enemyColor.Color(player);
			}
			else
			{
				if (visible)
					playerColor = Settings::ESP::allyVisibleColor.Color(player);
				else
					playerColor = Settings::ESP::allyColor.Color(player);
			}
		}
		else if (Settings::ESP::teamColorType == TeamColorType::ABSOLUTE)
		{
			if (player->GetTeam() == TeamID::TEAM_TERRORIST)
			{
				if (visible)
					playerColor = Settings::ESP::tVisibleColor.Color(player);
				else
					playerColor = Settings::ESP::tColor.Color(player);
			}
			else if (player->GetTeam() == TeamID::TEAM_COUNTER_TERRORIST)
			{
				if (visible)
					playerColor = Settings::ESP::ctVisibleColor.Color(player);
				else
					playerColor = Settings::ESP::ctColor.Color(player);
			}
		}
	}

	if (player->GetImmune())
	{
		playerColor.Value.x *= 0.45f;
		playerColor.Value.y *= 0.45f;
		playerColor.Value.z *= 0.45f;
	}

	return playerColor;
}

bool ESP::WorldToScreen( const Vector &origin, ImVec2 * const screen ) {
	float w = vMatrix[3][0] * origin.x
			  + vMatrix[3][1] * origin.y
			  + vMatrix[3][2] * origin.z
			  + vMatrix[3][3];

	if ( w < 0.01f ) // Is Not in front of our player
		return false;

	float width = (float)Paint::engineWidth;
	float height = (float)Paint::engineHeight;

	float halfWidth = width / 2;
	float halfHeight = height / 2;

	float inverseW = 1 / w;

	screen->x = halfWidth +
				(0.5f * ((vMatrix[0][0] * origin.x +
						  vMatrix[0][1] * origin.y +
						  vMatrix[0][2] * origin.z +
						  vMatrix[0][3]) * inverseW) * width + 0.5f);

	screen->y = halfHeight -
				(0.5f * ((vMatrix[1][0] * origin.x +
						  vMatrix[1][1] * origin.y +
						  vMatrix[1][2] * origin.z +
						  vMatrix[1][3]) * inverseW) * height + 0.5f);
	return true;
}
void DrawDormant (C_BaseEntity* entity, ImColor color){
      //  int x, y, w, h;
       // if ( !GetBox( entity, x, y, w, h ) || !(entity->GetDormant()))
        //        return;

      //  DrawBox( color, x, y, w, h, entity , Settings::ESP::Boxes::type);
       // Vector2D nameSize = Draw::GetTextSize( string, esp_font );
       // Draw::AddText(( int ) ( x + ( w / 2 ) - ( nameSize.x / 2 ) ), y + h + 2, string, color );

}


static void DrawBox( ImColor color, int x, int y, int w, int h, C_BaseEntity* entity, BoxType& boxtype ) {
 
	if ( boxtype == BoxType::FRAME_2D ) 
	{
		Draw::AddLine(x,y,x+w, y, color);
		Draw::AddLine(x+w,y,x+w, y+h, color);
		Draw::AddLine(x+w,y+h,x, y+h, color);
		Draw::AddLine(x,y+h,x, y, color);	
	} 
	else if ( boxtype == BoxType::FLAT_2D ) 
	{
		int VertLine = ( int ) ( w * 0.33f );
		int HorzLine = ( int ) ( h * 0.33f );
		int squareLine = std::min( VertLine, HorzLine );

		// top-left corner / color
		Draw::AddRect( x, y, x + squareLine, y + 1, color );
		Draw::AddRect( x, y, x + 1, y + squareLine, color );

		// top-left corner / missing edges
		Draw::AddRect( x + squareLine, y - 1, x + squareLine + 1, y + 2, ImColor( 10, 10, 10, 190 ) );
		Draw::AddRect( x - 1, y + squareLine, x + 2, y + squareLine + 1, ImColor( 10, 10, 10, 190 ) );


		// top-right corner / color
		Draw::AddRect( x + w - squareLine, y, x + w, y + 1, color );
		Draw::AddRect( x + w - 1, y, x + w, y + squareLine, color );

		// top-right corner / missing edges
		Draw::AddRect( x + w - squareLine - 1, y - 1, x + w - squareLine, y + 2, ImColor( 10, 10, 10, 190 ) );
		Draw::AddRect( x + w - 2, y + squareLine, x + w + 1, y + squareLine + 1, ImColor( 10, 10, 10, 190 ) );


		// bottom-left corner / color
		Draw::AddRect( x, y + h - 1, x + squareLine, y + h, color );
		Draw::AddRect( x, y + h - squareLine, x + 1, y + h, color );

		// bottom-left corner / missing edges
		Draw::AddRect( x + squareLine, y + h - 2, x + squareLine + 1, y + h + 1, ImColor( 10, 10, 10, 190 ) );
		Draw::AddRect( x - 1, y + h - squareLine - 1, x + 2, y + h - squareLine, ImColor( 10, 10, 10, 190 ) );


		// bottom-right corner / color
		Draw::AddRect( x + w - squareLine, y + h - 1, x + w, y + h, color );
		Draw::AddRect( x + w - 1, y + h - squareLine, x + w, y + h, color );

		// bottom-right corner / missing edges
		Draw::AddRect( x + w - squareLine, y + h - 2, x + w - squareLine + 1, y + h + 1, ImColor( 10, 10, 10, 190 ) );
		Draw::AddRect( x + w - 2, y + h - squareLine - 1, x + w + 1, y + h - squareLine, ImColor( 10, 10, 10, 190 ) );

		Draw::AddRectFilled( x, y, x + w, y + h, ImColor( color.Value.x, color.Value.y, color.Value.z, 21 * (1.0f/255.0f) ) );
	} 
	else if ( boxtype == BoxType::BOX_3D ) 
	{
		Vector vOrigin = entity->GetVecOrigin();
		Vector min = entity->GetCollideable()->OBBMins() + vOrigin;
		Vector max = entity->GetCollideable()->OBBMaxs() + vOrigin;

		Vector points[] = { Vector( min.x, min.y, min.z ),
							Vector( min.x, max.y, min.z ),
							Vector( max.x, max.y, min.z ),
							Vector( max.x, min.y, min.z ),
							Vector( min.x, min.y, max.z ),
							Vector( min.x, max.y, max.z ),
							Vector( max.x, max.y, max.z ),
							Vector( max.x, min.y, max.z ) };

		int edges[12][2] = {
				{ 0, 1 },
				{ 1, 2 },
				{ 2, 3 },
				{ 3, 0 },
				{ 4, 5 },
				{ 5, 6 },
				{ 6, 7 },
				{ 7, 4 },
				{ 0, 4 },
				{ 1, 5 },
				{ 2, 6 },
				{ 3, 7 },
		};

		for ( const auto edge : edges ) {
			Vector p1, p2;
			if ( debugOverlay->ScreenPosition( points[edge[0]], p1 ) || debugOverlay->ScreenPosition( points[edge[1]], p2 ) )
				return;
			Draw::AddLine( p1.x, p1.y, p2.x, p2.y, color );
		}
	} 
	/*else if ( Settings::ESP::Boxes::type == BoxType::HITBOXES )  credits to 1337floesen - https://www.unknowncheats.me/forum/counterstrike-global-offensive/157557-drawing-hitboxes.html */ //{
	/*static std::map<int, long> playerDrawTimes;
    if ( playerDrawTimes.find( entity->GetIndex() ) == playerDrawTimes.end() ) { // haven't drawn this player yet
        playerDrawTimes[entity->GetIndex()] = Util::GetEpochTime();
    }
    matrix3x4_t matrix[128];
    if ( !entity->SetupBones( matrix, 128, 0x00000100, globalVars->curtime ) )
        return;
    studiohdr_t* hdr = modelInfo->GetStudioModel( entity->GetModel() );
    mstudiohitboxset_t* set = hdr->pHitboxSet( 0 ); // :^)
    long diffTime = Util::GetEpochTime() - playerDrawTimes.at( entity->GetIndex() );
    if ( diffTime >= 12 ) {
        for ( int i = 0; i < set->numhitboxes; i++ ) {
            mstudiobbox_t* hitbox = set->pHitbox( i );
            if ( !hitbox ) {
                continue;
            }
            Vector vMin, vMax;
            Math::VectorTransform( hitbox->bbmin, matrix[hitbox->bone], vMin );
            Math::VectorTransform( hitbox->bbmax, matrix[hitbox->bone], vMax );
            debugOverlay->DrawPill( vMin, vMax, hitbox->radius, color.r, color.g, color.b, 0.025f, color.a, false );
        }
        playerDrawTimes[entity->GetIndex()] = Util::GetEpochTime();
    }
}*/
}
static void DrawSprite( int x, int y, int w, int h, C_BaseEntity* entity ){
	if ( Settings::ESP::Sprite::type == SpriteType::SPRITE_TUX ) {
		static Texture sprite(tux_rgba, tux_width, tux_height);

		sprite.Draw(x, y, ((float)h/tux_height)*tux_width, h);
	}
	// TODO: Handle other sprites
}

static void DrawWatermark(C_BasePlayer* player){
        if (!Settings::ESP::Watermark::enabled)
                return;
int lag = TIME_TO_TICKS(player->GetSimulationTime() - player->GetOldSimulationTime());
                int woop = lag;
std::string bombStr = std::to_string(woop );
                
				Draw::AddRectFilled(1696 + 72 + 22, 2, 1696 + 224 , 30, Settings::UI::mainColor.Color()); // OUTSIDE !
                Draw::AddRectFilled(1696 + 5 + 72 + 22, 1 + 5, 1696 + 219 , 30 - 5, Settings::UI::bodyColor.Color()); // INSIDE
                Draw::AddRect(1696 - 1 + 72 + 22, 1, 1696 + 224 , 31, ImColor(0, 0, 0, 255)); // OUTSIDE
                Draw::AddRect(1695 + 5 + 72 + 22, 1 + 5, 1696 + 219  , 31 - 5, ImColor(0, 0, 0, 225)); // INSIDE
                int fps = static_cast< int >( 1.f / globalVars->frametime );
                std::string fps_string = std::to_string(fps);
//std::string name = "Kali | " + fps_string + " fps | 39ms";
std::string debugOverlay = std::to_string(fps);
std::string name = "Kali.cc | " + fps_string + " fps | " + bombStr + "FL";
Draw::AddText(1696 + 10 + 72 + 22, 10.8, name.c_str(), Settings::UI::fontColor.Color()); // TRUE

}
static void drawfire(C_BaseEntity* entity){
     int x, y, w, h;
     if ( !GetBox( entity, x, y, w, h ) )
          return;
     Vector screen_origin;
	int factor = 3;
 	debugOverlay->ScreenPosition(entity->GetVecOrigin(), screen_origin);
     Draw::FilledCircle3D( entity->GetVecOrigin(), 32, 160, Color::FromImColor(ImColor(49, 97, 0, 255)) );
     auto string = "Molotov";
     Vector2D nameSize = Draw::GetTextSize( string, esp_font );
     Draw::AddText(( int ) ( x + ( w / 2 ) - ( nameSize.x / 2 ) ), y + h + 2 + nameSize.y, string, ImColor(255, 255, 255, 255) );
     if (spawntime == 0){
     spawntime = globalVars->curtime;
     }else {
	int time = TIME_TO_TICKS(globalVars->curtime - spawntime);
     Draw::AddText(( int ) ( x + ( w / 2 ) - ( nameSize.x / 2 ) ), y + h + 2, std::to_string(489 - (TIME_TO_TICKS(globalVars->curtime - spawntime))).c_str(), ImColor(255, 255, 255, 255) );
	if (time > 489)
	spawntime = 0;
}

      //  static auto size = Vector2D(35.0f, 5.0f);
        //Draw::FilledCircle(Vector2D(screen_origin.x, screen_origin.y - size.y * 0.5f), 60, 20, Color(15, 15, 15, 187));

        //Draw::FilledRectangle(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(37, 37, 37, 255));
        //Draw::FilledRectangle(screen_origin.x - size.x * 0.5f + 2.0f, screen_origin.y - size.y * 0.5f, (size.x - 4.0f) * factor, size.y - 2.0f, Color ( 255, 0, 0, 255));

        //Draw::Rectangle(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, 255));
       // render::get().text(fonts[ESP], screen_origin.x, screen_origin.y - size.y * 0.5f + 12.0f, g_cfg.esp.molotov_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "FIRE");
       // render::get().text(fonts[GRENADES], screen_origin.x + 1.0f, screen_origin.y - size.y * 0.5f - 9.0f, g_cfg.esp.molotov_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "l");

}

static void DrawEntity( C_BaseEntity* entity, const char* string, ImColor color, int nadetype ) {
	int x, y, w, h;
	if ( !GetBox( entity, x, y, w, h ) )
		return;

	DrawBox( color, x, y, w, h, entity , Settings::ESP::Boxes::type);
	Vector2D nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(( int ) ( x + ( w / 2 ) - ( nameSize.x / 2 ) ), y + h + 2, string, color );

              switch (nadetype)
                {
               case 1:
			{
	//	static std::string freeman;
      //          freeman = std::to_string(globalVars->curtime).c_str();
    //    Vector2D smokeSize = Draw::GetTextSize( "k", astrium );
        Draw::AddCircle3D( entity->GetVecOrigin(), 144, ImColor(0,0,0,255), 32 );
//        Draw::FilledCircle(Vector2D(( int ) ( x), y + 5), 60, 20, Color(15, 15, 15, 187));
  //      Draw::Text(( int ) ( x - smokeSize.x / 2), y - 10, XORSTR("k"), astrium, Color(255, 255, 255, 187));
                        break;
			}
               case 2:
        Draw::AddCircle3D( entity->GetVecOrigin(), 150, ImColor(255, 0, 0, 255), 32 );
                        break;
	      case 3:
        Draw::AddCircle3D( entity->GetVecOrigin(), 185, ImColor(255, 0, 0, 255), 18 );
                        break;



              }

}

static void DrawEntity( C_BaseEntity* entity, const char* string, ImColor color) {
        int x, y, w, h;
        if ( !GetBox( entity, x, y, w, h ) )
                return;

        DrawBox( color, x, y, w, h, entity , Settings::ESP::Boxes::type);
        Vector2D nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(( int ) ( x + ( w / 2 ) - ( nameSize.x / 2 ) ), y + h + 2, string, color );
}

static void DrawLag(int x, int y, C_BasePlayer* player){
  const auto netchannel = GetLocalClient(-1)->m_NetChannel;
  int lag = netchannel->m_nChokedPackets;
		int woop = lag * 10;
		std::string bombStr = std::to_string(woop );
                Vector2D nameSize = Draw::GetTextSize(bombStr.c_str(), esp_font);
		float shite = 10.f;
		Vector sent;
	//	if (CreateMove::sendPacket)
	//		sent = player->GetAbsOrigin();
	//	if (!(player->GetFlags() & FL_ONGROUND))
	  //      debugOverlay->DrawPill(player->GetAbsOrigin(),sent, shite, 255, 0, 255, 100, 3 );

		Draw::AddRectFilled(x, y, x + woop, y + 20, ImColor(0, 40, 0, 255));
                Draw::AddText(x + woop - nameSize.x, y + 5, bombStr.c_str()  , ImColor( 255, 255, 255, 255 ) );
}

bool fakeass_head()
{
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

        if (!localplayer)
                return false;

        float server_time = TICKS_TO_TIME(localplayer->GetTickBase());

        auto animstate = localplayer->GetAnimState();
        if (!animstate)
                return false;

        if (localplayer->GetVelocity().Length2D() > 6.0f || (fabs(animstate->verticalVelocity) > 100.f))
                next_break = server_time + 1.1f;

        if (next_break < server_time)
        {
                next_break = server_time + 1.1f;
                return true;
        }

        return false;
}

void DrawFH(int x, int y){
if (!Settings::AntiAim::RageAntiAim::head || !Settings::AntiAim::RageAntiAim::fakepeek)
	return;

C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

if (!localplayer->GetAlive())
	return;

float server_time = TICKS_TO_TIME(localplayer->GetTickBase());

Vector2D tsize = Draw::GetTextSize(XORSTR("Fake Head") , esp_font);
Draw::AddText(x , y + 35, XORSTR("Fake Head")  ,fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );
float woop = (next_break - server_time);
Draw::AddRectFilled(x, y + 50, x + (woop * 11), y + 60, fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );

//if (Settings::AntiAim::RageAntiAim::inverted)
//Draw::AddText(x + tsize.x - 5, y + 45, XORSTR(">")  ,fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );
//else
//Draw::AddText(x, y + 45, XORSTR("<")  ,fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );

//NON INVERTED = LEFT
//INVERTED = RIGHT

}

static void DrawSkeleton( C_BasePlayer* player, C_BasePlayer* localplayer ) {
	studiohdr_t* pStudioModel = modelInfo->GetStudioModel( player->GetModel() );
	if ( !pStudioModel )
		return;

	static matrix3x4_t pBoneToWorldOut[128];
	if ( !player->SetupBones( pBoneToWorldOut, 128, 256, 0 ) )
		return;

	for ( int i = 0; i < pStudioModel->numbones; i++ ) {
		mstudiobone_t* pBone = pStudioModel->pBone( i );
		if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
			continue;

		Vector vBonePos1;
		if ( debugOverlay->ScreenPosition( Vector( pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3] ), vBonePos1 ) )
			continue;

		Vector vBonePos2;
		if ( debugOverlay->ScreenPosition( Vector( pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3] ), vBonePos2 ) )
			continue;

		Draw::AddLine( vBonePos1.x, vBonePos1.y, vBonePos2.x, vBonePos2.y, Entity::IsTeamMate(player, localplayer) ? Settings::ESP::Skeleton::allyColor.Color() : Settings::ESP::Skeleton::enemyColor.Color());
	
	}
        for ( int i = 0; i < pStudioModel->numbones; i++ ) {
                mstudiobone_t* pBone = pStudioModel->pBone( i );
                if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
                        continue;

                Vector vBonePos1;
                if ( debugOverlay->ScreenPosition( Vector( Resolver::players[player->GetIndex()].ogmatrix[i][0][3], Resolver::players[player->GetIndex()].ogmatrix[i][1][3], Resolver::players[player->GetIndex()].ogmatrix[i][2][3] ), vBonePos1 ) )
                        continue;

                Vector vBonePos2;
                if ( debugOverlay->ScreenPosition( Vector( Resolver::players[player->GetIndex()].ogmatrix[pBone->parent][0][3], Resolver::players[player->GetIndex()].ogmatrix[pBone->parent][1][3], Resolver::players[player->GetIndex()].ogmatrix[pBone->parent][2][3] ), vBonePos2 ) )
                        continue;

                Draw::AddLine( vBonePos1.x, vBonePos1.y, vBonePos2.x, vBonePos2.y, Entity::IsTeamMate(player, localplayer) ? Settings::ESP::Skeleton::allyColor.Color() : Settings::ESP::Skeleton::enemyColor.Color());

        }

}
        void rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
        {
                const auto pointsCenter = (points.at(0) + points.at(1) + points.at(2)) / 3;
                for (auto& point : points)
                {
                        point -= pointsCenter;

                        const auto tempX = point.x;
                        const auto tempY = point.y;

                        const auto theta = DEG2RAD(rotation);
                        const auto c = cos(theta);
                        const auto s = sin(theta);

                        point.x = tempX * c - tempY * s;
                        point.y = tempX * s + tempY * c;

                        point += pointsCenter;
                }
        }
        bool world_to_screen(const Vector &origin, Vector &screen)
        {
                return !debugOverlay->ScreenPosition(origin, screen);
        }

        Vector CalcAngle(const Vector src, const Vector dst) {
                Vector angles;

                Vector delta = src - dst;
                float hyp = delta.Length2D();

                angles.y = atanf(delta.y / delta.x) * M_RADPI;
                angles.x = atanf(-delta.z / hyp) * -M_RADPI;
                angles.z = 0.0f;

                if (delta.x >= 0.0f)
                        angles.y += 180.0f;

                return angles;
        }

static void DrawTracer( C_BasePlayer* player, TracerType& tracerType ) {
	Vector src3D;
	Vector src;
	src3D = player->GetVecOrigin() - Vector( 0, 0, 0 );
        bool bIsVisible = Entity::IsVisible( player, CONST_BONE_HEAD, 360.f, Settings::ESP::Filters::smokeCheck );
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

if ( tracerType != TracerType::ARROWS){

	if ( debugOverlay->ScreenPosition( src3D, src ) )
		return;

	int x = Paint::engineWidth / 2;
	int y = 0;

	if ( tracerType == TracerType::CURSOR )
		y = Paint::engineHeight / 2;
	else if ( tracerType == TracerType::BOTTOM )
		y = Paint::engineHeight;

	Draw::AddLine( ( int ) ( src.x ), ( int ) ( src.y ), x, y, ESP::GetESPPlayerColor( player, bIsVisible ) );
}else{
        auto isOnScreen = [](Vector origin, Vector& screen) -> bool
        {
                if (!world_to_screen(origin, screen))
                        return false;
   static int iScreenWidth, iScreenHeight;
    engine->GetScreenSize(iScreenWidth, iScreenHeight );


                auto xOk = iScreenWidth > screen.x; 
                auto yOk = iScreenHeight > screen.y;

                return xOk && yOk;
        };
        Vector screenPos;

 //       if (isOnScreen(player->GetAbsOrigin(), screenPos))
 //               return;
    QAngle ViewAngles;
        engine->GetViewAngles(ViewAngles);
        static int width, height;
    engine->GetScreenSize(width, height );
        auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
        auto angleYawRad = DEG2RAD(ViewAngles.y - CalcAngle(localplayer->GetEyePosition(), player->GetAbsOrigin()).y - 90.0f);
        auto radius = Settings::ESP::arrows::distance;
        auto size = Settings::ESP::arrows::size;
        auto newPointX = screenCenter.x + ((((width - (size * 3)) * 0.5f) * (radius / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
        auto newPointY = screenCenter.y + ((((height - (size * 3)) * 0.5f) * (radius / 100.0f)) * sin(angleYawRad));

        std::array <Vector2D, 3> points
        {
                Vector2D(newPointX - size, newPointY - size),
                Vector2D(newPointX + size, newPointY),
                Vector2D(newPointX - size, newPointY + size)
        };
        rotate_triangle(points, ViewAngles.y - CalcAngle(localplayer->GetEyePosition(), player->GetAbsOrigin()).y - 90.0f);
        Draw::triangle(points.at(0), points.at(1), points.at(2), Color::FromImColor(Settings::ESP::arrows::color.Color()));


}
}
static void Drawlc()
{
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer || !localplayer->GetAlive())
                return;

        if (/*!Settings::ESP::lagbones ||*/ !Settings::ThirdPerson::toggled)
                return;
        IMaterial* meterial = nullptr;

Vector last_networked_origin;
//if ( localplayer->GetVelocity().Length2D() > 30)
   // last_networked_origin = Vector(0, 0, 0);
if (AntiAim::bSend)
     last_networked_origin = localplayer->GetVecOrigin();
Vector lc;
 debugOverlay->ScreenPosition(last_networked_origin, lc);
        Draw::AddCircleFilled( lc.x, lc.y, Settings::ESP::HeadDot::size, Settings::ESP::Skeleton::allyColor.Color(), 10 );

        studiohdr_t* pStudioModel = modelInfo->GetStudioModel( localplayer->GetModel() );
        if ( !pStudioModel )
                return;


        static matrix3x4_t pBoneToWorldOut[128];
        if ( !localplayer->SetupBones( pBoneToWorldOut, 128, 256, 0 ) )
                return;
/*
        for ( int i = 0; i < pStudioModel->numbones; i++ ) {
                mstudiobone_t* pBone = pStudioModel->pBone( i );
                if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
                        continue;

                Vector vBonePos1;
                if ( debugOverlay->ScreenPosition( Vector( pBoneToWorldOut[i][0][3] + last_networked_origin.x, pBoneToWorldOut[i][1][3]+ last_networked_origin.y, pBoneToWorldOut[i][2][3] + last_networked_origin.z), vBonePos1 ) )
                        continue;

                Vector vBonePos2;
                if ( debugOverlay->ScreenPosition( Vector( pBoneToWorldOut[pBone->parent][0][3]+ last_networked_origin.x, pBoneToWorldOut[pBone->parent][1][3]+ last_networked_origin.y, pBoneToWorldOut[pBone->parent][2][3]+ last_networked_origin.z ), vBonePos2 ) )
                        continue;

                Draw::AddLine( vBonePos1.x, vBonePos1.y, vBonePos2.x, vBonePos2.y, Settings::ESP::Skeleton::allyColor.Color());

		}
*/

}

static void CustomFog(){
	if(!Settings::ESP::customfog::enabled)
		return;
	//WHY DOESNT IT FUCKING WORK?!
       static auto fog_override = cvar->FindVar(XORSTR("fog_override")); //-V807

                fog_override->SetValue(1);

        static auto fog_start = cvar->FindVar(XORSTR("fog_start"));

        if (fog_start->GetInt())
                fog_start->SetValue(0);

        static auto fog_end = cvar->FindVar(XORSTR("fog_end"));

        if (fog_end->GetInt() != Settings::ESP::customfog::distance)
                fog_end->SetValue(Settings::ESP::customfog::distance);

        static auto fog_maxdensity = cvar->FindVar(XORSTR("fog_maxdensity"));

        if (fog_maxdensity->GetFloat() != (float)Settings::ESP::customfog::density * 0.01f) //-V550
                fog_maxdensity->SetValue((float)Settings::ESP::customfog::density * 0.01f);

        char buffer_color[12];
        sprintf(buffer_color, "%i %i %i", Settings::ESP::customfog::color.Color().Value.x, Settings::ESP::customfog::color.Color().Value.y, Settings::ESP::customfog::color.Color().Value.z);

        static auto fog_color = cvar->FindVar(XORSTR("fog_color"));

                fog_color->SetValue(buffer_color);

}
static void DrawAimbotSpot( ) {
	C_BasePlayer* localplayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
	if ( !localplayer || !localplayer->GetAlive() ){
		Settings::Debug::AutoAim::target = {0,0,0};
		return;
	}
	if ( !Settings::Legitbot::AutoAim::enabled || !Settings::Legitbot::enabled ){
		Settings::Debug::AutoAim::target = {0,0,0};
		return;
	}
	if ( Settings::Debug::AutoAim::target.IsZero() )
		return;

	Vector spot2D;
	if( debugOverlay->ScreenPosition( Settings::Debug::AutoAim::target, spot2D) )
		return;

	Draw::AddLine( Paint::engineWidth / 2, Paint::engineHeight / 2, spot2D.x, spot2D.y, ImColor( 45, 235, 60 ) );
	Draw::AddCircle( Paint::engineWidth / 2, Paint::engineHeight / 2, 1, ImColor( 45, 235, 60 ) );
	Draw::AddCircle( spot2D.x, spot2D.y, 1, ImColor( 45, 235, 60 ) );

    Vector start2D = Vector(0,0,0);
    Vector end2D;
    if( !debugOverlay->ScreenPosition( lastRayEnd, end2D ) ){
        Draw::AddLine( start2D.x, start2D.y, end2D.x, end2D.y, ImColor( 255, 25, 25, 255 ) );
    }
}

static void DrawBoneMap( C_BasePlayer* player ) {
	static Vector bone2D;
	static Vector bone3D;
	studiohdr_t* pStudioModel = modelInfo->GetStudioModel( player->GetModel() );

	if( !pStudioModel )
		return;

	for( int i = 1; i < pStudioModel->numbones; i++ ){
		bone3D = player->GetBonePosition( i );
        mstudiobone_t* pBone = pStudioModel->pBone( i );
		if( !pBone )
			continue;

        if ( debugOverlay->ScreenPosition( bone3D, bone2D ) )
			continue;
		if( Settings::Debug::BoneMap::justDrawDots ){
			Draw::AddCircleFilled( bone2D.x, bone2D.y, 2.0f, ImColor( 255, 0, 255, 255 ), 10 );
		} else {
			char buffer[72];
			snprintf(buffer, 72, "%d - %s", i, pBone->pszName());
			Draw::AddText( bone2D.x, bone2D.y,buffer, ImColor( 255, 0, 255, 255 ) );
		}
	}
	IEngineClient::player_info_t entityInformation;
	engine->GetPlayerInfo( player->GetIndex(), &entityInformation );
	cvar->ConsoleDPrintf( XORSTR( "(%s)-ModelName: %s, numBones: %d\n" ), entityInformation.name, pStudioModel->name, pStudioModel->numbones );
}
static void DrawAATrace( QAngle fake, QAngle actual ) {
        C_BasePlayer* localPlayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
         if(!localPlayer->GetAlive())
            return;
	Vector src3D, dst3D, forward;
        Vector src, dst;
        trace_t tr;
        Ray_t ray;
        CTraceFilter filter;
        char* string;
        Vector2D nameSize;
        ImColor color;

        filter.pSkip = localPlayer;
        src3D = localPlayer->GetVecOrigin();
// LBY
        Math::AngleVectors( QAngle(0, *localPlayer->GetLowerBodyYawTarget(), 0), forward );
        dst3D = src3D + ( forward * 50 );

        ray.Init( src3D, dst3D );

        trace->TraceRay( ray, MASK_SHOT, &filter, &tr );

        if ( debugOverlay->ScreenPosition( src3D, src ) || debugOverlay->ScreenPosition( tr.endpos, dst ) )
                return;

        color = ImColor( 135, 235, 169 );
        Draw::AddLine( src.x, src.y, dst.x, dst.y, color );
//      Draw::AddRectFilled( ( int ) ( dst.x - 3 ), ( int ) ( dst.y - 3 ), 6, 6, color );
        string = XORSTR("LBY");
        nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(dst.x, dst.y, string, color );
//////////////////////////////////

// FAKE
        Math::AngleVectors( QAngle(0, fake.y, 0), forward );
        dst3D = src3D + ( forward * 50.f );

        ray.Init( src3D, dst3D );

        trace->TraceRay( ray, MASK_SHOT, &filter, &tr );

        if ( debugOverlay->ScreenPosition( src3D, src ) || debugOverlay->ScreenPosition( tr.endpos, dst ) )
                return;

        color = ImColor( 5, 200, 5 );
        Draw::AddLine( src.x, src.y, dst.x, dst.y, color );
//      Draw::AddRectFilled( ( int ) ( dst.x - 3 ), ( int ) ( dst.y - 3 ), 6, 6, color );
        string = XORSTR("FAKE");
        nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(dst.x, dst.y, string, color );
//////////////////////////////////

// ACTUAL
        Math::AngleVectors( QAngle(0, actual.y, 0), forward );
        dst3D = src3D + ( forward * 50.f );

        ray.Init( src3D, dst3D );

        trace->TraceRay( ray, MASK_SHOT, &filter, &tr );

        if ( debugOverlay->ScreenPosition( src3D, src ) || debugOverlay->ScreenPosition( tr.endpos, dst ) )
                return;

        color = ImColor( 225, 5, 5 );
        Draw::AddLine( src.x, src.y, dst.x, dst.y, color );
//      Draw::AddRectFilled( ( int ) ( dst.x - 3 ), ( int ) ( dst.y - 3 ), 6, 6, color );
        string = XORSTR("REAL");
        nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(dst.x, dst.y, string, color );
//////////////////////////////////

// FEET YAW
        Math::AngleVectors( QAngle(0, localPlayer->GetAnimState()->currentFeetYaw, 0), forward );
        dst3D = src3D + ( forward * 50.f );

        ray.Init( src3D, dst3D );

        trace->TraceRay( ray, MASK_SHOT, &filter, &tr );

        if ( debugOverlay->ScreenPosition( src3D, src ) || debugOverlay->ScreenPosition( tr.endpos, dst ) )
                return;

        color = ImColor( 225, 225, 80 );
        Draw::AddLine( src.x, src.y, dst.x, dst.y, color );
//      Draw::AddRectFilled( ( int ) ( dst.x - 3 ), ( int ) ( dst.y - 3 ), 6, 6, color );
        string = XORSTR("FEET");
        nameSize = Draw::GetTextSize( string, esp_font );
        Draw::AddText(dst.x, dst.y, string, color );
//////////////////////////////////
}

static void DrawAutoWall(C_BasePlayer *player) {
	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(player);
	for( int i = 0; i < 31; i++ )
	{
		auto bone = modelType->find(i);
		if( bone == modelType->end() || bone->second <= BONE_INVALID )
			continue;
		Vector bone2D;
		Vector bone3D = player->GetBonePosition(bone->second);
		if ( debugOverlay->ScreenPosition( bone3D, bone2D ) )
			continue;

		AutoWall::FireBulletData data;
		int damage = (int)AutoWall::GetDamage( bone3D, !Settings::Legitbot::friendly, data );
		char buffer[4];
		snprintf(buffer, sizeof(buffer), "%d", damage);
		Draw::AddText( bone2D.x, bone2D.y, buffer, ImColor( 255, 0, 255, 255 ) );
	}
	return;
	matrix3x4_t matrix[128];

	if( !player->SetupBones(matrix, 128, 0x100, 0.f) )
		return;
	model_t *pModel = player->GetModel();
	if( !pModel )
		return;

	studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
	if( !hdr )
		return;

	mstudiobbox_t *bbox = hdr->pHitbox((int)Hitbox::HITBOX_HEAD, 0); // bounding box
	if( !bbox )
		return;

	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);

	Vector center = ( mins + maxs ) * 0.5f;

	// 0 - center, 1 - forehead, 2 - skullcap, 3 - upperleftear, 4 - upperrightear, 5 - uppernose, 6 - upperbackofhead
	// 7 - leftear, 8 - rightear, 9 - nose, 10 - backofhead
	Vector headPoints[11] = { center, center, center, center, center, center, center, center, center, center, center };
	headPoints[1].z += bbox->radius * 0.60f;
	headPoints[2].z += bbox->radius * 1.25f;
	headPoints[3].x += bbox->radius * 0.80f;
	headPoints[3].z += bbox->radius * 0.60f;
	headPoints[4].x -= bbox->radius * 0.80f;
	headPoints[4].z += bbox->radius * 0.90f;
	headPoints[5].y += bbox->radius * 0.80f;
	headPoints[5].z += bbox->radius * 0.90f;
	headPoints[6].y -= bbox->radius * 0.80f;
	headPoints[6].z += bbox->radius * 0.90f;
	headPoints[7].x += bbox->radius * 0.80f;
	headPoints[8].x -= bbox->radius * 0.80f;
	headPoints[9].y += bbox->radius * 0.80f;
	headPoints[10].y -= bbox->radius * 0.80f;


	AutoWall::FireBulletData data;
	for ( int i = 0; i < 11; i++ ) {
		int damage = (int)AutoWall::GetDamage( headPoints[i], !Settings::Legitbot::friendly, data );
		char buffer[4];
		snprintf(buffer, sizeof(buffer), "%d", damage);

		Vector string2D;
		if( debugOverlay->ScreenPosition( headPoints[i], string2D ) )
			continue;
		Draw::AddText( string2D.x, string2D.y, buffer, ImColor( 255, 0, 255, 255 ) );
	}
}

static void DrawHeaddot( C_BasePlayer* player ) {
    Vector head2D;
    Vector head3D;

    head3D = player->GetBonePosition( CONST_BONE_HEAD );
	if ( debugOverlay->ScreenPosition( Vector( head3D.x, head3D.y, head3D.z ), head2D ) )
		return;

	bool bIsVisible = false;
	if ( Settings::ESP::Filters::visibilityCheck || Settings::ESP::Filters::legit )
		bIsVisible = Entity::IsVisible( player, CONST_BONE_HEAD, 180.f, Settings::ESP::Filters::smokeCheck );

	Draw::AddCircleFilled( head2D.x, head2D.y, Settings::ESP::HeadDot::size, ESP::GetESPPlayerColor( player, bIsVisible ), 10 );
}
int GetBlendedColor(int percentage)
{
    if (percentage < 56.5)
        return std::round(percentage * 2.55);
    else
        return 255;
}
//world_modulation(C_BasePlayer* entity)
//{}
static void DrawIndicators() {
   static int x, y;
    engine->GetScreenSize(x, y );
Vector2D indSize = Draw::GetTextSize( "LBY", esp_font );
x = x - (indSize.x / 2);
//int x = 960 - (indSize.x / 2);
//int y = 520;

        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon() );

        if (!localplayer)
               return;

    if ( !Settings::ESP::indicators::enabled )
	        return;

        float vel2D = localplayer->GetVelocity().Length2D();
		static Vector sent_origin;
		static Vector oldOrigin;
		bool canBreakLC;

     if (!(localplayer->GetFlags() & FL_ONGROUND) && (Settings::FakeLag::value >= 14 || Settings::FakeLag::adaptive)) {
			if (vel2D >= 5.0f)
                        canBreakLC = true;
                }
                else
                        canBreakLC = false;

Draw::AddText( x /2 , (y / 2) + 50, "LC", canBreakLC ? ImColor (255, 0, 0, 255) : ImColor (0, 255, 0, 255) );

    float desyncAmount = AntiAim::GetMaxDelta(localplayer->GetAnimState());

           int desyncGreenPercentage;

            if (desyncAmount > 0)
                desyncGreenPercentage = (3.4483 * desyncAmount) / 2;
            else
                desyncGreenPercentage = ((3.4483 * desyncAmount) * -1) / 2;
            int desyncRedPercentage = 100 - desyncGreenPercentage;
ImColor AColor = ImColor ( GetBlendedColor(desyncRedPercentage), GetBlendedColor(desyncGreenPercentage), 0, 255);
Draw::AddText( x /2 , (y / 2) + 60, "AA", AColor );

ImColor DColor;
		if (vel2D < 1.02 && localplayer->GetAlive() || (vel2D < 2.05 && localplayer->GetAnimState()->duckProgress == 1.0f)) // P1000000 MicroMovement Removal
		vel2D = 0;
std::string veltext = std::to_string(vel2D);
Draw::AddText( x / 2, (y / 2) + 70, veltext.c_str(), ImColor(0,255,0,255)/*Color*/);

}
static void DrawSounds( C_BasePlayer *player, ImColor playerColor ) {
    std::unique_lock<std::mutex> lock( footstepMutex, std::try_to_lock );
    if( lock.owns_lock() ){
        if ( playerFootsteps[player->GetIndex()].empty() )
            return;

        for ( auto it = playerFootsteps[player->GetIndex()].begin(); it != playerFootsteps[player->GetIndex()].end(); it++ ){
            long diff = it->expiration - Util::GetEpochTime();
	    const Vector magci = Vector(0,0,0);
            //if (player->GetDormant())
              //  player->GetVecOrigin() = it->position;

            if ( diff <= 0 ){
                playerFootsteps[player->GetIndex()].pop_front(); // This works because footsteps are a trail.
                continue;
            }
            //const Vector* magic;
            float percent = ( float ) diff / ( float ) Settings::ESP::Sounds::time;
            Color drawColor = Color::FromImColor( playerColor );
            drawColor.a = std::min( powf( percent * 2, 0.6f ), 1.f ) * drawColor.a; // fades out alpha when its below 0.5

            float circleRadius = fabs( percent - 1.0f ) * 42.0f;
            float points = std::max(12.0f, circleRadius * 0.75f);

            Draw::AddCircle3D( it->position, circleRadius, ImColor(drawColor.r, drawColor.g, drawColor.b, drawColor.a ), (int)points );
	    const Vector magic = Vector(it->position.x, it->position.y, it->position.z);
	    //magic = it->position;
        }
        footstepMutex.unlock();
    }
}

static void DrawVelGraph(){
    static std::vector<float> velData(120, 0);
C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    Vector vecVelocity = localplayer->GetVelocity();
    float currentVelocity = sqrt(vecVelocity.x * vecVelocity.x + vecVelocity.y * vecVelocity.y);

    velData.erase(velData.begin());
    velData.push_back(currentVelocity * 2);

    const auto heighth = Paint::engineHeight;
    const auto widthh = Paint::engineWidth;

    for (auto i = 0; i < velData.size() - 1; i++)
    {
        int cur = velData.at(i);
        int next = velData.at(i + 1);

        Draw::AddLine(
            widthh / 2 + (velData.size() * 5 / 2) - (i - 1) * 5.f,
            heighth / 2 - (std::clamp(cur, 0, 450) * .2f) + 200,
            widthh / 2 + (velData.size() * 5 / 2) - i * 5.f,
            heighth / 2 - (std::clamp(next, 0, 450) * .2f) + 200,
            ImColor( 255, 255, 255, 255)
        );
    }
}

static void DrawKeyBinds(int x, int y) {
int b = x;
int c = y;
C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
		int width, height;
engine->GetScreenSize(width,height);
Vector2D nameSize = Draw::GetTextSize(XORSTR("AntiAim Inverter [Toggled]"), esp_font);
    Draw::AddRectFilled(x - 5, c - 5, x + nameSize.x , y + 80, ImColor(40, 40, 40, 225));
    Draw::AddRectFilled(x, c, x + nameSize.x - 5, y + 75, ImColor(0, 0, 0, 235));
    Draw::AddRect(x - 5, c - 5, x + nameSize.x , y + 80, ImColor(200, 200, 200, 50));
    Draw::AddRect(x, c, x + nameSize.x - 5, y + 75, ImColor(200, 200, 200, 50));

    Draw::AddLine(x, c, x + nameSize.x - 5, c, Settings::ESP::Watermark::color.Color());

if (inputSystem->IsButtonDown(Settings::Autoblock::key) && Settings::Autoblock::enabled){
                Draw::AddText( x + 2, y + 1, "AutoBlock  [Holding]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if (inputSystem->IsButtonDown(Settings::AntiAim::SlowWalk::key) && Settings::AntiAim::SlowWalk::enabled){
                Draw::AddText( x + 2, y + 2, "SlowWalk  [Holding]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if (Settings::AntiAim::RageAntiAim::inverted && Settings::AntiAim::RageAntiAim::enable || Settings::AntiAim::LegitAntiAim::inverted){
                Draw::AddText( x + 2, y + 3, "AA Inverter [Toggled]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if (inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey) && Settings::AntiAim::FakeDuck::enabled){
                Draw::AddText( x + 2, y + 4, "FakeDuck  [Holding]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if (inputSystem->IsButtonDown(Settings::Ragebot::quickpeek::key) && Settings::Ragebot::quickpeek::enabled){
                Draw::AddText( x + 2, y + 5, "QuickPeek [Holding]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if (AntiAim::ManualAntiAim::alignLeft && Settings::AntiAim::ManualAntiAim::Enable){
                Draw::AddText( x + 2, y + 5, "Manual AA [LEFT]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}else if (AntiAim::ManualAntiAim::alignBack && Settings::AntiAim::ManualAntiAim::Enable){
                Draw::AddText( x + 2, y + 5, "Manual AA [BACK]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}else if (AntiAim::ManualAntiAim::alignRight && Settings::AntiAim::ManualAntiAim::Enable){
                Draw::AddText( x + 2, y + 5, "Manual AA [RIGHT]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if ( Settings::Resolver::resolveAll && Settings::Resolver::manual && Settings::Resolver::forcebrute){
                Draw::AddText( x + 2, y + 5, "Resolver Override [BRUTE]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}
if ( Settings::Ragebot::onshot::enabled && inputSystem->IsButtonDown(Settings::Ragebot::onshot::button)){
                Draw::AddText( x + 2, y + 5, "Wait for onshot [Holding]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;
}


/*if (Settings::AntiAim::RageAntiAim::head && Settings::AntiAim::RageAntiAim::fakepeek){

float server_time = TICKS_TO_TIME(localplayer->GetTickBase());

Vector2D tsize = Draw::GetTextSize(XORSTR("Fake Head [       ]") , esp_font);
//Draw::AddText(x , y + 35, XORSTR("Fake Head")  ,fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );
float woop = (next_break - server_time);
Draw::AddRectFilled(x + tsize.x - 10, y + 5, x + tsize.x - 10 + (woop * 11), y + 5 + nameSize.y, fakeass_head() ? ImColor( 0, 255, 0, 255 ) : ImColor( 255, 0, 0, 255 ) );

                Draw::AddText( x + 2, y + 5, "Fake Head [       ]", ImColor( 255, 255, 255, 255 ) );
y = y + 10;

//NON INVERTED = LEFT
//INVERTED = RIGHT

}
*/

// FindHudElement = (PatternFinder::FindPatternInModule( XORSTR( "/client_client.so" ),( unsigned char* ) XORSTR("\xE8\x00\x00\x00\x00\x48\x8D\x50\xE0"),XORSTR( "x????xxxx" )) + 1);
//auto hud = (PatternFinder::FindPatternInModule( XORSTR( "/client_client.so" ),( unsigned char* ) XORSTR("\x53\x48\x8D\x3D\x00\x00\x00\x00\x48\x83\xEC\x10\xE8"),XORSTR( "xxxx????xxxxx" )) + 1);
  //  const auto deathNotice = FindHudElement(hud, "CCSGO_HudDeathNotice");
    //if (!deathNotice)
      //  return;
  //  const auto deathNoticePanel = (*(IVPanel**)(*(deathNotice - 5 + 22) + 4));
//    fakePrime = reinterpret_cast<std::uint8_t*>(PatternFinder::FindPatternInModule( XORSTR( "/client_client.so" ),( unsigned char* ) XORSTR("\xE8\x00\x00\x00\x00\x48\x8D\x50\xE0"),XORSTR( "xxxxx" )) - 1);
}

bool FakeDuckCheck(C_BasePlayer* player){
auto animstate = player->GetAnimState();

        if (animstate){
                if (player->GetFlags() & FL_ONGROUND){
                                static auto stored_tick = 0;
                                static int crouched_ticks;

                                if (animstate->duckProgress)
                                {
                                        if (animstate->duckProgress < 0.9f && animstate->duckProgress > 0.5f)
                                        {
                                                if (stored_tick != global::tickcount)
                                                {
                                                        crouched_ticks++;
                                                        stored_tick = global::tickcount;
                                                }

                                                return crouched_ticks > 16;
                                        }
                                        else
                                                crouched_ticks = 0;
                                }

                                return false;

                        }
                }
        return false;
}

static void DrawPlayerHealthBars( C_BasePlayer* player, int x, int y, int w, int h, ImColor color, BarType& bartype ) {
	
	int boxSpacing = 3;
	ImColor barColor;

	// clamp it to 100
	int healthValue = std::max( 0, std::min( player->GetHealth(), 100 ) );
	float HealthPerc = healthValue / 100.f;

	int barx = x;
	int bary = y;
	int barw = w;
	int barh = h;

	barColor = ImColor(
				std::min( 510 * ( 100 - healthValue ) / 100, 255 ),
				std::min( 510 * healthValue / 100, 255 ),
				25,
				255);
	if ( bartype == BarType::VERTICAL ) {
		barw = 4; // outline(1px) + bar(2px) + outline(1px) = 6px;
		barx -= barw + boxSpacing; // spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 8 px
		Draw::AddRectFilled( barx, bary, barx + barw, bary + barh, ImColor( 10, 10, 10, 255 ) );

		if ( HealthPerc > 0 )
			Draw::AddRectFilled( barx + 1, bary + ( barh * ( 1.f - HealthPerc ) ) + 1,
									 barx + barw - 1, bary + barh - 1, barColor);

		barsSpacing.x += barw;
	} 
	else if ( bartype == BarType::VERTICAL_RIGHT ) {
		barx += barw + boxSpacing; // spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 8 px
		barw = 4; // outline(1px) + bar(2px) + outline(1px) = 6px;

		Draw::AddRectFilled( barx, bary, barx + barw, bary + barh, ImColor( 10, 10, 10, 255 ) );

		if ( HealthPerc > 0 )
			Draw::AddRectFilled( barx + 1, bary + ( barh * ( 1.f - HealthPerc ) ) + 1, barx + barw - 1,
									 bary + barh - 1, barColor );

		barsSpacing.x += barw;
	} 
	else if ( bartype == BarType::HORIZONTAL ) {
		bary += barh + boxSpacing; // player box(?px) + spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 5 px
		barh = 4; // outline(1px) + bar(2px) + outline(1px) = 4px;

		Draw::AddRectFilled( barx, bary, barx + barw, bary + barh, ImColor( 10, 10, 10, 255 ) );

		if ( HealthPerc > 0 ) {
			barw *= HealthPerc;
			Draw::AddRect( barx + 1, bary + 1, barx + barw - 1, bary + barh - 1, barColor );
		}
		barsSpacing.y += barh;
	} 
	else if ( bartype == BarType::HORIZONTAL_UP) {
		barh = 4; // outline(1px) + bar(2px) + outline(1px) = 4px;
		bary -= barh + boxSpacing; // spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 5 px

		Draw::AddRect( barx - 1, bary - 1, barx + barw + 1, bary + barh + 1, ImColor( 255, 255, 255, 170 ) );
		Draw::AddRectFilled( barx, bary, barx + barw, bary + barh, ImColor( 10, 10, 10, 255 ) );

		if ( HealthPerc > 0 ) {
			barw *= HealthPerc;
			Draw::AddRect( barx + 1, bary + 1, barx + barw - 1, bary + barh - 1, barColor );
		}
		barsSpacing.y += barh;
	}
        else if ( bartype == BarType::BATTERY) {

float flBoxes = std::ceil(player->GetHealth() / 10.f );
  barw = 4; // outline(1px) + bar(2px) + outline(1px) = 6px;
                barx -= barw + boxSpacing; // spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 8 px
	float flX = x - 7 - barh / 4.f; float flY = y - 1;
	float flHeight = barh / 10.f;
	float flMultiplier = 12 / 360.f; flMultiplier *= flBoxes - 1;
	Color ColHealth = Color::FromHSB( flMultiplier, 1, 1 );

	 Draw::AddRectFilled( flX, flY, 4, barh + 2, ImColor( 80, 80, 80, 125 ) );
	Draw::AddRect( flX, flY, 4, barh + 2, ImColor( 10, 10, 10, 255 ) );
	Draw::AddRectFilled( flX + 1, flY, 2, flHeight * flBoxes + 1, barColor );

	for ( int i = 0; i < 10; i++ )
		Draw::AddLine( flX, flY + i * flHeight, flX + 4, flY + i * flHeight, ImColor( 10, 10, 10, 255 ) );

        barsSpacing.y += barh;
}
}

static void DrawPlayerText( C_BasePlayer* player, C_BasePlayer* localplayer, int x, int y, int w, int h ) {
	int boxSpacing = Settings::ESP::Boxes::enabled ? 3 : 0;
	int lineNum = 1;
	int nameOffset = ( int ) ( Settings::ESP::Bars::type == BarType::HORIZONTAL_UP ? boxSpacing + barsSpacing.y : 0 );

	Vector2D textSize = Draw::GetTextSize( XORSTR( "Hi" ), esp_font );
	// draw name
	if ( Settings::ESP::Info::name || Settings::ESP::Info::clan ) {
		std::string displayString;
		IEngineClient::player_info_t playerInfo;
		engine->GetPlayerInfo( player->GetIndex(), &playerInfo );
		if ( Settings::ESP::Info::clan )
			displayString += std::string( ( *csPlayerResource )->GetClan( player->GetIndex() ) );

		if ( Settings::ESP::Info::clan && Settings::ESP::Info::name )
			displayString += " ";

		if ( Settings::ESP::Info::name )
			displayString += playerInfo.name;

		Vector2D nameSize = Draw::GetTextSize( displayString.c_str(), esp_font );
		Draw::AddText( x + ( w / 2 ) - ( nameSize.x / 2 ), ( y - textSize.y - nameOffset ), displayString.c_str(), Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
		lineNum++;
	}
	if( Settings::Resolver::resolveAll && /*!(Entity::IsTeamMate(player,localplayer)) &&*/ Settings::Resolver::manual){
 		                std::string bombStr = "Max Desync Delta " + std::to_string(AntiAim::GetMaxDelta(player->GetAnimState()));
		
		Vector2D rankSize = Draw::GetTextSize( bombStr.c_str(), esp_font );
                Draw::AddText( ( x + ( w / 2 ) - ( rankSize.x / 2 ) ),( y - ( textSize.y * lineNum ) - nameOffset ), bombStr.c_str(), Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
                lineNum++;
		}
	// draw steamid
	if ( Settings::ESP::Info::steamId ) {
		IEngineClient::player_info_t playerInfo;
		engine->GetPlayerInfo( player->GetIndex(), &playerInfo );
		Vector2D rankSize = Draw::GetTextSize( playerInfo.guid, esp_font );
		Draw::AddText( ( x + ( w / 2 ) - ( rankSize.x / 2 ) ),( y - ( textSize.y * lineNum ) - nameOffset ), playerInfo.guid, Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
		lineNum++;
	}

	// draw rank
	if ( Settings::ESP::Info::rank ) {
		int rank = *( *csPlayerResource )->GetCompetitiveRanking( player->GetIndex() );

		if ( rank >= 0 && rank < 19 ) {
			Vector2D rankSize = Draw::GetTextSize( ESP::ranks[rank], esp_font );
			Draw::AddText( ( x + ( w / 2 ) - ( rankSize.x / 2 ) ), ( y - ( textSize.y * lineNum ) - nameOffset ), ESP::ranks[rank], Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
		}
	}

	// health
	if ( Settings::ESP::Info::health ) {
		std::string buf = std::to_string( player->GetHealth() ) + XORSTR( " HP" );
		Draw::AddText( x + w + boxSpacing, ( y + h - textSize.y ), buf.c_str(), Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
	}

	// armor
	if ( Settings::ESP::Info::armor ) {
		std::string buf = std::to_string( player->GetArmor() ) + (player->HasHelmet() ? XORSTR(" AP*") : XORSTR(" AP"));
		Draw::AddText( x + w + boxSpacing, ( y + h - (textSize.y / 3) ), buf.c_str(), Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
	   //             Draw::AddText( x + w + boxSpacing, ( y + h - (textSize.y / 3) ), "", Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
	}

	// weapon
	C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle( player->GetActiveWeapon() );
	if ( Settings::ESP::Info::weapon || Settings::ESP::Info::tweapon) {
	// if (!localplayer->GetAlive())
//return;
		auto activeeWeapon =  *activeWeapon->GetItemDefinitionIndex();
		std::string modelName;
		int offset = ( int ) ( boxSpacing);
Vector2D weaponTextSizeF;
if (Settings::ESP::Info::weapon){
if ( ESP::Weaponsi.find(activeeWeapon) != ESP::Weaponsi.end()){
modelName = ESP::Weaponsi.find(activeeWeapon)->second;
}
                                 weaponTextSizeF = Draw::GetTextSize(modelName.c_str() , astrium );

}else{
                                weaponTextSizeF = Draw::GetTextSize(modelName.c_str() , esp_font );
		std::string modelName = Util::Items::GetItemDisplayName( *activeWeapon->GetItemDefinitionIndex() );
}
		if (localplayer->GetAlive())
		Draw::Text( ( x + ( w / 2 ) - ( weaponTextSizeF.x / 2 ) ), y + h + offset, modelName.c_str(), astrium,Color::FromImColor( Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color()) );


	}
	// draw info
	std::vector<std::string> stringsToShow;

	if (Settings::ESP::Info::money)
	{
int wap;
int previous_ticks[128];
auto ticks = TIME_TO_TICKS(player->GetSimulationTime() - player->GetOldSimulationTime());
if (ticks == 0 && previous_ticks[player->GetIndex()] > 0) {
wap = previous_ticks[player->GetIndex()] - 1;
}
else {
previous_ticks[player->GetIndex()] = ticks;
wap = ticks;
}

if (wap > 2)
		stringsToShow.push_back("Cheating?, CP:" + std::to_string(wap) );
	}
	if ( Settings::ESP::Info::scoped && player->IsScoped() )
		stringsToShow.push_back( XORSTR( "B" ) );
	if (Settings::ESP::Info::reloading)
	{
		CUtlVector<AnimationLayer> *layers = player->GetAnimOverlay();
		for (int i = 0; i <= layers->Count(); i++)
		{
			if (player->GetSequenceActivity(layers->operator[](i).m_nSequence) == (int)CCSGOAnimStatePoses::ACT_CSGO_RELOAD && layers->operator[](i).m_flWeight != 0.f)
				stringsToShow.push_back( XORSTR( "Reloading" ) );
		}
	}
	if ( Settings::ESP::Info::flashed && player->IsFlashed())
		stringsToShow.push_back( XORSTR( "Flashed" ) );
	if ( Settings::ESP::Info::planting && Entity::IsPlanting( player ) )
		stringsToShow.push_back( XORSTR( "Planting" ) );
	if ( Settings::ESP::Info::planting && player->GetIndex() == ( *csPlayerResource )->GetPlayerC4() )
		stringsToShow.push_back( XORSTR( "Bomb Carrier" ) );
	if ( Settings::ESP::Info::hasDefuser && player->HasDefuser() )
		stringsToShow.push_back( XORSTR( "r" ) );
	if ( Settings::ESP::Info::defusing && player->IsDefusing() )
		stringsToShow.push_back( XORSTR( "Defusing" ) );
	if ( Settings::ESP::Info::grabbingHostage && player->IsGrabbingHostage() )
		stringsToShow.push_back( XORSTR( "Hostage Carrier" ) );
	if ( Settings::ESP::Info::rescuing && !Entity::IsTeamMate(player, localplayer) && localplayer->GetAlive() && !player->GetDormant()){
		//stringsToShow.push_back( std::to_string(Resolver::players[player->GetIndex()].delta).c_str() );
		std::string flagst;
		switch (Resolver::players[player->GetIndex()].flags)
		{
			case Resolver::rflag::LBY:
				flagst = "LBY";
				break;
			case Resolver::rflag::LOW:
				flagst = "LOW";
				break;
			case Resolver::rflag::NOTIME:
				flagst = "NONE";
				break;
                        case Resolver::rflag::AP:
                                flagst = "AP(1)";
                                break;
                        case Resolver::rflag::BRUTE:
                                flagst = "BRUTE(";
				flagst += std::to_string(Resolver::players[player->GetIndex()].MissedCount);
				flagst +=+ ")";
                                break;
                        case Resolver::rflag::LAA:
                                flagst = "Legit AA";
                                break;


		}
                stringsToShow.push_back(flagst.c_str());

	}
        if (FakeDuckCheck(player)  && Settings::ESP::Info::Fakeduck)
                stringsToShow.push_back(XORSTR("FD"));
	if (player->GetDormant())
		stringsToShow.push_back(XORSTR("Dormant"));
	if ( Settings::ESP::Info::location )
		stringsToShow.push_back( player->GetLastPlaceName() );
	if (Settings::Debug::AnimLayers::draw){
		CUtlVector<AnimationLayer> *layers = player->GetAnimOverlay();
		for ( int i = 0; i <= layers->Count(); i++ ){
			stringsToShow.push_back( Util::GetActivityName(player->GetSequenceActivity(layers->operator[](i).m_nSequence)) );
		}
	}


	for( unsigned int i = 0; i < stringsToShow.size(); i++ ){
if (stringsToShow[i] != "B" && stringsToShow[i] != "r"){
		Draw::AddText( x + w + boxSpacing, ( y + ( i * ( textSize.y + 2 ) ) ), stringsToShow[i].c_str(), Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color() );
}
if (stringsToShow[i] == "r"){
                Draw::Text( x + w + boxSpacing, ( y + ( i * ( textSize.y + 2 ) ) ), stringsToShow[i].c_str(), astrium,Color::FromImColor( Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color()) );
}

if (stringsToShow[i] == "B"){
		Draw::Text( x + w + boxSpacing, ( y + ( i * ( textSize.y + 2 ) ) ), stringsToShow[i].c_str(), icon_font,Color::FromImColor( Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color()) );
}

	}
//	if(player->IsScoped()){
 
//Draw::TextW( x + w + boxSpacing, ( y + h ), L"\356\200\202", icon_font,Color::FromImColor( Entity::IsTeamMate(player, localplayer) ? Settings::ESP::allyInfoColor.Color() : Settings::ESP::enemyInfoColor.Color()) );
//}

}

static void DrawPlayer(C_BasePlayer* player)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !player)
		return;

	bool bIsVisible = false;
	if (Settings::ESP::Filters::visibilityCheck || Settings::ESP::Filters::legit)
	{
		bIsVisible = Entity::IsVisible(player, CONST_BONE_HEAD, 360.f, Settings::ESP::Filters::smokeCheck);
		if (!bIsVisible && Settings::ESP::Filters::legit)
			return;
	}

	ImColor playerColor = ESP::GetESPPlayerColor(player, bIsVisible);

	int x, y, w, h;
	if (!GetBox(player, x, y, w, h))
		return;

	if ( !Entity::IsTeamMate(player, localplayer) ) // mean the entity is enemy
	{
		if (Settings::ESP::FilterEnemy::Boxes::enabled ) // Drawing the box around player
			DrawBox(playerColor, x, y, w, h, player, Settings::ESP::FilterEnemy::Boxes::type);
		
		if (Settings::ESP::FilterEnemy::HelthBar::enabled) // Drawing the health bar
			DrawPlayerHealthBars(player, x, y, w, h, playerColor, Settings::ESP::FilterEnemy::HelthBar::type);
		
		if ( Settings::ESP::FilterEnemy::Tracers::enabled ) // Drawing tracers
			DrawTracer(player, Settings::ESP::FilterEnemy::Tracers::type);
		
		if ( Settings::ESP::FilterEnemy::Skeleton::enabled ) // Drawing Skeleton Bitch Fuzion is god
			DrawSkeleton(player, localplayer);
		
		if (Settings::ESP::FilterEnemy::HeadDot::enabled) // Draw the head dot white indicate to neck sry :P
			DrawHeaddot(player);
		
		/* Checks various Text Settings */
		if (Settings::ESP::FilterEnemy::playerInfo::enabled)
			DrawPlayerText( player, localplayer, x, y, w, h );
	}
	else if ( player != localplayer ) // then it means it is our friend nigga
	{
		if (Settings::ESP::FilterAlise::Boxes::enabled)
			DrawBox(playerColor, x, y, w, h, player, Settings::ESP::FilterAlise::Boxes::type);
		
		if (Settings::ESP::FilterAlise::HelthBar::enabled) // Drawing the helth bar
			DrawPlayerHealthBars(player, x, y, w, h, playerColor, Settings::ESP::FilterAlise::HelthBar::type);
		
		if ( Settings::ESP::FilterAlise::Tracers::enabled ) // Drawing tracers
			DrawTracer(player, Settings::ESP::FilterAlise::Tracers::type);
		
		if ( Settings::ESP::FilterAlise::Skeleton::enabled ) // Drawing Skeliton Bitch Fuzion is god
			DrawSkeleton(player, localplayer);
		
		if (Settings::ESP::FilterAlise::HeadDot::enabled) // Draw the head dot white indicate to kneck sry :P
			DrawHeaddot(player);
		
		/* Checks various Text Settings */
		if (Settings::ESP::FilterAlise::playerInfo::enabled)
			DrawPlayerText( player, localplayer, x, y, w, h );
	}
	else if ( player == localplayer && Settings::ThirdPerson::toggled) // There is no need to check this. must I try to be secure xd
	{
		if (Settings::ESP::FilterLocalPlayer::Boxes::enabled )
			DrawBox(playerColor, x, y, w, h, player, Settings::ESP::FilterLocalPlayer::Boxes::type);
		
		if (Settings::ESP::FilterLocalPlayer::HelthBar::enabled) // Drawing the helth bar
			DrawPlayerHealthBars(player, x, y, w, h, playerColor, Settings::ESP::FilterLocalPlayer::HelthBar::type);
		
		if ( Settings::ESP::FilterLocalPlayer::Tracers::enabled ) // Drawing tracers
			DrawTracer(player, Settings::ESP::FilterLocalPlayer::Tracers::type);
		
		if ( Settings::ESP::FilterLocalPlayer::Skeleton::enabled ) // Drawing Skeliton Bitch Fuzion is god
			DrawSkeleton(player, localplayer);
		
		if (Settings::ESP::FilterLocalPlayer::HeadDot::enabled) // Draw the head dot white indicate to kneck sry :P
			DrawHeaddot(player);
		
		/* Checks various Text Settings */
		if (Settings::ESP::FilterLocalPlayer::playerInfo::enabled && Settings::ThirdPerson::toggled)
			DrawPlayerText( player, localplayer, x, y, w, h );
		
	}
	if (Settings::ESP::Sprite::enabled)
       DrawSprite(x, y, w, h, player);
	// This is broken xd
	// if (Settings::ESP::BulletTracers::enabled)
	// 	DrawBulletTrace(player);
	// else if (!Entity::IsTeamMate(player, localplayer) && Settings::ESP::FilterEnemy::BulletTracers::enabled);
	// 	DrawBulletTrace(player);
	if (Settings::Debug::AutoWall::debugView)
		DrawAutoWall(player);

	if ( Settings::Debug::BoneMap::draw )
		DrawBoneMap( player );

	if (Settings::Debug::AutoAim::drawTarget)
		DrawAimbotSpot();

    if (Settings::ESP::Sounds::enabled) {
		DrawSounds( player, playerColor );
	}
	
}
static void DrawBulletTracers(IGameEvent* event)
{
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer)
        return;
        if (!Settings::ESP::tracebullet::enabled)
        return;
        if (!(strstr(event->GetName(), XORSTR("bullet_impact"))))
                return;
	if (!event->GetInt(XORSTR("userid")))
		return;
	float x = event->GetFloat(XORSTR("x"));
        float y = event->GetFloat(XORSTR("y"));
        float z = event->GetFloat(XORSTR("z"));
        C_BasePlayer* attacker = (C_BasePlayer*) entityList->GetClientEntity(engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))));
	if (attacker != localplayer && Settings::ESP::tracebullet::local)
		return;
	if (!attacker || attacker->GetDormant())
		return;

	float shite = 0.5f;
	ImColor color;
                           if (!Entity::IsTeamMate(attacker, localplayer))
                                        color = Settings::ESP::tracebullet::enemycolor.Color();
                                else
                                        color = Settings::ESP::tracebullet::friendcolor.Color();

	debugOverlay->DrawPill( attacker->GetEyePosition(), Vector(x, y, z), shite, color.Value.x * 255, color.Value.y * 255, color.Value.z * 255, 100, 3 );

 }
static void DrawImpacts(IGameEvent* event)
{
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
	return;
	if (!Settings::ESP::showimpacts)
	return;
	if (!(strstr(event->GetName(), XORSTR("player_hurt"))))
		return;
if (!event->GetInt(XORSTR("userid")) || !event->GetInt(XORSTR("attacker")))
return;
		C_BasePlayer* victim = (C_BasePlayer*) entityList->GetClientEntity(engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))));
		C_BasePlayer* attacker = (C_BasePlayer*) entityList->GetClientEntity(engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker"))));
	if (attacker != localplayer)
	return;

    matrix3x4_t matrix[128];

    if ( !victim->SetupBones( matrix, 128, 0x00000100, globalVars->curtime ) )
        return;

        studiohdr_t* pStudioModel = modelInfo->GetStudioModel( victim->GetModel() );
        if ( !pStudioModel )
                return;
	if (Entity::IsTeamMate(victim, localplayer))
	return;
        static matrix3x4_t pBoneToWorldOut[128];
        if ( !victim->SetupBones( pBoneToWorldOut, 128, 256, 0 ) )
                return;

    studiohdr_t* hdr = modelInfo->GetStudioModel( victim->GetModel() );
    mstudiohitboxset_t* set = hdr->pHitboxSet( 0 );

        for ( int i = 0; i < set->numhitboxes; i++ ) {
            mstudiobbox_t* hitbox = set->pHitbox( i );
            if ( !hitbox ) {
                continue;
            }
            Vector vMin, vMax;
            Math::VectorTransform( hitbox->bbmin, matrix[hitbox->bone], vMin );
            Math::VectorTransform( hitbox->bbmax, matrix[hitbox->bone], vMax );

            debugOverlay->DrawPill( vMin, vMax, hitbox->radius, 20, 187, 0, 100, 3 );
        }

}
static void DrawBomb(C_BaseCombatWeapon* bomb, C_BasePlayer* localplayer)
{
	if (!(*csGameRules) || !(*csGameRules)->IsBombDropped())
		return;

	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(bomb, localplayer))
		return;

	DrawEntity(bomb, XORSTR("Bomb"), Settings::ESP::bombColor.Color());
}

static void DrawPlantedBomb(C_PlantedC4* bomb, C_BasePlayer* localplayer)
{
	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(bomb, localplayer))
		return;

	ImColor color = bomb->GetBombDefuser() != -1 || bomb->IsBombDefused() ? Settings::ESP::bombDefusingColor.Color() : Settings::ESP::bombColor.Color();
	float defuseTimer = bomb->GetBombDefuseCountDown() - globalVars->curtime;
	float bombTimer = bomb->GetBombTime() - globalVars->curtime;
	std::stringstream displayText;
	if (bomb->IsBombDefused() || !bomb->IsBombTicking() || bombTimer <= 0.f)
	{
		displayText << XORSTR("Bomb");
	}
	else
	{
		C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
		Vector vecOrigin = bomb->GetVecOrigin();

		float flDistance = localplayer->GetEyePosition().DistTo(vecOrigin);

		float a = 450.7f;
		float b = 75.68f;
		float c = 789.2f;
		float d = ((flDistance - b) / c);
		float flDamage = a * expf(-d * d);

		float damage = std::max((int) ceilf(GetArmourHealth(flDamage, localplayer->GetArmor())), 0);
		displayText << XORSTR("Bomb: ") << std::fixed << std::showpoint << std::setprecision(1) << bombTimer << XORSTR(", damage: ") << (int) damage;
		Draw::AddRectFilled(-1920, 0, bombTimer * 50, 10, ImColor( 255, 0, 0, 150 ) );
		std::string bombStr = std::to_string(roundf(bombTimer * 100) / 100 );
		std::string st = bombStr.substr(0, bombStr.size()-4);
                Vector2D nameSize = Draw::GetTextSize(st.c_str(), esp_font);
		Draw::AddText(bombTimer * 50 - nameSize.x, 0, st.c_str()  , ImColor( 255, 255, 255, 255 ) );


	if( bomb->GetBombDefuser() != -1 ){
                Draw::AddRectFilled(-1920, -15, defuseTimer * 50, 15, ImColor( 0, 0, 255, 150 ) );
                //Draw::AddText( XORSTR("") << defuseTimer * 50, -15, XORSTR("") << defuseTimer, ImColor( 0, 0, 0, 255 ) );

	}
	}
        int x, y, w, h;
GetBox( bomb, x, y, w, h );
                               Vector2D weaponTextSizeF = Draw::GetTextSize("o" , astrium );
                Draw::Text( x + ( w / 2 ) - ( weaponTextSizeF.x / 2)  , y + h + 2, "o", astrium,Color::FromImColor(ImColor( 255, 255, 255, 255 )));
	DrawEntity(bomb, displayText.str().c_str(), color);
}

static void DrawDefuseKit(C_BaseEntity* defuser, C_BasePlayer* localplayer)
{
	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(defuser, localplayer))
		return;
	DrawEntity(defuser, XORSTR("Defuser"), Settings::ESP::defuserColor.Color());
}

static void DrawDroppedWeapons(C_BaseCombatWeapon* weapon, C_BasePlayer* localplayer)
{
	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(weapon, localplayer))
		return;

	Vector vOrig = weapon->GetVecOrigin();
	int owner = weapon->GetOwner();

	if (owner > -1 || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
		return;

	std::string modelName = Util::Items::GetItemDisplayName(*weapon->GetItemDefinitionIndex());

	if (weapon->GetAmmo() > 0)
	{
		modelName += XORSTR(" | ");
		modelName += std::to_string(weapon->GetAmmo());
	}

	DrawEntity(weapon, modelName.c_str(), Settings::ESP::weaponColor.Color());
}

static void DrawHostage(C_BaseEntity* hostage, C_BasePlayer* localplayer)
{
	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(hostage, localplayer))
		return;

	DrawEntity(hostage, XORSTR("Hostage"), Settings::ESP::hostageColor.Color());
}

static void DrawChicken(C_BaseEntity* chicken)
{
	DrawEntity(chicken, XORSTR("Chicken"), Settings::ESP::chickenColor.Color());
}

static void DrawFish(C_BaseEntity* fish)
{
	DrawEntity(fish, XORSTR("Fish"), Settings::ESP::fishColor.Color());
}

static void DrawSafe(C_BaseEntity* safe, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(safe, localplayer))
		return;
	if (*(bool*)((uintptr_t)safe + offsets.DT_BRC4Target.m_bBrokenOpen))
		return;
    DrawEntity(safe, XORSTR("Safe"), Settings::ESP::DangerZone::safeColor.Color());
}

static void DrawAmmoBox(C_BaseEntity *ammobox, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(ammobox, localplayer))
		return;
    DrawEntity(ammobox, XORSTR("Ammo box"), Settings::ESP::DangerZone::ammoboxColor.Color());
}

static void DrawSentryTurret(C_BaseEntity *sentry, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(sentry, localplayer))
		return;

	std::string name = XORSTR("Sentry Turret");
	name += XORSTR(" | ");
	name += std::to_string(*(int*)((uintptr_t)sentry + offsets.DT_Dronegun.m_iHealth));
	name += XORSTR(" HP");
    DrawEntity(sentry, name.c_str(), Settings::ESP::DangerZone::dronegunColor.Color());
}

static void DrawRadarJammer(C_BaseEntity *jammer, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(jammer, localplayer))
		return;
    DrawEntity(jammer, XORSTR("Radar Jammer"), Settings::ESP::DangerZone::radarjammerColor.Color());
}

static void DrawExplosiveBarrel(C_BaseEntity *barrel, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(barrel, localplayer))
		return;
	studiohdr_t* barrelModel = modelInfo->GetStudioModel(barrel->GetModel());
	if (!barrelModel)
		return;
	std::string mdlName = barrelModel->name;
	mdlName = mdlName.substr(mdlName.find_last_of('/') + 1);
	if (mdlName.find(XORSTR("exploding_barrel.mdl")) == mdlName.npos)
		return;
    DrawEntity(barrel, XORSTR("Explosive Barrel"), Settings::ESP::DangerZone::barrelColor.Color());
}

static void DrawDrone(C_BaseEntity *drone, C_BasePlayer* localplayer)
{
	// TODO: Add Drone info (owner/package).
	if (!dzShouldDraw(drone, localplayer))
		return;
    DrawEntity(drone, XORSTR("Drone"), Settings::ESP::DangerZone::droneColor.Color());
}

static void DrawCash(C_BaseEntity *cash, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(cash, localplayer))
		return;
    DrawEntity(cash, XORSTR("Cash"), Settings::ESP::DangerZone::cashColor.Color());
}

static void DrawTablet(C_BaseEntity *tablet, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(tablet, localplayer))
		return;
    DrawEntity(tablet, XORSTR("Tablet"), Settings::ESP::DangerZone::tabletColor.Color());
}

static void DrawHealthshot(C_BaseEntity *healthshot, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(healthshot, localplayer))
		return;
    DrawEntity(healthshot, XORSTR("Healthshot"), Settings::ESP::DangerZone::healthshotColor.Color());
}

static void DrawLootCrate(C_BaseEntity *crate, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(crate, localplayer))
		return;
    studiohdr_t* crateModel = modelInfo->GetStudioModel(crate->GetModel());
    if (!crateModel)
        return;
    std::string mdlName = crateModel->name;
    mdlName = mdlName.substr(mdlName.find_last_of('/') + 1);
    std::string crateName;
    if (mdlName.find(XORSTR("case_pistol")) != mdlName.npos)
        crateName = XORSTR("Pistol Case");
    else if (mdlName.find(XORSTR("light_weapon")) != mdlName.npos)
        crateName = XORSTR("SMG Case");
    else if (mdlName.find(XORSTR("heavy_weapon")) != mdlName.npos)
        crateName = XORSTR("Heavy Case");
    else if (mdlName.find(XORSTR("explosive")) != mdlName.npos)
        crateName = XORSTR("Explosive Case");
    else if (mdlName.find(XORSTR("tools")) != mdlName.npos)
        crateName = XORSTR("Tools Case");
    else if (mdlName.find(XORSTR("dufflebag")) != mdlName.npos)
        crateName = XORSTR("Duffle Bag");
	else if (mdlName.find(XORSTR("random")) != mdlName.npos)
        crateName = XORSTR("Airdrop");

		crateName += XORSTR(" | ");
		crateName += std::to_string(*(int*)((uintptr_t)crate + offsets.DT_PhysPropLootCrate.m_iHealth));
		crateName += XORSTR(" HP");

    DrawEntity(crate, crateName.c_str(), Settings::ESP::DangerZone::lootcrateColor.Color());
}

static void DrawMelee(C_BaseCombatWeapon *weapon, C_BasePlayer* localplayer)
{
	if (!weapon)
		return;

	if (!dzShouldDraw(weapon, localplayer))
		return;

    std::string modelName = Util::Items::GetItemDisplayName(*weapon->GetItemDefinitionIndex());
    DrawEntity(weapon, modelName.c_str(), Settings::ESP::DangerZone::meleeColor.Color());
}

static void DrawDZItems(C_BaseEntity *item, C_BasePlayer* localplayer)
{
	if (!dzShouldDraw(item, localplayer))
		return;

    studiohdr_t* itemModel = modelInfo->GetStudioModel(item->GetModel());

    if (!itemModel)
        return;

    std::string mdlName = itemModel->name;
    mdlName = mdlName.substr(mdlName.find_last_of('/') + 1);
    std::string itemName;

	if (mdlName.find(XORSTR("dz_armor_helmet")) != mdlName.npos) // upgrade_...
		itemName = XORSTR("Full Armor");
	else if (mdlName.find(XORSTR("dz_armor")) != mdlName.npos)
		itemName = XORSTR("Armor");
	else if (mdlName.find(XORSTR("dz_helmet")) != mdlName.npos)
		itemName = XORSTR("Helmet");
	else if (mdlName.find(XORSTR("upgrade_tablet_drone")) != mdlName.npos)
		itemName = XORSTR("Tablet (drone)");
	else if (mdlName.find(XORSTR("upgrade_tablet_zone")) != mdlName.npos)
		itemName = XORSTR("Tablet (zone)");
	else if (mdlName.find(XORSTR("upgrade_tablet_hires")) != mdlName.npos)
		itemName = XORSTR("Tablet (highres)");
	else if (mdlName.find(XORSTR("briefcase")) != mdlName.npos)
		itemName = XORSTR("Briefcase");
	else if (mdlName.find(XORSTR("parachutepack")) != mdlName.npos)
		itemName = XORSTR("Parachute");
	else if (mdlName.find(XORSTR("exojump")) != mdlName.npos) // TODO: not working.
		itemName = XORSTR("Exojump");
	else
		itemName = mdlName;

    DrawEntity(item, itemName.c_str(), Settings::ESP::DangerZone::upgradeColor.Color());
}

static void DrawThrowable(C_BaseEntity* throwable, ClientClass* client, C_BasePlayer* localplayer)
{
	// Draw Distance only in DangerZone
	if (Util::IsDangerZone() && !dzShouldDraw(throwable, localplayer))
		return;

	model_t* nadeModel = throwable->GetModel();

	if (!nadeModel)
		return;

	studiohdr_t* hdr = modelInfo->GetStudioModel(nadeModel);

	if (!hdr)
		return;

	if (!strstr(hdr->name, XORSTR("thrown")) && !strstr(hdr->name, XORSTR("dropped")))
		return;

	ImColor nadeColor = ImColor(255, 255, 255, 255);
	std::string nadeName = XORSTR("Unknown Grenade");
	int nadetype = -1;
	IMaterial* mats[32];
	modelInfo->GetModelMaterials(nadeModel, hdr->numtextures, mats);

	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		if (strstr(mat->GetName(), XORSTR("flashbang")))
		{
			nadeName = XORSTR("Flashbang");
			nadeColor = Settings::ESP::flashbangColor.Color();
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("m67_grenade")) || strstr(mat->GetName(), XORSTR("hegrenade")))
		{
			nadeName = XORSTR("HE Grenade");
			nadeColor = Settings::ESP::grenadeColor.Color();
			nadetype = 3;
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("smoke")))
		{
			nadeName = XORSTR("Smoke");
			nadeColor = Settings::ESP::smokeColor.Color();
			nadetype = 1;
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("decoy")))
		{
			nadeName = XORSTR("Decoy");
			nadeColor = Settings::ESP::decoyColor.Color();
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("incendiary")) || strstr(mat->GetName(), XORSTR("molotov")))
		{
			nadeName = XORSTR("Molotov");
			nadeColor = Settings::ESP::molotovColor.Color();
			nadetype = 2;
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("bump_mine")))
		{
			nadeName = XORSTR("Bump Mine (placed)");
			nadeColor = Settings::ESP::mineColor.Color();
			break;
		}
		else if (strstr(mat->GetName(), XORSTR("c4"))) // breach charge
		{
			nadeName = XORSTR("Breach Charge (placed)");
			nadeColor = Settings::ESP::chargeColor.Color();
			break;
		}
	}

	DrawEntity(throwable, nadeName.c_str(), nadeColor, nadetype);
}

static void DrawGlow()
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	for (int i = 0; i < glowManager->m_GlowObjectDefinitions.Count(); i++)
	{
		GlowObjectDefinition_t& glow_object = glowManager->m_GlowObjectDefinitions[i];

		if (glow_object.IsUnused() || !glow_object.m_pEntity)
			continue;

		ImColor color;
		ClientClass* client = glow_object.m_pEntity->GetClientClass();
		bool shouldGlow = true;

		if (client->m_ClassID == EClassIds::CCSPlayer)
		{
			C_BasePlayer* player = (C_BasePlayer*) glow_object.m_pEntity;

			if (player->GetDormant() || !player->GetAlive())
				continue;

			if (player == localplayer)
			{
				color = Settings::ESP::Glow::localplayerColor.Color(player);
			}
			else
			{
				if (!Entity::IsTeamMate(player, localplayer))
				{
					if (Entity::IsVisible(player, CONST_BONE_HEAD))
						color = Settings::ESP::Glow::enemyVisibleColor.Color(player);
					else
						color = Settings::ESP::Glow::enemyColor.Color(player);
				}
				else
					color = Settings::ESP::Glow::allyColor.Color(player);
			}
		}
		else if (client->m_ClassID != EClassIds::CBaseWeaponWorldModel &&
				 (strstr(client->m_pNetworkName, XORSTR("Weapon")) || client->m_ClassID == EClassIds::CDEagle || client->m_ClassID == EClassIds::CAK47))
		{
			color = Settings::ESP::Glow::weaponColor.Color();
		}
		else if (client->m_ClassID == EClassIds::CBaseCSGrenadeProjectile || client->m_ClassID == EClassIds::CDecoyProjectile ||
				 client->m_ClassID == EClassIds::CMolotovProjectile || client->m_ClassID == EClassIds::CSmokeGrenadeProjectile)
		{
			color = Settings::ESP::Glow::grenadeColor.Color();
		}
		else if (client->m_ClassID == EClassIds::CBaseAnimating)
		{
			color = Settings::ESP::Glow::defuserColor.Color();

			if (localplayer->HasDefuser() || localplayer->GetTeam() == TeamID::TEAM_TERRORIST)
				shouldGlow = false;
		}
		else if (client->m_ClassID == EClassIds::CChicken)
		{
			color = Settings::ESP::Glow::chickenColor.Color();

			*reinterpret_cast<C_Chicken*>(glow_object.m_pEntity)->GetShouldGlow() = shouldGlow;
		}

		shouldGlow = shouldGlow && color.Value.w > 0;

		glow_object.m_flGlowColor[0] = color.Value.x;
		glow_object.m_flGlowColor[1] = color.Value.y;
		glow_object.m_flGlowColor[2] = color.Value.z;
		glow_object.m_flGlowAlpha = shouldGlow ? color.Value.w : 1.0f;
		glow_object.m_flBloomAmount = 1.0f;
		glow_object.m_bRenderWhenOccluded = shouldGlow;
		glow_object.m_bRenderWhenUnoccluded = true;
	}
}

static void DrawFOVCrosshair()
{
	if (!Settings::ESP::FOVCrosshair::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer->GetAlive())
		return;

	if (Settings::Legitbot::AutoAim::fov > OverrideView::currentFOV)
		return;

	float radius;
	if (Settings::Legitbot::AutoAim::realDistance)
	{
		Vector src3D, dst3D, forward;
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		QAngle angles = viewanglesBackup;
		Math::AngleVectors(angles, forward);
		filter.pSkip = localplayer;
		src3D = localplayer->GetEyePosition();
		dst3D = src3D + (forward * 8192);

		ray.Init(src3D, dst3D);
		trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		QAngle leftViewAngles = QAngle(angles.x, angles.y - 90.f, 0.f);
		Math::NormalizeAngles(leftViewAngles);
		Math::AngleVectors(leftViewAngles, forward);
		forward *= Settings::Legitbot::AutoAim::fov * 5.f;

		Vector maxAimAt = tr.endpos + forward;

		Vector max2D;
		if (debugOverlay->ScreenPosition(maxAimAt, max2D))
			return;

		radius = fabsf(Paint::engineWidth / 2 - max2D.x);
	}
	else
		radius = ((Settings::Legitbot::AutoAim::fov / OverrideView::currentFOV) * Paint::engineWidth) / 2;

	radius = std::min(radius, (((180.f / OverrideView::currentFOV) * Paint::engineWidth) / 2)); // prevents a big radius (CTD).

	if (Settings::ESP::FOVCrosshair::filled)
		Draw::AddCircleFilled(Paint::engineWidth / 2, Paint::engineHeight / 2 , radius, Settings::ESP::FOVCrosshair::color.Color(), std::max(12, (int)radius*2));
	else
		Draw::AddCircle(Paint::engineWidth / 2, Paint::engineHeight / 2, radius, Settings::ESP::FOVCrosshair::color.Color(), std::max(12, (int)radius*2));
}

static void DrawSpread()
{
    if ( !Settings::ESP::Spread::enabled && !Settings::ESP::Spread::spreadLimit )
        return;

    C_BasePlayer* localplayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
    if ( !localplayer )
        return;

    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle(
            localplayer->GetActiveWeapon() );
    if ( !activeWeapon )
        return;

    if ( Settings::ESP::Spread::enabled ) {
        float cone = activeWeapon->GetSpread() + activeWeapon->GetInaccuracy();
        if ( cone > 0.0f ) {
            float radius = ( cone * Paint::engineHeight ) / 1.5f;
            Draw::AddRect( ( ( Paint::engineWidth / 2 ) - radius ), ( Paint::engineHeight / 2 ) - radius + 1,
                               ( Paint::engineWidth / 2 ) + radius + 1, ( Paint::engineHeight / 2 ) + radius + 2,
                               Settings::ESP::Spread::color.Color() );
        }
    }
    if ( Settings::ESP::Spread::spreadLimit ) {
        float cone = Settings::Legitbot::ShootAssist::Hitchance::value;
        if ( cone > 0.0f ) {
            float radius = ( cone * Paint::engineHeight ) / 1.5f;
            Draw::AddRect( ( ( Paint::engineWidth / 2 ) - radius ), ( Paint::engineHeight / 2 ) - radius + 1,
                               ( Paint::engineWidth / 2 ) + radius + 1, ( Paint::engineHeight / 2 ) + radius + 2 ,
                               Settings::ESP::Spread::spreadLimitColor.Color() );
        }
    }
}

static void DrawScope()
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return;

    if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SG556 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AUG)
        return;

    Draw::AddLine(0, Paint::engineHeight * 0.5, Paint::engineWidth, Paint::engineHeight * 0.5, ImColor(0, 0, 0, 255));
    Draw::AddLine(Paint::engineWidth * 0.5, 0, Paint::engineWidth * 0.5, Paint::engineHeight, ImColor(0, 0, 0, 255));
}

bool ESP::PrePaintTraverse(VPANEL vgui_panel, bool force_repaint, bool allow_force)
{
	if (Settings::ESP::enabled && Settings::NoScopeBorder::enabled && strcmp("HudZoom", panel->GetName(vgui_panel)) == 0)
		return false;

      // for (int i = 1; i <= entityList->GetHighestEntityIndex(); i++)  //-V807
      //  {
    //            auto e = entityList->GetClientEntity(i);
  //              auto client_class = e->GetClientClass();
//		if (!client_class)
//			continue;
    //           switch (client_class->m_ClassID)
      //          {
    //           case EClassIds::CEnvTonemapController:
        //                world_modulation(e);
  //                      break;
//		}


//	}

	return true;
}

void ESP::Paint()
{
	if (!Settings::ESP::enabled && !inputSystem->IsButtonDown(Settings::ESP::key))
		return;

	if (!engine->IsInGame())
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	for (int i = 1; i < entityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity* entity = entityList->GetClientEntity(i);
		if (!entity)
			continue;

		ClientClass* client = entity->GetClientClass();
               if (client->m_ClassID == EClassIds::CInferno)
                {
			drawfire(entity);
		}
		if (client->m_ClassID == EClassIds::CCSPlayer)
		{
			C_BasePlayer* player = (C_BasePlayer*) entity;

			if (player->GetDormant() &&  !Settings::ESP::showDormant)
				continue;
			  if (!player->GetAlive())
				continue;

			DrawPlayer(player);
		}
		if ((client->m_ClassID != EClassIds::CBaseWeaponWorldModel && (strstr(client->m_pNetworkName, XORSTR("Weapon")) || client->m_ClassID == EClassIds::CDEagle || client->m_ClassID == EClassIds::CAK47 || client->m_ClassID == EClassIds::CBreachCharge || client->m_ClassID == EClassIds::CBumpMine)) && client->m_ClassID != EClassIds::CPhysPropWeaponUpgrade && Settings::ESP::Filters::weapons)
		{
			DrawDroppedWeapons((C_BaseCombatWeapon*) entity, localplayer);
		}
		else if (client->m_ClassID == EClassIds::CC4 && Settings::ESP::Filters::bomb)
		{
			DrawBomb((C_BaseCombatWeapon*) entity, localplayer);
		}
		else if (client->m_ClassID == EClassIds::CPlantedC4 && Settings::ESP::Filters::bomb)
		{
			DrawPlantedBomb((C_PlantedC4*) entity, localplayer);
		}
		else if (client->m_ClassID == EClassIds::CHostage && Settings::ESP::Filters::hostages)
		{
			DrawHostage(entity, localplayer);
		}
		else if (client->m_ClassID == EClassIds::CBaseAnimating && Settings::ESP::Filters::defusers)
		{
			DrawDefuseKit(entity, localplayer);
		}
		else if (client->m_ClassID == EClassIds::CChicken && Settings::ESP::Filters::chickens)
		{
			DrawChicken(entity);
		}
		else if (client->m_ClassID == EClassIds::CFish && Settings::ESP::Filters::fishes)
		{
			DrawFish(entity);
		}
		else if (Settings::ESP::Filters::throwables && strstr(client->m_pNetworkName, XORSTR("Projectile")))
		{
			DrawThrowable(entity, client, localplayer);
		}
		else if (Util::IsDangerZone())
		{
			if (Settings::ESP::DangerZone::safe && client->m_ClassID == EClassIds::CBRC4Target)
				DrawSafe(entity, localplayer);

			else if (Settings::ESP::DangerZone::dronegun && client->m_ClassID == EClassIds::CDronegun)
				DrawSentryTurret(entity, localplayer);

			else if (Settings::ESP::DangerZone::ammobox && client->m_ClassID == EClassIds::CPhysPropAmmoBox)
				DrawAmmoBox(entity, localplayer);

			else if (Settings::ESP::DangerZone::radarjammer && client->m_ClassID == EClassIds::CPhysPropRadarJammer)
				DrawRadarJammer(entity, localplayer);

			else if (Settings::ESP::DangerZone::barrel && client->m_ClassID == EClassIds::CPhysicsProp)
				DrawExplosiveBarrel(entity, localplayer);

			else if (Settings::ESP::DangerZone::drone && client->m_ClassID == EClassIds::CDrone)
				DrawDrone(entity, localplayer);

			else if (Settings::ESP::DangerZone::cash && client->m_ClassID == EClassIds::CItemCash)
				DrawCash(entity, localplayer);

			else if (Settings::ESP::DangerZone::tablet && client->m_ClassID == EClassIds::CTablet)
				DrawTablet(entity, localplayer);

			else if (Settings::ESP::DangerZone::healthshot && client->m_ClassID == EClassIds::CItem_Healthshot)
				DrawHealthshot(entity, localplayer);

			else if (Settings::ESP::DangerZone::lootcrate && client->m_ClassID == EClassIds::CPhysPropLootCrate)
				DrawLootCrate(entity, localplayer);

			else if (Settings::ESP::DangerZone::melee && (client->m_ClassID == EClassIds::CMelee || client->m_ClassID == EClassIds::CKnife))
			{
				C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*) entity;
				DrawMelee(weapon, localplayer);
			}

			else if (Settings::ESP::DangerZone::upgrade && client->m_ClassID == EClassIds::CPhysPropWeaponUpgrade)
				DrawDZItems(entity, localplayer);
		}
	}
	Drawlc();
        DrawFH(Settings::ESP::keybi::x, Settings::ESP::keybi::y + 85);
	DrawLag(Settings::ESP::keybi::x, Settings::ESP::keybi::y + 90, localplayer);
	if (Settings::ESP::VelGraph)
		DrawVelGraph();
	if (Settings::ESP::KeyBinds)
        	DrawKeyBinds(Settings::ESP::keybi::x, Settings::ESP::keybi::y);
	if (Settings::ESP::FOVCrosshair::enabled)
		DrawFOVCrosshair();
	if (Settings::ESP::Spread::enabled || Settings::ESP::Spread::spreadLimit)
		DrawSpread();
	if (Settings::NoScopeBorder::enabled && localplayer->IsScoped())
		DrawScope();

		DrawWatermark(localplayer);
		CustomFog();
if (Settings::ThirdPerson::toggled)
{
DrawAATrace(AntiAim::fakeAngle, AntiAim::realAngle);
                renderRange();

}
 if (Settings::AntiAim::ManualAntiAim::Enable || Settings::AntiAim::LegitAntiAim::enable)
    {

		DrawManualAntiaim();
}
DrawIndicators();
}
void ESP::FireGameEvent(IGameEvent* event)
{
        if(!event)      
	return;
DrawImpacts(event);
DrawBulletTracers(event);
}
void ESP::DrawModelExecute()
{
	if (!Settings::ESP::enabled)
		return;

	if (!engine->IsInGame())
		return;

	if (Settings::ESP::Glow::enabled)
		DrawGlow();
}

void ESP::CreateMove(CUserCmd* cmd)
{
	viewanglesBackup = cmd->viewangles;
    if( Settings::ESP::enabled){
        CheckActiveSounds();
    }

}

void ESP::PaintToUpdateMatrix( ) {
	if( !engine->IsInGame() )
		return;

	vMatrix = engine->WorldToScreenMatrix();
}
