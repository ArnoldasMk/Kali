#include "ragebot.h"
#include "autowall.h"

#include "../Utils/bonemaps.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"

#include <future>

#define absolute(x) ( x = x < 0 ? x * -1 : x)
std::vector<int64_t> Ragebot::friends = {};
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone

inline bool RagebotShouldAim = false, 
			EnemyPresent = false,
			doubleFire = false,
			deathDamage = false;

inline int prevWeaponIndex = NULL,
			DeathBoneIndex = NULL;

const int MultiVectors = 7, HeadMultiVectors = 11;

static float prevSpotDamage = NULL;

static Vector DoubleTapSpot = Vector{NULL,NULL,NULL};

struct Enemy
{
	C_BasePlayer* player = nullptr;
	int Index = 0;
	int size = 0;
};


/* Fills points Vector. True if successful. False if not.  Credits for Original method - ReactiioN */
static bool HeadMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{

	model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)Hitbox::HITBOX_HEAD, 0);
    
	if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;

	points[HeadMultiVectors] = (center,center,center,center,center,center,center, center, center, center, center);
	/* // OLD CODE
    for (int i = 0; i < headVectors; i++) // set all points initially to center mass of head.
	points[i] = center;
	*/
    points[0].y += bbox->radius * 0.80f;
	points[1].y += bbox->radius * 0.80f;
	points[2].z += bbox->radius * 1.25f;
	points[3].x += bbox->radius * 0.80f;
	points[3].z += bbox->radius * 0.60f;
	points[4].x -= bbox->radius * 0.80f;
	points[4].z += bbox->radius * 0.90f;
	points[5].y += bbox->radius * 0.80f;
	points[5].z += bbox->radius * 0.90f;
	points[6].y -= bbox->radius * 0.80f;
	points[6].z += bbox->radius * 0.90f;
	points[7].x += bbox->radius * 0.80f;
	points[8].x -= bbox->radius * 0.80f;
	points[9].z += bbox->radius * 0.60f;
	points[10].y -= bbox->radius * 0.80f;;

    return true;
}

static bool UpperChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{

	model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* upChst = modelInfo->GetStudioModel(pModel);
    if (!upChst)
		return false;
    
	mstudiobbox_t* bbox = upChst->pHitbox((int)CONST_BONE_UPPER_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;

	points[MultiVectors] = (center, center, center, center, center, center,center);
    
    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}
 
static bool ChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{

    model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)CONST_BONE_MIDDLE_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;
	/*
	* To redunce time we directly implement the values rather than ... using for loop
	*/
	points[MultiVectors] = (center,center,center,center,center,center,center);

    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}

static bool LowerChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{
    model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)CONST_BONE_LOWER_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;

	/*
	* To redunce time we directly implement the values rather than ... using for loop
	*/
	points[MultiVectors] = (center,center,center,center,center,center,center);
    
    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}

