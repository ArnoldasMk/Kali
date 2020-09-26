#include "hitmarkers3D.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../fonts.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Hooks/hooks.h"
#include "esp.h"
// int - damage dealt, long - timestamp
std::vector<std::pair<int, long>> damages3;
long lastHitmarkerTimestamp3 = 0;
int hurt_player_id;
int hurti_player_id;
float x;
float y;
float z;
void Hitmarkers3D::Paint( ) {
        if ( !Settings::ESP::enabled || !Settings::ESP::Hitmarker::enabled )
                return;

        if ( !engine->IsInGame() )
                return;

        C_BasePlayer* localplayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
        if ( !localplayer )
                return;

        if ( !localplayer->GetAlive() )
                return;

        int duration = Settings::ESP::Hitmarker::duration;
        long now = Util::GetEpochTime();

        long diff = lastHitmarkerTimestamp3 + duration - now;
        if ( diff <= 0 )
                return;

        ImColor color =Settings::ESP::Hitmarker::color.Color();
        float sc = 1.0f/255.0f;
        color.Value.w = std::min( color.Value.w, (( diff * (color.Value.w / sc ) / duration * 2 ) ) * sc);

        int sides[4][2] = {
                        { -1, -1 },
                        { 1,  1 },
                        { -1, 1 },
                        { 1,  -1 }
        };
                Vector pos3D = Vector(x, y, z);
		ImVec2* pos2D;
	if (hurt_player_id != hurti_player_id)
		return;

        ESP::WorldToScreen(pos3D, pos2D);
        for ( auto& it : sides )
                Draw::AddLine( pos2D->x+ ( Settings::ESP::Hitmarker::innerGap * it[0] ),
                                           pos2D->x + ( Settings::ESP::Hitmarker::innerGap * it[1] ),
                                           pos2D->x + ( Settings::ESP::Hitmarker::size * it[0] ),
                                           pos2D->x + ( Settings::ESP::Hitmarker::size * it[1] ), color );

        if ( !Settings::ESP::Hitmarker::Damage::enabled )
                return;

        int textHeight = (int)Draw::GetTextSize( XORSTR( "[cool]" ), esp_font ).y;

        for ( unsigned int i = 0; i < damages3.size(); i++ ) {
                long timestamp = damages3[i].second;
                long hitDiff = timestamp + duration - now;

                if ( hitDiff <= 0 ) {
                        damages3.erase( damages3.begin() + i );
                        continue;
                }

                int damage = damages3[i].first;

                std::string damageStr = '-' + std::to_string( damage );

                color.Value.w = Settings::ESP::Hitmarker::color.Color().Value.w;
                color.Value.w = std::min( color.Value.w, (( hitDiff * ( color.Value.w / sc ) / duration * 2 ) *sc ));

                Draw::AddText( pos2D->x + Settings::ESP::Hitmarker::size + 4,
                                           pos2D->x - Settings::ESP::Hitmarker::size - textHeight * i + 4, damageStr.c_str(), color);

        }
}

void Hitmarkers3D::FireGameEvent(IGameEvent* event)
{
        if (!Settings::ESP::Hitmarker::enabled)
                return;

        if (!engine->IsInGame())
                return;

        if (strcmp(event->GetName(), XORSTR("player_hurt")) != 0)
{
        hurt_player_id = event->GetInt(XORSTR("userid"));
        int attacker_id = event->GetInt(XORSTR("attacker"));

        if (engine->GetPlayerForUserID(hurt_player_id) == engine->GetLocalPlayer())
                return;

        if (engine->GetPlayerForUserID(attacker_id) != engine->GetLocalPlayer())
                return;

        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer)
                return;

        C_BasePlayer* hurt_player = (C_BasePlayer*) entityList->GetClientEntity(engine->GetPlayerForUserID(hurt_player_id));
        if (!hurt_player)
                return;

        if (Entity::IsTeamMate(hurt_player, localplayer) && !Settings::ESP::Hitmarker::allies)
                return;

        if (!Entity::IsTeamMate(hurt_player, localplayer) && !Settings::ESP::Hitmarker::enemies)
                return;
}
        if (strcmp(event->GetName(), XORSTR("bullet_impact")) != 0)
{
        hurti_player_id = event->GetInt(XORSTR("userid"));
	x = event->GetFloat(XORSTR("x"));
        y = event->GetFloat(XORSTR("y"));
        z = event->GetFloat(XORSTR("z"));

}
        long now = Util::GetEpochTime();
        lastHitmarkerTimestamp3 = now;
        damages3.insert(damages3.begin(), std::pair<int, long>(event->GetInt(XORSTR("dmg_health")), now));
}
