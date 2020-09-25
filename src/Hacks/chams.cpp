#include "chams.h"
#include "thirdperson.h"
#include "antiaim.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "lagcomp.h"
IMaterial *materialChamsFlat, *materialChamsFlatIgnorez;
IMaterial *WhiteAdditive,*WhiteAdditiveIgnoreZ;
IMaterial *AdditiveTwo, *AdditiveTwoIgnoreZ;
IMaterial* materialChamsPearl;
IMaterial* materialChamsGlow;
                Vector colro = Vector(0, 0, 1);

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

static void DrawPlayer(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	using namespace Settings::ESP;
	if ( !FilterEnemy::Chams::enabled && !FilterLocalPlayer::RealChams::enabled && !FilterAlise::Chams::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer)
		return;
	
	ChamsType chamsType = ChamsType::NONE;
	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive())
		return;

	if ( Entity::IsTeamMate(entity,localplayer) && Settings::ESP::FilterAlise::Chams::enabled && entity != localplayer)
		chamsType = Settings::ESP::FilterAlise::Chams::type;
	else if (!Entity::IsTeamMate(entity, localplayer) && FilterEnemy::Chams::enabled)
		chamsType = Settings::ESP::FilterEnemy::Chams::type;
	else if ( entity == localplayer && FilterLocalPlayer::RealChams::enabled)
		chamsType = FilterLocalPlayer::RealChams::type;
	else 
		return;

	IMaterial *visible_material = nullptr;
	IMaterial *hidden_material = nullptr;
	IMaterial *overlay_material = nullptr;
	bool wap;

	switch (chamsType)
	{
		case ChamsType::NONE :
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			visible_material = WhiteAdditive;
			hidden_material = materialChamsFlatIgnorez;
			wap = false;
			break;
		case ChamsType::FLAT:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			wap = false;
			break;
		case ChamsType::ADDITIVETWO:
			visible_material = AdditiveTwo;
			hidden_material = AdditiveTwoIgnoreZ;
			wap = false;
			break;
		case ChamsType::PEARL:
        		visible_material = materialChamsPearl;
			hidden_material = materialChamsPearl;
			wap = false;
			break;
		case ChamsType::GLOW:
			visible_material = materialChamsFlat;
                        hidden_material = materialChamsFlatIgnorez;
			overlay_material = materialChamsGlow;
			wap = true;
			break;
		default :
			return;
	}

	visible_material->AlphaModulate(1.f);
	hidden_material->AlphaModulate(1.f);

	if (entity == localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
		Color color = visColor;
		color *= 0.45f;
		
		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
	}
	else if (Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::allyVisibleColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::allyColor.Color(entity).Value.w);
		colro.x = Settings::ESP::Chams::allyColor.Color(entity).Value.x;
		colro.y = Settings::ESP::Chams::allyColor.Color(entity).Value.y;
                colro.z = Settings::ESP::Chams::allyColor.Color(entity).Value.z;

	}
	else if (!Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::enemyVisibleColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::enemyColor.Color(entity).Value.w);
	}

	if (entity->GetImmune())
	{
		visible_material->AlphaModulate(0.5f);
		hidden_material->AlphaModulate(0.5f);
	}

	visible_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chamsType == ChamsType::WIREFRAME);
	visible_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, chamsType == ChamsType::NONE);
	hidden_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chamsType == ChamsType::WIREFRAME);
	hidden_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, chamsType == ChamsType::NONE);

	if (!Settings::ESP::Filters::legit && !Settings::ESP::Filters::visibilityCheck && !Entity::IsVisible(entity, CONST_BONE_HEAD, 180.f, true))
	{
		modelRender->ForcedMaterialOverride(hidden_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
		modelRender->ForcedMaterialOverride(nullptr);
	}

	modelRender->ForcedMaterialOverride(visible_material);
		if (wap){
                modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);

	        modelRender->ForcedMaterialOverride(overlay_material);
}
}

