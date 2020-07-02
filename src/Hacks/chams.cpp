#include "chams.h"
#include "thirdperson.h"
#include "antiaim.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"

IMaterial* materialChamsFlat;
IMaterial* materialChamsFlatIgnorez;
IMaterial *WhiteAdditive,*WhiteAdditiveIgnoreZ,
			 *PredictionGlass, *PredictionGlassIgnoreZ,
			 *FbiGlass, *CrystalClear, *GibGlass, *DogClass,
			 *achivements, *achivementsIgnoreZ;
IMaterial* materialChamsWeapons;

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

static void DrawPlayer(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	using namespace Settings::ESP;
	if ( !Settings::ESP::Chams::enabled && !FilterEnemy::Chams::enabled && !FilterLocalPlayer::Chams::enabled)
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

	else if ( entity == localplayer && FilterLocalPlayer::Chams::enabled)
		chamsType = FilterLocalPlayer::Chams::type;

	IMaterial *visible_material = nullptr;
	IMaterial *hidden_material = nullptr;
	

	switch (chamsType)
	{
		case ChamsType::WHITE_ADDTIVE :
			visible_material = WhiteAdditive;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::WIREFRAME :
			visible_material = WhiteAdditive;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::PREDICTION_GLASS :
			visible_material = PredictionGlass;
			hidden_material = PredictionGlassIgnoreZ;
			break;
		case ChamsType::FBI_GLASS :
			visible_material = FbiGlass;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::CRYSTAL_CLEAR :
			visible_material = CrystalClear;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::GIB_GLASS :
			visible_material = GibGlass;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::Dog_Class :
			visible_material = DogClass;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::CHAMS_FLAT:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::Achivements :
			visible_material = achivements;
			hidden_material = achivementsIgnoreZ;
			break;
		case ChamsType::NONE :
			return;
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
	else if (Entity::IsTeamMate(entity, localplayer) && entity != localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}
	else if (!Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}

	if (entity->GetImmune())
	{
		visible_material->AlphaModulate(0.5f);
		hidden_material->AlphaModulate(0.5f);
	}

	if (!Settings::ESP::Filters::legit && !Settings::ESP::Filters::visibilityCheck && !Entity::IsVisible(entity, CONST_BONE_HEAD, 180.f, true))
	{
		modelRender->ForcedMaterialOverride(hidden_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}

	modelRender->ForcedMaterialOverride(visible_material);
	
}

static void DrawFake(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{

	if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LegitAntiAim::enable && Settings::ThirdPerson::toggled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;
	
	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive())
		return;

	IMaterial* Fake_meterial = nullptr;
	
	Fake_meterial = DogClass;
	Fake_meterial->AlphaModulate(1.f);

	if (entity == localplayer)
	{
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
		static Vector OutPos;
		for (int i = 0; i < 128; i++)
		{
			Math::AngleMatrix(Vector(0, fakeangle, 0), fakeBoneMatrix[i]);
			matrix::MatrixMultiply(fakeBoneMatrix[i], pCustomBoneToWorld[i]);
			Vector BonePos = Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3], pCustomBoneToWorld[i][2][3]) - pInfo.origin;
			Math::VectorRotate(BonePos, Vector(0, fakeangle, 0), OutPos);
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
		//modelRenderVMT->ApplyVMT();
		modelRender->ForcedMaterialOverride(nullptr);
		// End of chams for fake angle
	
	}
	else
	{
		return;
	}

	
}

static void DrawWeapon(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Weapon::type)
	{
		case WeaponType::WHITE_ADDTIVE :
			// Default for arms
			mat = WhiteAdditive;
			break;
		case WeaponType::PREDICTION_GLASS :
			mat = PredictionGlass;
			break;
		case WeaponType::FBI_GLASS :
			mat = FbiGlass;
			break;
		case WeaponType::CRYSTAL_CLEAR :
			mat = CrystalClear;
			break;
		case WeaponType::GIB_GLASS :
			mat = GibGlass;
			break;
		case WeaponType::Dog_Class :
			mat = DogClass;
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Weapon::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == WeaponType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == WeaponType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawArms(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Arms::type)
	{
		case ArmsType::WIREFRAME :
		case ArmsType::WHITE_ADDTIVE :
			// Default for arms
			mat = WhiteAdditive;
			break;
		case ArmsType::PREDICTION_GLASS :
			mat = PredictionGlass;
			break;
		case ArmsType::FBI_GLASS :
			mat = FbiGlass;
			break;
		case ArmsType::CRYSTAL_CLEAR :
			mat = CrystalClear;
			break;
		case ArmsType::GIB_GLASS :
			mat = GibGlass;
			break;
		case ArmsType::Dog_Class :
			mat = DogClass;
			break;
		default :
			break;
	}
	

	if (!Settings::ESP::Chams::Arms::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ArmsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ArmsType::NONE);
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
	
		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsFlatIgnorez = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		
		WhiteAdditive = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false, true, true, true);;
		WhiteAdditiveIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		
		//Materials
		PredictionGlass = material->FindMaterial("csgo/materials/glowOverlay", TEXTURE_GROUP_VGUI);
		PredictionGlassIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/achievements/glow"), true, true, true, true, true);
		// PredictionGlass = material->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_VGUI);
		FbiGlass = material->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_VGUI);
		CrystalClear = material->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_VGUI);
		GibGlass = material->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_VGUI);
		DogClass = material->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_VGUI);
		materialChamsWeapons = material->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_VGUI);
		
		achivements = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/achievements/glow"), false, false, true, true, true);
		achivementsIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/achievements/glow"), true, true, true, true, true);
		materialsCreated = true;
		
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);

	if (modelName.find(XORSTR("models/player")) != std::string::npos)
	{
		DrawFake(thisptr, context, state, pInfo, pCustomBoneToWorld);
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
		
	}
		
	else if (modelName.find(XORSTR("arms")) != std::string::npos)
		DrawArms(pInfo);
	else if (modelName.find(XORSTR("weapon")) != std::string::npos)
		DrawWeapon(pInfo);
}