/*
** Method for safety damage prediction where 
** It will just look for required Damage Not for the best damage
*/
static void safetyPrediction(C_BasePlayer* player, Vector *wallbangspot, float* wallbangdamage, Vector *visibleSpot, float* VisibleDamage, int *i, float *playerHelth)
{
	// cvar->ConsoleDPrintf("In Safety Prediction Function\n");
	static float minDamage = Settings::Ragebot::AutoWall::value;
    static float minDamageVisible = Settings::Ragebot::visibleDamage;
    const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);	
	
	// static int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);

    float FOV = Settings::Ragebot::AutoAim::fov;

	matrix3x4_t boneMatrix[128];

	if ( !player->SetupBones(boneMatrix, 128, 0x100, 0) )
		return;


	if (!Settings::Ragebot::AutoAim::desiredBones[*i])
	    return;

	int boneID = (*modelType).at(*i);

	if (boneID == BONE_INVALID) // bone not available on this modeltype.
	   	return;

	bool VisiblityCheck = Entity::IsVisible(player, boneID, FOV, false);
		

	// If we found head here
	if (boneID == CONST_BONE_HEAD) // head multipoint
	{
	    Vector headPoints[HeadMultiVectors];
	    if (!HeadMultiPoint(player, headPoints, boneMatrix))
			return;

	    // cvar->ConsoleDPrintf(XORSTR("Found the head\n"));
	    for (int j = 0; j < HeadMultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(headPoints[j], true, data);
			// cvar->ConsoleDPrintf(XORSTR("Head Spot Damage : %d\n"), spotDamage);	
			
			if (spotDamage <= 0.f )
				continue;

			if (spotDamage < minDamage && spotDamage < minDamageVisible)
				continue;

			if ( !EnemyPresent)
		    	EnemyPresent = true;

			if (VisiblityCheck)
			{	
				if (spotDamage >= *playerHelth + 9 )
				{
					*VisibleDamage = spotDamage;
		    		*visibleSpot = headPoints[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*VisibleDamage = spotDamage;
		    	*visibleSpot = headPoints[j];
			}
			else 
			{
				if ( spotDamage >= *playerHelth + 9 )
				{
					*wallbangdamage = spotDamage;
		    		*wallbangspot = headPoints[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*wallbangdamage = spotDamage;
		    	*wallbangspot = headPoints[j];
			}		
	    }
	}
	
	else if (boneID == CONST_BONE_UPPER_SPINE) // upper chest MultiPoint
	{
	    Vector upperChest[MultiVectors];
	    if (!UpperChestMultiPoint(player, upperChest, boneMatrix))
			return;

		//cvar->ConsoleDPrintf(XORSTR("Found the upperchest\n"));
	    for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(upperChest[j], true, data);

			if (spotDamage <= 0.f )
				continue;

			if (spotDamage < minDamage && spotDamage < minDamageVisible)
				continue;

			if ( !EnemyPresent)
		    	EnemyPresent = true;
				
			if (VisiblityCheck)
			{	
				if (spotDamage >= *playerHelth + 9 )
				{
					*VisibleDamage = spotDamage;
		    		*visibleSpot = upperChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*VisibleDamage = spotDamage;
		    	*visibleSpot = upperChest[j];
			}
			else 
			{
				if ( spotDamage >= *playerHelth + 9 )
				{
					*wallbangdamage = spotDamage;
		    		*wallbangspot = upperChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*wallbangdamage = spotDamage;
		    	*wallbangspot = upperChest[j];
			}
			
	    }
	}
	
	else if (boneID == CONST_BONE_MIDDLE_SPINE) // Chest Multipoint
	{
	    Vector MiddleChest[MultiVectors];
	    if (!ChestMultiPoint(player, MiddleChest, boneMatrix))
			return;

	    //cvar->ConsoleDPrintf(XORSTR("Found the middle chest\n"));
	    for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(MiddleChest[j], true, data);

			if (spotDamage <= 0.f )
				continue;

			if (spotDamage < minDamage && spotDamage < minDamageVisible)
				continue;
			
			if ( !EnemyPresent)
		    	EnemyPresent = true;

			if (VisiblityCheck)
			{	
				if (spotDamage >= *playerHelth + 9)
				{
					*VisibleDamage = spotDamage;
		    		*visibleSpot = MiddleChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*VisibleDamage = spotDamage;
		    	*visibleSpot = MiddleChest[j];
			}
			else 
			{
				if ( spotDamage >= *playerHelth + 9)
				{
					*wallbangdamage = spotDamage;
		    		*wallbangspot = MiddleChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*wallbangdamage = spotDamage;
		    	*wallbangspot = MiddleChest[j];
			}
	    }
	}
	
	else if (boneID == CONST_BONE_LOWER_SPINE) // Lower multipoint
	{
	    Vector LowerChest[MultiVectors];
	    if (!LowerChestMultiPoint(player, LowerChest, boneMatrix))
			return;

	   //cvar->ConsoleDPrintf(XORSTR("Found the lowerChest\n"));
	    for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(LowerChest[j], true, data);


			if (spotDamage <= 0.f )
				continue;

			if (spotDamage < minDamage && spotDamage < minDamageVisible)
				continue;

			if ( !EnemyPresent)
		    	EnemyPresent = true;

			if (VisiblityCheck)
			{	
				if (spotDamage >= *playerHelth + 9 )
				{
					*VisibleDamage = spotDamage;
		    		*visibleSpot = LowerChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*VisibleDamage = spotDamage;
		    	*visibleSpot = LowerChest[j];
			}
			else 
			{
				if ( spotDamage >= *playerHelth + 9 )
				{
					*wallbangdamage = spotDamage;
		    		*wallbangspot = LowerChest[j];
					deathDamage = true;
					DeathBoneIndex = *i;
					return;
				}
		    	*wallbangdamage = spotDamage;
		    	*wallbangspot = LowerChest[j];
			}
			
	    }
	}

	else {
		Vector bone3D = player->GetBonePosition(boneID);

		//cvar->ConsoleDPrintf(XORSTR("bone ID : %d \n"), boneID);
		Autowall::FireBulletData data;
		float spotDamage = Autowall::GetDamage(bone3D, true, data);

		if (spotDamage <= 0.f )
			return;

		if (spotDamage < minDamage && spotDamage < minDamageVisible)
			return;

		if ( !EnemyPresent)
			EnemyPresent = true;

		if (VisiblityCheck)
		{	
			if (spotDamage >= *playerHelth  + 9 )
			{
				*VisibleDamage = spotDamage;
		    	*visibleSpot = bone3D;
				deathDamage = true;
				DeathBoneIndex = *i;
				return;
			}
		    *VisibleDamage = spotDamage;
		    *visibleSpot = bone3D;
		}
		else 
		{
			if ( spotDamage >= *playerHelth + 9 )
			{
				*wallbangdamage = spotDamage;
				*wallbangspot = bone3D;
				deathDamage = true;
				DeathBoneIndex = *i;
				return;
			}
		   	*wallbangdamage = spotDamage;
		   	*wallbangspot = bone3D;
		}
    }
	
	return;
}

/*
** Method to calculate the best damge
** To kill enemy instantly
*/
static void BestDamagePrediction(C_BasePlayer* player, Vector *wallbangspot, float* wallbangdamage, Vector *visibleSpot, float* VisibleDamage, int i)
{

	static float minDamage = Settings::Ragebot::AutoWall::value;
    static float minDamageVisible = Settings::Ragebot::visibleDamage;
    const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);	
	
    float FOV = Settings::Ragebot::AutoAim::fov;

	float playerHelth = player->GetHealth();

	matrix3x4_t boneMatrix[256];

	if (!player->SetupBones(boneMatrix, 256, 0x100, 0.f))
		return;

	if (!Settings::Ragebot::AutoAim::desiredBones[i])
	   	return;

	int boneID = (*modelType).at(i);

	if (boneID == BONE_INVALID) // bone not available on this modeltype.
	    return;

	bool VisiblityCheck = Entity::IsVisible(player, boneID, FOV, false);
		

	// If we found head here
	if (boneID == BONE_HEAD) // head multipoint
	{
	    Vector headPoints[MultiVectors];
	    if (!HeadMultiPoint(player, headPoints, boneMatrix))
			return;

	    // cheaking for all head vectors
	    for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(headPoints[j], !Settings::Ragebot::friendly, data);

			if (spotDamage <= 0.f )
				continue;

			if ( (spotDamage < minDamage && spotDamage < minDamageVisible) )
				continue;

			if ( !EnemyPresent ) 
		    	EnemyPresent = true;

			if ( VisiblityCheck ) // cheking if the enemy  is visible
			{
				if (spotDamage >= playerHelth + 9)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = headPoints[j];
		    		prevSpotDamage = 0.f;
		    		return;
				}
				else if (spotDamage >= minDamageVisible && spotDamage > prevSpotDamage)
				{
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = headPoints[j];
					return;
				}
			}
			else
			{
				if (spotDamage >= playerHelth + 9)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*wallbangspot = headPoints[j];
		    		*wallbangdamage = spotDamage;
		    		prevSpotDamage = 0;
		    		return;
				}
				else if (spotDamage >= minDamage)
				{
		    		*wallbangdamage = spotDamage;
		    		*wallbangspot = headPoints[j];
					return;
				}
			}
	    }
	}
	
	else if (boneID == BONE_UPPER_SPINAL_COLUMN) // head multipoint
	{
	    Vector upperChest[MultiVectors];
	    if (!UpperChestMultiPoint(player, upperChest, boneMatrix))
		return;

	    // cheaking for all head vectors
	    for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(upperChest[j], !Settings::Ragebot::friendly, data);

			if (spotDamage <= 0.f )
				continue;

			if ( spotDamage < minDamage && spotDamage < minDamageVisible )
				continue;

			if ( !EnemyPresent ) 
		    	EnemyPresent = true;

			if ( VisiblityCheck ) // cheking if the enemy  is visible
			{
				if (spotDamage >= playerHelth)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = upperChest[j];
		    		prevSpotDamage = 0;
		    		return;
				}
				if ( spotDamage >= minDamageVisible )
				{
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = upperChest[j];
					return;
				}
			}
			else
			{
				if (spotDamage >= playerHelth)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*wallbangspot = upperChest[j];
		    		*wallbangdamage = spotDamage;
		    		prevSpotDamage = 0;
		    		return;
				}
				if (spotDamage >= minDamage && spotDamage > prevSpotDamage)
				{
		    		*wallbangdamage = spotDamage;
		    		*wallbangspot = upperChest[j];
					return;
				}
			}
	    }
	}
	
	else if (boneID == BONE_MIDDLE_SPINAL_COLUMN) // head multipoint
	{
	    Vector MiddleChest[MultiVectors];
	    if (!ChestMultiPoint(player, MiddleChest, boneMatrix))
			return;

		for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(MiddleChest[j], !Settings::Ragebot::friendly, data);

			if (spotDamage <= 0.f )
				continue;

			if (spotDamage < minDamage && spotDamage < minDamageVisible)
			 	continue;

			if ( !EnemyPresent ) 
		    	EnemyPresent = true;

			if ( VisiblityCheck ) // cheking if the enemy  is visible
			{
				if (spotDamage >= playerHelth)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = MiddleChest[j];
		    		prevSpotDamage = 0;
		    		return;
				}
				if (spotDamage >= minDamageVisible && spotDamage > prevSpotDamage)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = MiddleChest[j];
					return;
				}
			}
			if (!VisiblityCheck)
			{
				if (spotDamage >= playerHelth)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*wallbangspot = MiddleChest[j];
		    		*wallbangdamage = spotDamage;
		    		prevSpotDamage = 0;
		    		return;
				}
				if (spotDamage >= minDamage && spotDamage > prevSpotDamage)
				{
		    		*wallbangdamage = spotDamage;
		    		*wallbangspot = MiddleChest[j];
					return;
				}
			}
	    }
	}
	
	else if (boneID == BONE_UPPER_SPINAL_COLUMN) // head multipoint
	{
	    Vector LowerChest[MultiVectors];
	    if (!LowerChestMultiPoint(player, LowerChest, boneMatrix))
			return;
	    
		for (int j = 0; j < MultiVectors; j++)
	    {
			Autowall::FireBulletData data;
			float spotDamage = Autowall::GetDamage(LowerChest[j], !Settings::Ragebot::friendly, data);

			if (spotDamage <= 0.f)
				continue;

			if ( spotDamage < minDamage && spotDamage < minDamageVisible)
				continue;

			if ( !EnemyPresent ) 
		    	EnemyPresent = true;

			if ( VisiblityCheck ) // cheking if the enemy  is visible
			{
				if (spotDamage >= playerHelth + 9 )
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = LowerChest[j];
		    		prevSpotDamage = 0;
		    		return;
				}
				if (spotDamage >= minDamageVisible && spotDamage > prevSpotDamage)
				{
					deathDamage = true;
					DeathBoneIndex = i;
		    		*VisibleDamage = spotDamage;
		    		*visibleSpot = LowerChest[j];
					return;
				}
			}
			else
			{
				if (spotDamage >= playerHelth + 9 )
				{
		    		*wallbangspot = LowerChest[j];
		    		*wallbangdamage = spotDamage;
		    		prevSpotDamage = 0;
		    		return;
				}
				if ( spotDamage >= minDamage )
				{
		    		*wallbangdamage = spotDamage;
		    		*wallbangspot = LowerChest[j];
					return;
				}
			}
	    }
	}

	else {	
		Vector bone3D = player->GetBonePosition(boneID);

		Autowall::FireBulletData data;
		float boneDamage = Autowall::GetDamage(bone3D, !Settings::Ragebot::friendly, data);
		
		if (boneDamage <= 0.f )
				return;

		if ( boneDamage < minDamage && boneDamage < minDamageVisible )
			return;

		if ( !EnemyPresent ) 
		    	EnemyPresent = true;

		if (VisiblityCheck)
		{
			if (boneDamage >= playerHelth + 9)
			{
				deathDamage = true;
				DeathBoneIndex = i;
				*visibleSpot = bone3D;
				*VisibleDamage = boneDamage;
				prevSpotDamage = 0.f;
				return;
			}
			if (boneDamage > prevSpotDamage && boneDamage >= minDamageVisible)
	    	{
				*visibleSpot = bone3D;
				*VisibleDamage = boneDamage;
				return;
	    	}
		}
		else
		{
			if (boneDamage >= playerHelth + 9)
			{
				deathDamage = true;
				DeathBoneIndex = i;
				*wallbangspot = bone3D;
				*wallbangdamage = boneDamage;
				prevSpotDamage = 0.f;
				return;
			}
			if (boneDamage >= minDamage )
			{
	    		*wallbangspot = bone3D;
	    		*wallbangdamage = boneDamage;
				return;
			}
		}
		
    }

}