static void DrawFake(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{

	if (!Settings::ESP::FilterLocalPlayer::Chams::enabled || !Settings::ThirdPerson::toggled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;
	
	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive()
		|| entity != localplayer)
		return;

	IMaterial* Fake_meterial = nullptr;
        IMaterial *faoverlay_material = nullptr;

	switch (Settings::ESP::FilterLocalPlayer::Chams::type)
	{
		case ChamsType::NONE:
		case ChamsType::WIREFRAME:
		case ChamsType::WHITEADDTIVE:
			Fake_meterial = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			Fake_meterial = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			Fake_meterial = AdditiveTwo;
			break;
                case ChamsType::PEARL:
                        Fake_meterial = materialChamsPearl;
	      		break;
		case ChamsType::GLOWF:
                        Fake_meterial = materialChamsGlow;
			break;
		default:
			return;
	}


	Fake_meterial->AlphaModulate(1.f);
		/*
		* Testing for chams in fake angle 
		* Hope for best
		*/

	Color fake_color = Color::FromImColor(Settings::ESP::Chams::FakeColor.Color(entity));
	Color color = fake_color;
	color *= 0.45f;

	Fake_meterial->ColorModulate(fake_color);
	Fake_meterial->AlphaModulate(Settings::ESP::Chams::FakeColor.Color(entity).Value.w);

	static matrix3x4_t fakeBoneMatrix[128];
	float fakeangle = AntiAim::fakeAngle.y - AntiAim::realAngle.y;
        float fakeanglex = 0;

	static Vector OutPos;
	for (int i = 0; i < 128; i++)
	{
		Math::AngleMatrix(Vector(fakeanglex, fakeangle, 0), fakeBoneMatrix[i]);
		matrix::MatrixMultiply(fakeBoneMatrix[i], pCustomBoneToWorld[i]);
		Vector BonePos = Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3], pCustomBoneToWorld[i][2][3]) - pInfo.origin;
		Math::VectorRotate(BonePos, Vector(fakeanglex , fakeangle, 0), OutPos);
		OutPos += pInfo.origin;
                        fakeBoneMatrix[i][0][3] = OutPos.x;
                        fakeBoneMatrix[i][1][3] = OutPos.y;
                        fakeBoneMatrix[i][2][3] = OutPos.z;
	}

	if (entity->GetImmune())
	{
		Fake_meterial->AlphaModulate(0.5f);
	}
	//entity->SetupBones
	modelRender->ForcedMaterialOverride(Fake_meterial);
	modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, fakeBoneMatrix);
                if (Settings::ESP::FilterLocalPlayer::Chams::type == ChamsType::GLOW){
                modelRender->ForcedMaterialOverride(faoverlay_material);
}else {
                modelRender->ForcedMaterialOverride(nullptr);
}

	// End of chams for fake angle
}

static void DrawWeapon(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Weapon::type)
	{
		case ChamsType::NONE:
		case ChamsType::WIREFRAME:
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			mat = AdditiveTwo;
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Weapon::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}
static void DrawRecord(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
  //  if (!Settings::LagComp::enabled)
 //       return;
  //if (!Settings::ESP::drawback)
    //    return;

    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer)
        return;
if (!localplayer->GetAlive())
return;
        Color lag_color = Color::FromImColor(Settings::ESP::Chams::Arms::color.Color());

    IMaterial *material = materialChamsFlat;
    Color color = lag_color;

    material->ColorModulate(color);
    material->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

    for (auto &frame : LagComp::lagCompTicks)
    {
        for (auto &ticks : frame.records)
        {
            if (pInfo.entity_index < engine->GetMaxClients() && entityList->GetClientEntity(pInfo.entity_index) == ticks.entity)
            {
                auto tick_difference = (globalVars->tickcount - frame.tickCount);
                if (tick_difference <= 1) continue;

                material->ColorModulate(color);
                material->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

                modelRender->ForcedMaterialOverride(materialChamsFlat);
                modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, (matrix3x4_t *)ticks.bone_matrix);
                modelRender->ForcedMaterialOverride(nullptr);
            }
        }
    }
}

static void DrawArms(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Arms::type)
	{
		case ChamsType::NONE:
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			mat = AdditiveTwo;
			break;
                case ChamsType::PEARL:
                        mat = materialChamsPearl;

		default :
			return;
	}
	

	if (!Settings::ESP::Chams::Arms::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

void Chams::DrawModelExecute(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!engine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	if (!pInfo.pModel)
		return;

	static bool materialsCreated = false;
	if (!materialsCreated)
	{
	        materialChamsPearl = Util::CreateMaterial2(XORSTR("VertexLitGeneric"), XORSTR("models/inventory_items/dogtags/dogtags_outline"), false, true, true, true, 1.f);
                materialChamsGlow = Util::CreateMaterial3(XORSTR("VertexLitGeneric"), XORSTR("csgo/materials/glowOverlay.vmt"), false, true, true, true, 1.f, colro);

		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsFlatIgnorez = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
	
		WhiteAdditive = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false, true, true, true);
		WhiteAdditiveIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true, false, true, true, true);
		
		AdditiveTwo = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false, true, true, true, "models/effects/cube_white", "[1 1 1]", 1, "[0 1 1]" );
		AdditiveTwoIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false, true, true, true, "models/effects/cube_white", "[1 1 1]", 1, "[0 1 1]" );
		materialsCreated = true;
		
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);

	if (modelName.find(XORSTR("models/player")) != std::string::npos)
	{
		DrawFake(thisptr, context, state, pInfo, pCustomBoneToWorld);
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
                 DrawRecord(thisptr, context, state, pInfo, pCustomBoneToWorld);
		
	}
		
	else if (modelName.find(XORSTR("arms")) != std::string::npos)
		DrawArms(pInfo);
	else if (modelName.find(XORSTR("weapon")) != std::string::npos)
		DrawWeapon(pInfo);
}