/*
** Get best Damage from the enemy and the spot
*/
static void GetBestSpotAndDamage(C_BasePlayer* player, Vector& wallBangSpot, float& wallBangDamage, Vector& visibleSpot, float& visibleDamage)
{
	
	int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);
	//cvar->ConsoleDPrintf(XORSTR("lenth : %d"), len);
	Vector WallBangSpot[len],
			VisibleSpot[len];

	float WallBangDamage[len],
			VisibleDamage[len];

	float playerHelth = player->GetHealth();

	// For safety mesurements
	if (Settings::Ragebot::damagePrediction == DamagePrediction::safety)
	{	
		// cvar->ConsoleDPrintf(XORSTR("doing safety damage\n"));
		for (int i = 0; i < len; i++)
		{
				WallBangDamage[i] = VisibleDamage[i] = 0;
				WallBangSpot[i] = VisibleSpot[i] = Vector{0,0,0};
				auto temp = std::async(std::launch::async, safetyPrediction, player, &WallBangSpot[i], &WallBangDamage[i], &VisibleSpot[i], &VisibleDamage[i], &i, &playerHelth);
				
		}

		for (int i = 0; i < len; i++)
		{
			if (deathDamage)
			{
				deathDamage = false;
				visibleDamage = VisibleDamage[DeathBoneIndex];
				visibleSpot = VisibleSpot[DeathBoneIndex];
				wallBangSpot = WallBangSpot[DeathBoneIndex];
				wallBangDamage = WallBangDamage[DeathBoneIndex];
				DeathBoneIndex = NULL;
				return;
			}

			if ( VisibleDamage[i] <= 0.f && WallBangDamage[i] <= 0.f)
				continue;

			if ( VisibleDamage[i] >= playerHelth)
			{
				visibleDamage = VisibleDamage[i];
				visibleSpot = VisibleSpot[i];
				prevSpotDamage = 0.f;
				return;
			}
				
			else if (WallBangDamage[i] >= playerHelth)
			{
				wallBangDamage = WallBangDamage[i];
				wallBangSpot = WallBangSpot[i];
				prevSpotDamage = 0.f;
				return;
			}
				
			if (VisibleDamage[i] >= playerHelth / 2)
			{
				visibleSpot = VisibleSpot[i];
				visibleDamage = VisibleDamage[i];
				doubleFire = true;
				prevSpotDamage = 0.f;
				return;
			}
			else if ( WallBangDamage[i] >= playerHelth / 2)
			{
				wallBangDamage = WallBangDamage[i];
				wallBangSpot = WallBangSpot[i];
				doubleFire = true;
				prevSpotDamage = 0.f;
				return;
			}
			else
			{
				if (VisibleDamage[i] >= WallBangDamage[i] )
				{
					visibleSpot = VisibleSpot[i];
					visibleDamage = VisibleDamage[i];
					return;
				}
						
				else if ( WallBangDamage[i] > VisibleDamage[i] )
				{
					wallBangSpot = WallBangSpot[i];
					wallBangDamage = WallBangDamage[i];
					return;
				}
						
			}
		}
	}
	
	if (Settings::Ragebot::damagePrediction == DamagePrediction::damage)
	{
		for (int i = 0; i < len; i++)
		{
				WallBangDamage[i] = VisibleDamage[i] = 0;
				WallBangSpot[i] = VisibleSpot[i] = Vector{0,0,0};
				auto temp = std::async(std::launch::async, BestDamagePrediction, player, &WallBangSpot[i], &WallBangDamage[i], &VisibleSpot[i], &VisibleDamage[i], i);
		}


		for (int i = 0; i < len; i++)
		{
			if (deathDamage)
			{
				deathDamage = false;
				visibleDamage = VisibleDamage[DeathBoneIndex];
				visibleSpot = VisibleSpot[DeathBoneIndex];
				wallBangSpot = WallBangSpot[DeathBoneIndex];
				wallBangDamage = WallBangDamage[DeathBoneIndex];
				DeathBoneIndex = NULL;
				return;
			}

			if ( VisibleDamage[i] <= 0.f && WallBangDamage[i] <= 0.f)
				continue;
				
			if (VisibleDamage[i] >= playerHelth / 2)
			{
				visibleSpot = VisibleSpot[i];
				visibleDamage = VisibleDamage[i];
				doubleFire = true;
				prevSpotDamage = 0.f;
				return;
			}
			else if ( WallBangDamage[i] >= playerHelth / 2)
			{
				wallBangDamage = WallBangDamage[i];
				wallBangSpot = WallBangSpot[i];
				doubleFire = true;
				prevSpotDamage = 0.f;
				return;
			}
			else
			{
				if (VisibleDamage[i] >= WallBangDamage[i] >= prevSpotDamage )
				{
					visibleSpot = VisibleSpot[i];
					prevSpotDamage = visibleDamage = VisibleDamage[i];
				}
						
				else if ( WallBangDamage[i] > VisibleDamage[i] >= prevSpotDamage)
				{
					wallBangSpot = WallBangSpot[i];
					prevSpotDamage = wallBangDamage = WallBangDamage[i];
				}
						
			}
		}
	}
	
	prevSpotDamage = 0.f;
	return;
}


static Vector VelocityExtrapolate(C_BasePlayer* player, Vector aimPos)
{
    return aimPos + (player->GetVelocity() * globalVars->interval_per_tick);
}


/*
* To find the closesnt enemy to reduce the calculation time and increase perdormace

/* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) */
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, CUserCmd* cmd)
{
	C_BasePlayer* closenstEntity = nullptr;
	
	float prevDistance = 0.f,
	  		cbFov = 0.f;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	for (int i = 1; i < engine->GetMaxClients(); ++i)
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
		
		cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));
		Vector enemyPos = player->GetAbsOrigin();
		QAngle viewAngles = cmd->viewangles;
		
		//float distance = Math::CalMaxDistance(viewAngles, enemyPos);

		if (prevDistance == 0 && cbFov != 0)
		{
			// prevViewOffset = viewDirection;
			prevDistance = cbFov;
			closenstEntity = player;
		}
		else if ( cbFov < prevDistance /*&& view > prevViewAngle &&*/ /*viewDirection < prevViewOffset*/)
		{
			prevDistance = cbFov;
			closenstEntity = player;
		}
	}
	return closenstEntity;
}

// get the clossest player from crosshair
static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, Vector* bestSpot, float* bestDamage)
{
	if ( !( Settings::Ragebot::enemySelectionType == EnemySelectionType::CLosestToCrosshair ) ) 
		return nullptr;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BasePlayer* closestEntity = nullptr;

	if (!localplayer->GetAlive())
	{
		return nullptr;
	}
    if ( doubleFire && Settings::Ragebot::DoubleFire)
	{
		*bestSpot = DoubleTapSpot;
		DoubleTapSpot = Vector{NULL,NULL,NULL};
		doubleFire = false;
		return localplayer;
	}
	else
	{
		doubleFire = false;
		*bestSpot = Vector{NULL, NULL, NULL};
		DoubleTapSpot = Vector{NULL,NULL,NULL};
	}

	Vector wallBangSpot,
	       	VisibleSpot;
	float WallBangdamage, 
			VisibleDamage;
			
			wallBangSpot = Vector{ NULL, NULL, NULL },
	       	VisibleSpot = Vector{ NULL, NULL, NULL };
			WallBangdamage = NULL, 
			VisibleDamage = NULL;

		C_BasePlayer* player = GetClosestEnemy(localplayer, cmd);
	
		if ( player == nullptr )
		{
			cvar->ConsoleDPrintf(XORSTR("returning null mean no enemy \n"));
			return player;
		}

		GetBestSpotAndDamage(player, wallBangSpot, WallBangdamage, VisibleSpot, VisibleDamage);

		C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

		float playerHelth = player->GetHealth();

		if (Settings::Ragebot::damagePrediction == DamagePrediction::safety)
		{
			if (VisibleDamage > 0.f && VisibleDamage >= WallBangdamage)
			{
				if (VisibleDamage >= playerHelth + 9)
				{
					*bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if ( VisibleDamage >= playerHelth/2 )
				{
					doubleFire = true;
					*bestSpot = VisibleSpot;
					*bestDamage = VisibleDamage;
					DoubleTapSpot = VisibleSpot;
					prevSpotDamage = 0.f;
					return player;
				}

				*bestDamage = VisibleDamage;
				*bestSpot = VisibleSpot;
				closestEntity = player;
				lastRayEnd = VisibleSpot;
				return closestEntity;
			}
			else if ( WallBangdamage > 0.f && WallBangdamage > VisibleDamage)
			{
				if (WallBangdamage >= playerHelth)
				{
					*bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if ( WallBangdamage >= playerHelth/2 )
				{
					doubleFire = true;
					*bestSpot = wallBangSpot;
					*bestDamage = WallBangdamage;
					DoubleTapSpot = VisibleSpot;
					prevSpotDamage = 0.f;
					return player;
				}
				*bestDamage = WallBangdamage;
		    	*bestSpot = wallBangSpot;
		    	closestEntity = player;
		    	lastRayEnd = wallBangSpot;
				return closestEntity;	
			}
			else
			{
				return nullptr;
			}		
		}
		
		else if ( Settings::Ragebot::damagePrediction == DamagePrediction::damage) // if the damage prediction is best damage
		{
			if ( VisibleDamage >= WallBangdamage && VisibleDamage > 0.f)
			{
				if (VisibleDamage >= playerHelth + 9)
				{
					*bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if (VisibleDamage >= prevSpotDamage)
				{
					prevSpotDamage = *bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
				}
			}
			else if ( WallBangdamage >= VisibleDamage &&  WallBangdamage > 0.f)
			{
				if (WallBangdamage >= playerHelth + 9)
				{
					*bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if (VisibleDamage >= prevSpotDamage)
				{
					prevSpotDamage = *bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
				}
			
			}
		}
	
	prevSpotDamage  = 0.f;
    return closestEntity;
}

// Get the best damage and the player 
static C_BasePlayer* GetBestEnemyAndSpot(CUserCmd* cmd, Vector* bestSpot, float* bestDamage)
{
	if ( !(Settings::Ragebot::enemySelectionType == EnemySelectionType::BestDamage) )
		return nullptr;

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BasePlayer* closestEntity = nullptr;

	if (!localplayer->GetAlive())
	{
		return nullptr;
	}
    if ( doubleFire && Settings::Ragebot::DoubleFire)
	{
		*bestSpot = DoubleTapSpot;
		DoubleTapSpot = Vector{NULL,NULL,NULL};
		doubleFire = false;
		return localplayer;
	}
	else
	{
		doubleFire = false;
		*bestSpot = Vector{NULL, NULL, NULL};
		DoubleTapSpot = Vector{NULL,NULL,NULL};
	}

	Vector wallBangSpot,
	       	VisibleSpot;
	float WallBangdamage, 
			VisibleDamage;
			
			wallBangSpot = Vector{ NULL, NULL, NULL },
	       	VisibleSpot = Vector{ NULL, NULL, NULL };
			WallBangdamage = NULL, 
			VisibleDamage = NULL;

	cvar->ConsoleDPrintf(XORSTR("in best damage function"));
	for (int i = 1; i < engine->GetMaxClients(); ++i)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player
			|| player == localplayer
			|| player->GetDormant()
			|| !player->GetAlive()
			|| player->GetImmune())
			continue;

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
			continue;

		if( !Ragebot::friends.empty() ) // check for friends, if any
		{
			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (std::find(Ragebot::friends.begin(), Ragebot::friends.end(), entityInformation.xuid) != Ragebot::friends.end())
				continue;

		}
			GetBestSpotAndDamage(player, wallBangSpot, WallBangdamage, VisibleSpot, VisibleDamage);

		C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

		float playerHelth = player->GetHealth();

		if (Settings::Ragebot::damagePrediction == DamagePrediction::safety)
		{
			if (VisibleDamage > 0.f && VisibleDamage >= WallBangdamage)
			{
				if (  VisibleDamage >= playerHelth + 9)
				{
					*bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if ( VisibleDamage >= playerHelth/2 )
				{
					doubleFire = true;
					*bestSpot = VisibleSpot;
					*bestDamage = VisibleDamage;
					DoubleTapSpot = VisibleSpot;
					prevSpotDamage = 0.f;
					return player;
				}

				*bestDamage = VisibleDamage;
				*bestSpot = VisibleSpot;
				closestEntity = player;
				lastRayEnd = VisibleSpot;
				return closestEntity;
			}
			else if ( WallBangdamage > 0.f && WallBangdamage > VisibleDamage)
			{
				if (WallBangdamage >= playerHelth)
				{
					*bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if ( WallBangdamage >= playerHelth/2 )
				{
					doubleFire = true;
					*bestSpot = wallBangSpot;
					*bestDamage = WallBangdamage;
					DoubleTapSpot = VisibleSpot;
					prevSpotDamage = 0.f;
					return player;
				}
				*bestDamage = WallBangdamage;
		    	*bestSpot = wallBangSpot;
		    	closestEntity = player;
		    	lastRayEnd = wallBangSpot;
				return closestEntity;	
			}		
		}
		
		else if ( Settings::Ragebot::damagePrediction == DamagePrediction::damage) // if the damage prediction is best damage
		{
			if ( VisibleDamage > 0.f &&  VisibleDamage >= WallBangdamage)
			{
				if (VisibleDamage >= playerHelth + 9)
				{
					*bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if (VisibleDamage >= prevSpotDamage)
				{
					prevSpotDamage = *bestDamage = VisibleDamage;
					*bestSpot = VisibleSpot;
					closestEntity = player;
					lastRayEnd = VisibleSpot;
				}
			}
			else if ( WallBangdamage > 0.f && WallBangdamage > VisibleDamage)
			{
				if (WallBangdamage >= playerHelth + 9)
				{
					*bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
					prevSpotDamage = 0.f;
					return closestEntity;
				}
				else if (VisibleDamage >= prevSpotDamage)
				{
					prevSpotDamage = *bestDamage = WallBangdamage;
		    		*bestSpot = wallBangSpot;
		    		closestEntity = player;
		    		lastRayEnd = wallBangSpot;
				}
			
			}
		}
	
	}

	prevSpotDamage = 0.f;
	return closestEntity;
}

//Hitchance source from nanoscence
static bool Ragebothitchance(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
    float hitchance = 101;

    if (activeWeapon)
    {
		activeWeapon->UpdateAccuracyPenalty();
		float inaccuracy = activeWeapon->GetInaccuracy();
		float weaponspread = activeWeapon->GetSpread();

		if (inaccuracy == 0)
	    	inaccuracy = 0.0000001;

		hitchance = 1 / (inaccuracy);

		if (Settings::Ragebot::HitChanceOverwrride::enable)
		{
	    	return (hitchance >= Settings::Ragebot::HitChance::value * Settings::Ragebot::HitChanceOverwrride::value);
		}

		return hitchance >= Settings::Ragebot::HitChance::value * 1.2f;
    }
	return true;
}

static void RagebotRCS(QAngle& angle, C_BasePlayer* player, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{

    if (!(cmd->buttons & IN_ATTACK))
	return;

    static QAngle RagebotRCSLastPunch = { 0, 0, 0 };
    bool hasTarget = RagebotShouldAim && player;

    float aimpunch = cvar->FindVar("weapon_recoil_scale")->GetFloat();
    QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

    if (Settings::Ragebot::silent || hasTarget)
    {
	angle.x -= CurrentPunch.x * 2.0f;
	angle.y -= CurrentPunch.y * 2.0f;
    }
    else if (aimpunch)
    {
	QAngle NewPunch = { CurrentPunch.x - RagebotRCSLastPunch.x, CurrentPunch.y - RagebotRCSLastPunch.y, 0 };

	angle.x -= NewPunch.x * 2.0f;
	angle.y -= NewPunch.y * 2.0f;
    }

    RagebotRCSLastPunch = CurrentPunch;
}

/*
** AutoCroutch is a bad idea in hvh instant death if you miss
static void RagebotAutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
    if (!Settings::Ragebot::AutoCrouch::enabled)
	return;

    if (!player)
	return;

    cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}
*/
static void RagebotAutoSlow(C_BasePlayer* player, float& forward, float& sideMove, float& bestDamage, C_BaseCombatWeapon* active_weapon, CUserCmd* cmd)
{

    if (!Settings::Ragebot::AutoSlow::enabled || !player || !RagebotShouldAim)
    {
		return;
    }

    float nextPrimaryAttack = active_weapon->GetNextPrimaryAttack();

    if (nextPrimaryAttack > globalVars->curtime)
    {
		return;
    }

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon || activeWeapon->GetAmmo() == 0)
	return;

    if (Settings::Ragebot::HitChance::enabled && RagebotShouldAim)
    {
		if (!Ragebothitchance(localplayer, activeWeapon))
		{
	   	 	cmd->buttons |= IN_WALK;
	    	forward = 0;
	    	sideMove = 0;
	    	cmd->upmove = 0;
	    	return;
		}
		else if (Ragebothitchance(localplayer, activeWeapon))
		{
	    	cmd->buttons |= IN_WALK;
		}
		else
		{
	    	return;
		}

	// Experimental items
	/*if (!(HitPercentage(localplayer, activeWeapon)))
		{
			cmd->buttons |= IN_WALK;
			forward = 0;
			sideMove = 0;
			cmd->upmove = 0;
			return;
		}*/
	/*if( !Ragebothitchance(localplayer, activeWeapon) && !(cmd->buttons & IN_WALK))
			{
				cmd->buttons |= IN_WALK;
				forward = -forward;
				sideMove = -sideMove;
				cmd->upmove = 0;
				return;
			}
			else if( Ragebothitchance(localplayer, activeWeapon) && !(cmd->buttons & IN_WALK)) {
				cmd->buttons |= IN_WALK;
				forward = 0;
				sideMove = 0;
				cmd->upmove = 0;
				return;
			}
			else if( !Ragebothitchance(localplayer, activeWeapon) && (cmd->buttons & IN_WALK))
			{
				forward = -forward;
				sideMove = -sideMove;
				cmd->upmove = 0;
				return;
			}
			else if( Ragebothitchance(localplayer, activeWeapon) && (cmd->buttons & IN_WALK))
			{
				forward = 0;
				sideMove = 0;
				cmd->upmove = 0;
				return;
			}
			else
			{
				return;
			}*/

	/*else if (cmd->buttons & IN_ATTACK) 
        {
            cmd->buttons |= IN_WALK;
			return;
        }*/
    }

    else if ((active_weapon->GetSpread() + active_weapon->GetInaccuracy()) > (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f)) // https://youtu.be/ZgjYxBRuagA
    {
		cmd->buttons |= IN_WALK;
		forward = -forward;
		sideMove = -sideMove;
		cmd->upmove = 0;
		return;
    }

}

static void RagebotAutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    // if (!Settings::Ragebot::AutoPistol::enabled)
    // 	return;

    if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
	return;

    if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
	return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
	cmd->buttons &= ~IN_ATTACK;
}

static void AutoCock(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    if (!Settings::Ragebot::AutoShoot::enabled)
	return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
	return;

    if (activeWeapon->GetAmmo() == 0)
	return;
    if (cmd->buttons & IN_USE)
	return;

    cmd->buttons |= IN_ATTACK;
    float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
    if (postponeFireReadyTime > 0)
    {
	if (postponeFireReadyTime < globalVars->curtime)
	{
	    if (player)
	    {
		Ragebot::coacking = false;
		return;
	    }

	    Ragebot::coacking = true;
	    cmd->buttons &= ~IN_ATTACK;
	}
    }
}

static void RagebotAutoShoot(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    //cvar->ConsoleDPrintf("I ma in auto shoot method \n");
    if (!Settings::Ragebot::AutoShoot::enabled)
		return;

    if (!player || activeWeapon->GetAmmo() == 0)
		return;

    //C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

    if (cmd->buttons & IN_USE)
		return;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (Settings::Ragebot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2))
    {
		cmd->buttons |= IN_ATTACK2;
		return; // continue next tick
    }

    if (Settings::Ragebot::HitChance::enabled && !Ragebothitchance(localplayer, activeWeapon))
    {
		return;
    }

	//cvar->ConsoleDPrintf(XORSTR("WE are now applying auto shoot"));
    float nextPrimaryAttack = activeWeapon->GetNextPrimaryAttack();

    if (!(*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
    {
	if (nextPrimaryAttack > globalVars->curtime)
	{
	    cmd->buttons &= ~IN_ATTACK;
	    return;
	}
	else
	{
	    cmd->buttons |= IN_ATTACK;
	    return;
	}
    }
}

static void FixMouseDeltas(CUserCmd* cmd, const QAngle& angle, const QAngle& oldAngle)
{
    if (!RagebotShouldAim)
	return;

    QAngle delta = angle - oldAngle;
    float sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    float m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    float m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    float zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
    cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}

void Ragebot::CreateMove(CUserCmd* cmd)
{

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
    {
		RagebotShouldAim = false;
		return;
    }

    Ragebot::UpdateValues();

    QAngle oldAngle;
    engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

    QAngle angle = cmd->viewangles;
    Vector localEye = localplayer->GetEyePosition();

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon || activeWeapon->GetInReload())
	return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
	return;

    Vector bestSpot;
	float bestDamage;
	bestSpot = { NULL, NULL, NULL };
	bestDamage = NULL;

	C_BasePlayer* player = nullptr;
	if ( Settings::Ragebot::enemySelectionType == EnemySelectionType::CLosestToCrosshair)
    	player = GetClosestPlayerAndSpot(cmd, &bestSpot, &bestDamage);
	else if ( Settings :: Ragebot::enemySelectionType == EnemySelectionType::BestDamage )
		player = GetBestEnemyAndSpot(cmd, &bestSpot, &bestDamage);

    if (player)
    {
		//cvar->ConsoleDPrintf(XORSTR("find a player \n"));
		
		//Auto Scop Controll system to controll auto scoping every time
		if (Settings::Ragebot::ScopeControl::enabled)
		{
	    	//cheking if the weapon scopable and not scop then it will scop and go back to the next tick
	    	if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2))
	    	{
				cmd->buttons |= IN_ATTACK2;
				return; // will go to the next tick
	    	}
		}
		if (Settings::Ragebot::AutoShoot::enabled)
		{
	    	RagebotShouldAim = true;
		}
		else if (cmd->buttons & IN_ATTACK)
		{
	    	RagebotShouldAim = true;
		}

		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.
		if (RagebotShouldAim)
		{
	   		angle = Math::CalcAngle(localEye, bestSpot);
		}
   	}	
    else if (EnemyPresent) // Just Increase the probrability of scoping for faster shooting in some cases
    {
		EnemyPresent = !EnemyPresent;
		if (Settings::Ragebot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2))
		{
	    	cmd->buttons |= IN_ATTACK2;
	    	return;
		}
		Settings::Debug::AutoAim::target = { 0, 0, 0 };
		RagebotShouldAim = false;
    }
    else // No player to Shoot
    {
		Settings::Debug::AutoAim::target = { 0, 0, 0 };
		RagebotShouldAim = false;
		EnemyPresent = false;
    }

    RagebotAutoSlow(player, oldForward, oldSideMove, bestDamage, activeWeapon, cmd);
    RagebotAutoPistol(activeWeapon, cmd);
    RagebotAutoShoot(player, activeWeapon, cmd);
    AutoCock(player, activeWeapon, cmd);
    RagebotRCS(angle, player, cmd, localplayer, activeWeapon);

    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

    FixMouseDeltas(cmd, angle, oldAngle);
    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

    if (!Settings::Ragebot::silent)
		engine->SetViewAngles(cmd->viewangles);
}

void Ragebot::FireGameEvent(IGameEvent* event)
{
    if (!event)
	return;

    if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    return;
	Ragebot::friends.clear();
    }
    if (strcmp(event->GetName(), XORSTR("player_death")) == 0)
    {
	int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
	int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

	if (attacker_id == deadPlayer_id) // suicide
	    return;

	if (attacker_id != engine->GetLocalPlayer())
	    return;

	RagebotkillTimes.push_back(Util::GetEpochTime());
    }
}

void Ragebot::UpdateValues()
{
    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
	return;

    ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end())
	index = *activeWeapon->GetItemDefinitionIndex();

    const RagebotWeapon_t& currentWeaponSetting = Settings::Ragebot::weapons.at(index);

    Settings::Ragebot::silent = currentWeaponSetting.silent;
    Settings::Ragebot::friendly = currentWeaponSetting.friendly;
    Settings::Ragebot::AutoAim::fov = currentWeaponSetting.RagebotautoAimFov;
    Settings::Ragebot::AutoPistol::enabled = currentWeaponSetting.autoPistolEnabled;
    Settings::Ragebot::AutoShoot::enabled = currentWeaponSetting.autoShootEnabled;
    Settings::Ragebot::AutoShoot::autoscope = currentWeaponSetting.autoScopeEnabled;
    Settings::Ragebot::HitChance::enabled = currentWeaponSetting.HitChanceEnabled;
    Settings::Ragebot::HitChance::value = currentWeaponSetting.HitChance;
    Settings::Ragebot::HitChanceOverwrride::enable = currentWeaponSetting.HitChanceOverwrriteEnable;
    Settings::Ragebot::HitChanceOverwrride::value = currentWeaponSetting.HitchanceOverwrriteValue;
    Settings::Ragebot::AutoWall::value = currentWeaponSetting.autoWallValue;
    Settings::Ragebot::visibleDamage = currentWeaponSetting.visibleDamage;
    Settings::Ragebot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
    Settings::Ragebot::ScopeControl::enabled = currentWeaponSetting.scopeControlEnabled;
	Settings::Ragebot::damagePrediction = currentWeaponSetting.DmagePredictionType;
	Settings::Ragebot::enemySelectionType = currentWeaponSetting.enemySelectionType;
	Settings::Ragebot::DoubleFire = currentWeaponSetting.DoubleFire;

    for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
	Settings::Ragebot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];
}
