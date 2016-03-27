/*
Impact Physics
by Spearminty
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include "script.h"
#include "config.h"
#include <string>
#include <ctime>
#include <list>
#include <set>
#include <algorithm> 
#include "keyboard.h"
using namespace std;
#pragma warning(disable : 4244 4305) // double <-> float conversions

static Vector3 RotationToDirection(Vector3 Rotation)
{
	Vector3 v;
	float z = Rotation.z;
	float num = z * 0.0174532924f;
	float x = Rotation.x;
	float num2 = x * 0.0174532924f;
	float num3 = abs((float)cos((double)num2));
	v.x = (float)((double)((float)(-(float)sin((double)num))) * (double)num3);
	v.y = (float)((double)((float)cos((double)num)) * (double)num3);
	v.z = (float)sin((double)num2);
	return v;
}

Vector3 add(Vector3* vectorA, Vector3* vectorB)
{
	Vector3 result;
	result.x = vectorA->x;
	result.y = vectorA->y;
	result.z = vectorA->z;

	result.x += vectorB->x;
	result.y += vectorB->y;
	result.z += vectorB->z;

	return result;
}
Vector3 subtract(Vector3* vectorA, Vector3* vectorB)
{
	Vector3 result;
	result.x = vectorA->x;
	result.y = vectorA->y;
	result.z = vectorA->z;

	result.x -= vectorB->x;
	result.y -= vectorB->y;
	result.z -= vectorB->z;

	return result;
}

Vector3 multiply(Vector3* vector, float x)
{
	Vector3 result;
	result.x = vector->x;
	result.y = vector->y;
	result.z = vector->z;

	result.x *= x;
	result.y *= x;
	result.z *= x;

	return result;
}
list<WeaponStats> statList;
list<Ped> pedList;
Ped currentPed;
static bool AreVectorsEqual(Vector3 vectorA, Vector3 vectorB)
{
	if (vectorA.x == vectorB.x && vectorA.y == vectorB.y && vectorA.z == vectorB.z)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}
void draw_menu_line(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, int font = 0, bool rescaleText = true)
{
	// default values
	int text_col[4] = { 255, 255, 255, 255 },
		rect_col[4] = { 70, 95, 95, 255 };
	float text_scale = 0.35;

	// correcting values for active line
	if (active)
	{
		text_col[0] = 0;
		text_col[1] = 0;
		text_col[2] = 0;

		rect_col[0] = 218;
		rect_col[1] = 242;
		rect_col[2] = 216;

		if (rescaleText) text_scale = 0.40;
	}

	if (title)
	{
		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;

		if (rescaleText) text_scale = 0.50;
		font = 1;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	textLeft += lineLeft;

	float lineWidthScaled = lineWidth / (float)screen_w; // line width
	float lineTopScaled = lineTop / (float)screen_h; // line top offset
	float textLeftScaled = textLeft / (float)screen_w; // text left offset
	float lineHeightScaled = lineHeight / (float)screen_h; // line height

	float lineLeftScaled = lineLeft / (float)screen_w;

	// this is how it's done in original scripts

	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	UI::_DRAW_TEXT(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

}
std::string caption = "1";
std::string caption2 = "2";
std::string caption3 = "3";
std::string caption4 = "4";
std::string caption5 = "5";
std::string caption6 = "6";
std::string caption7 = "7";

float get_distance(Vector3* pointA, Vector3* pointB)
{
	float a_x = pointA->x;
	float a_y = pointA->y;
	float a_z = pointA->z;

	float b_x = pointB->x;
	float b_y = pointB->y;
	float b_z = pointB->z;

	double x_ba = (double)(b_x - a_x);
	double y_ba = (double)(b_y - a_y);
	double z_ba = (double)(b_z - a_z);

	double y_2 = y_ba * y_ba;
	double x_2 = x_ba * x_ba;
	double z_2 = z_ba * z_ba;
	double sum = y_2 + x_2 + z_2;

	return (float)std::sqrt(sum);
}

float get_vector_length(Vector3* vector)
{
	double x = (double)vector->x;
	double y = (double)vector->y;
	double z = (double)vector->z;

	return (float)std::sqrt(x * x + y * y + z * z);
}

inline const char * const BoolToString(bool b)
{
	return b ? "true" : "false";
}

Entity GetTargetedEntity(Player player)
{
	Entity entity;
	if (PLAYER::_GET_AIMED_ENTITY(player, &entity))
	{
		if (!ENTITY::DOES_ENTITY_EXIST(entity))
		{
			return NULL;
		}
		else if (ENTITY::IS_ENTITY_A_PED(entity))
		{
			return entity;
		}
		else if (ENTITY::IS_ENTITY_A_VEHICLE(entity))
		{
			return entity;
		}
		else if (ENTITY::IS_ENTITY_AN_OBJECT(entity))
		{
			return entity;
		}
	}
	return NULL;
}

int checkTick = 0;
Entity toProcess = NULL;
bool needsImpacting = false;

void ForcePed(Ped targetEntity, Vector3 resultForce, Vector3 resultRot, Vector3 damageLoc)
{	
	//modify rotation
	float multiplier = 0.02;

	resultRot = multiply(&resultRot, multiplier);

	caption2 = "Force: " + to_string(resultForce.x) + "__" + to_string(resultForce.y) + "__" + to_string(resultForce.z) + "__ROT: " + to_string(resultRot.x) + "__" + to_string(resultRot.y) + "__" + to_string(resultRot.z);

	ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(targetEntity);
	PED::SET_PED_CAN_RAGDOLL(targetEntity, true);
	ENTITY::APPLY_FORCE_TO_ENTITY(targetEntity, 1, resultForce.x, resultForce.y, resultForce.z, resultRot.x, resultRot.y, resultRot.z, false, false, true, true, false, true);
}

void ForceVeh(Vehicle targetEntity, Vector3 resultForce, Vector3 resultRot, Vector3 damageLoc, float weaponForce, float radius, float damage)
{
	ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(targetEntity);

	//additional bang for helis
	if (VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(targetEntity)))
	{
		VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, 100, 800, 1);
		//apply some extra rotation
		float bang = 1.2;
		float rotAmp = 1.2;
		resultForce= multiply(&resultForce, bang);
		resultRot = multiply(&resultRot, rotAmp);
	}
	Vector3 entityForward = ENTITY::GET_ENTITY_FORWARD_VECTOR(targetEntity);

	//if moving add a flip effect for cars based on velocity
	//always flip in the direction of movement
	float speed = ENTITY::GET_ENTITY_SPEED(targetEntity);
	float sForce = weaponForce * 0.5;
	float sRot = 0.75;

	if (VEHICLE::IS_THIS_MODEL_A_CAR(ENTITY::GET_ENTITY_MODEL(targetEntity)) && VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(targetEntity))
	{
		//less z flip effect if on all wheels
		resultRot.z = resultRot.z * 0.5;

		if (abs(speed) > 14.0)
		{
			//apply extra rotation and minimal force
			//apply some extra force. faster speeds apply more force
			sForce = sForce - abs(((speed / 20.0) * abs(sForce)));
			//modifier for rotation
			sRot = (speed / 25.0);
			if (sRot > 1)
			{
				sRot = 1;
			}

			//get average /midpoint between the forward direction of the entity and the rotation applied from the impact location
			//this will cause a roll halfway between the bullet impact rotational and vehicle forward direction.
			resultRot.x = ((entityForward.x * 0.75) + (resultRot.x * 0.25)) / 2;
			resultRot.y = ((entityForward.y * 0.75) + (resultRot.y * 0.25)) / 2;
			resultRot.z = ((entityForward.z * 0.75) + (resultRot.z * 0.25)) / 2;

			//apply some extra force. faster speeds apply more force
			resultForce.z = resultForce.z + abs(sForce);
			resultRot.x = resultRot.x * 2;
			resultRot.y = resultRot.y;
			resultRot.z = resultRot.z - abs(resultRot.x) - abs(sRot);
		}
		//if moving slowly apply less force
		else if (abs(speed) < 4.0)
		{
			resultForce = multiply(&resultForce, 0.5);
		}
		//if stopped apply much less force
		else if (VEHICLE::IS_VEHICLE_STOPPED(targetEntity))
		{
			resultForce = multiply(&resultForce, 0.3);
		}
	}

	//some calculation, cleanup on the rotation and force
	if (ENTITY::IS_ENTITY_A_VEHICLE(targetEntity))
	{
		//reduce roll effect overall
		resultRot.x = resultRot.z * 0.5;
		resultRot.y = (resultRot.y + resultRot.x) / 2;
	}

	//if the target is the current player vehicle, reduce less force 
	//(otherwise it is difficult to drive under fire and you can easily hit yourself during drivebys)

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::GET_PLAYER_PED(player);
	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed,true))
	{
		//reduce force by 30% (dam
		if (targetEntity == PED::GET_VEHICLE_PED_IS_IN(playerPed, false))
		{
			resultForce = multiply(&resultForce, 0.70);
		}
	}

	caption2 = "Force: " + to_string(resultForce.x) + "__" + to_string(resultForce.y) + "__" + to_string(resultForce.z) + "__ROT: " + to_string(resultRot.x) + "__" + to_string(resultRot.y) + "__" + to_string(resultRot.z);

	ENTITY::APPLY_FORCE_TO_ENTITY(targetEntity, 1, resultForce.x, resultForce.y, resultForce.z, resultRot.x, resultRot.y, resultRot.z, false, false, true, true, false, true);

	//apply destruction damage
	//distribute the damage across the radius
	//engine health is what blows the vehicle up
	//remove most of the impact of our collision to the engine, so it doesnt explode prematurely
	float engineHealthBefore = (VEHICLE::GET_VEHICLE_ENGINE_HEALTH(targetEntity));
	//VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, damage * 0.30, radius, 1);
	//VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, damage * 0.30, (radius * 0.5), 1);
	//VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, damage * 0.40, (radius * 0.1), 1);

	Vector3 damage2 = VEHICLE::GET_VEHICLE_DEFORMATION_AT_POS(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z);

	caption2 = "deformation: " + to_string(damage2.x) + "__" + to_string(damage2.y) + "__" + to_string(damage2.z);


	VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, damage, radius, 1);

	//VEHICLE::SET_VEHICLE_DAMAGE(targetEntity, damageLoc.x, damageLoc.y, damageLoc.z, damage * 0.40, (radius * 0.1), 1);
	float engineHealthAfter = (VEHICLE::GET_VEHICLE_ENGINE_HEALTH(targetEntity));
	//new health is the difference of what we applied minus damage from collision
	if ((engineHealthAfter - engineHealthBefore) > 0)
	{
		float newHealth = engineHealthBefore - ((engineHealthAfter - engineHealthBefore) - (damage * 0.9));
		VEHICLE::SET_VEHICLE_ENGINE_HEALTH(targetEntity, newHealth);
	}
}

//perform impact on entity using a normalized rotation vector
void PerformImpact(Entity targetEntity, Vector3 impactDir, Vector3 impactLoc, Hash weapon, float targetDistance)
{
	if (ENTITY::IS_ENTITY_A_PED(targetEntity) || ENTITY::IS_ENTITY_A_VEHICLE(targetEntity) || ENTITY::IS_ENTITY_AN_OBJECT(targetEntity))
	{
		//push force
		float force = 0.0f;
		//physics damage to vehicles
		float physicsDamage = 0.0f;
		//radius of the effect
		float physicsRadius = 0.0f;

		//read weapon table and lookup values for current weapon
		std::list<WeaponStats>::iterator result = std::find(statList.begin(), statList.end(), weapon);
		if (result != statList.end()) {
			force = result->Force;
			physicsDamage = result->physicsDamage;
			physicsRadius = result->physicsRadius;
		}

		//physics damage will be represent as a multiple of 10
		physicsDamage = physicsDamage / 10;
		
		//falloff will go here
		//net force is less based on distance
		//"400" is extremely far
		//the sniper is at 8 force
		//187.5 distance per force
		//linear
		/*float distanceFactor = (force * 25) / targetDistance;
		if (distanceFactor > 1)
		{
			distanceFactor = 1;
		}
		force = force * distanceFactor;*/

		//based on weapon stats, apply force to the directional vector
		Vector3 forceVector = impactDir;

		//peds seem to cap out at a certain force (as in they do not even move) so just set 30 as the max
		if (ENTITY::IS_ENTITY_A_PED(targetEntity))
		{
			if (force > 30)
			{
				force = 30;
			}
		}
		
		forceVector = multiply(&forceVector, force);
		Vector3 resultForce = forceVector;

		//identify where on the target we struck
		Vector3 dim1, dim2;
		GAMEPLAY::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(targetEntity), &dim1, &dim2);
		Vector3 worldPos = ENTITY::GET_ENTITY_COORDS(targetEntity, true);

		PhysicsObject target = PhysicsObject(dim1, dim2, ENTITY::GET_ENTITY_FORWARD_VECTOR(targetEntity));
		Vector3 damageLoc = ENTITY::GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(targetEntity, impactLoc.x, impactLoc.y, impactLoc.z);
		Vector3 resultRot = target.ApplyForce(damageLoc);		
		
		//reduce force applied for large objects so it doesnt floop unrealistically
		//but also deal slightly more impact damage
		if (target.Dimensions.y > 11 || target.Dimensions.z > 7 || target.Dimensions.x > 8)
		{
			resultForce = multiply(&resultForce, 0.5);
			resultRot = multiply(&resultRot, 0.5);

			physicsDamage = physicsDamage * 2.0;
			physicsRadius = physicsRadius * 2.2;
		}

		//some checks to make sure we never apply too much force
		if (abs(resultForce.x) > force)
		{
			resultForce.x = (resultForce.x / abs(resultForce.x)) * force;
		}
		if (abs(resultForce.y) > force)
		{
			resultForce.y = (resultForce.y / abs(resultForce.y)) * force;
		}
		if (abs(resultForce.z) > force)
		{
			resultForce.z = (resultForce.z / abs(resultForce.z)) * force;
		}

		caption6 = "pre-veh force: " + to_string(resultForce.x) + "__" + to_string(resultForce.y) + "__" + to_string(resultForce.z);
		caption7 = "pre-veh rot: " + to_string(resultRot.x) + "__" + to_string(resultRot.y) + "__" + to_string(resultRot.z);

		if (ENTITY::IS_ENTITY_A_PED(targetEntity))
		{
			ForcePed(targetEntity, resultForce, resultRot, damageLoc);
		}
		else if (ENTITY::IS_ENTITY_A_VEHICLE(targetEntity))
		{			
			ForceVeh(targetEntity, resultForce, resultRot, damageLoc, force, physicsRadius, physicsDamage);
		}
		if (ENTITY::IS_ENTITY_AN_OBJECT(targetEntity))
		{
			ForcePed(targetEntity, resultForce, resultRot, damageLoc);
		}
	}
}

Hash currentWeapon = NULL;
Entity lastAimDump = NULL;
Entity lastAimTarget = NULL;
//last player bullet impact location
//Vector3 impactLoc;
void update_features()
{
	//for debugging
	//draw_menu_line(caption, 250, 15, 18, 0.0, 5.0, false, true);
	//draw_menu_line(caption2, 250, 15, 38, 0.0, 5.0, false, true);	
	//draw_menu_line(caption3, 250, 15, 58, 0.0, 5.0, false, true);
	//draw_menu_line(caption4, 250, 15, 78, 0.0, 5.0, false, true);
	//draw_menu_line(caption5, 250, 15, 98, 0.0, 5.0, false, true);
	//draw_menu_line(caption6, 250, 15, 118, 0.0, 5.0, false, true);
	//draw_menu_line(caption7, 250, 15, 138, 0.0, 5.0, false, true);

	caption = "peds tracked: " + to_string(pedList.size());

	//this mod will proc on presence of a last_weapon_impact from all peds in an area
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::GET_PLAYER_PED(player);

	//cleanup our list for dead entities or entities that dont exist
	//add the player to the ped list if there are none
	if (pedList.size() == 0)
	{
		pedList.push_front(playerPed);
	}
	else
	{
		std::list<Ped>::iterator i = pedList.begin();
		while (i != pedList.end())
		{
			if (!ENTITY::DOES_ENTITY_EXIST(*i) || ENTITY::IS_ENTITY_DEAD(*i))
			{
				i = pedList.erase(i);
			}
			else
			{
				i++;
			}
		}
	}

	int *handles = new int[500 * 2 + 2];
	handles[0] = 500;

	const int amount = PED::GET_PED_NEARBY_PEDS(playerPed, handles, -1);

	//iterate through each item found and add it to the list (if it doesnt already exist)
	for (int i = 0; i < amount; ++i)
	{
		const int index = i * 2 + 2;

		if (handles[index] != 0 && ENTITY::DOES_ENTITY_EXIST(handles[index]) && !ENTITY::IS_ENTITY_DEAD(handles[index]))
		{
			if (std::find(pedList.begin(), pedList.end(), Ped(handles[index])) == pedList.end())
			{
				pedList.push_front(Ped(handles[index]));
			}
		}
	}

	Vector3 impactLoc;
	//we have all peds near the player. check if they test true to an impact event
	for (std::list<int>::const_iterator iterator = pedList.begin(), end = pedList.end(); iterator != end; ++iterator)
	{
		currentPed = *iterator;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(*iterator, &impactLoc))
		{
			caption3 = to_string(impactLoc.x) + "__" + to_string(impactLoc.y) + "__" + to_string(impactLoc.z);

			Hash currentWeapon = NULL;
			WEAPON::GET_CURRENT_PED_WEAPON(currentPed, &currentWeapon, true);
			//player 
			//Vector3 Rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			//currentPed
			Vector3 direction = ENTITY::GET_ENTITY_FORWARD_VECTOR(currentPed);
			//Vector3 direction = RotationToDirection(Rot);

			caption3 = "direction: " +to_string(direction.x) + "__" + to_string(direction.y) + "__" + to_string(direction.z);

			//Vector3 playerLoc = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 1, 0.5);
			Vector3 pedLoc = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(currentPed, 0, 1, 0.5);

			Vector3 finalLoc = impactLoc;

			//check for a direct hit on ped
			int didHit = 0;
			Vector3 hitCoords;
			Vector3 dummyVector;
			Entity objectHit = NULL;
			int directHitPed = 0;

			//direct hit on veh
			int didHit2 = 0;
			Vector3 hitCoords2;
			Vector3 dummyVector2;
			Entity objectHit2 = NULL;
			int directHitVeh = 0;

			//shotgun hashes
			Hash pumpShotgun = GAMEPLAY::GET_HASH_KEY("WEAPON_PUMPSHOTGUN");
			Hash heavyShotgun = GAMEPLAY::GET_HASH_KEY("WEAPON_HEAVYSHOTGUN");
			Hash sawedShotgun = GAMEPLAY::GET_HASH_KEY("WEAPON_SAWNOFFSHOTGUN");
			Hash assaultShotgun = GAMEPLAY::GET_HASH_KEY("WEAPON_ASSAULTSHOTGUN");

			//perform a slight aoe on the ray if a shotgun
			if (currentWeapon == pumpShotgun || currentWeapon == heavyShotgun || currentWeapon == sawedShotgun || currentWeapon == assaultShotgun)
			{
				directHitPed = WORLDPROBE::_CAST_3D_RAY_POINT_TO_POINT(pedLoc.x, pedLoc.y, pedLoc.z, finalLoc.x, finalLoc.y, finalLoc.z, 0.2, 12, currentPed, 7);
				directHitVeh = WORLDPROBE::_CAST_3D_RAY_POINT_TO_POINT(pedLoc.x, pedLoc.y, pedLoc.z, finalLoc.x, finalLoc.y, finalLoc.z, 0.2, 10, currentPed, 7);
			}
			//otherwise a more precisise radius ray
			else
			{
				directHitPed = WORLDPROBE::_CAST_3D_RAY_POINT_TO_POINT(pedLoc.x, pedLoc.y, pedLoc.z, finalLoc.x, finalLoc.y, finalLoc.z, 0.05, 12, currentPed, 7);
				directHitVeh = WORLDPROBE::_CAST_3D_RAY_POINT_TO_POINT(pedLoc.x, pedLoc.y, pedLoc.z, finalLoc.x, finalLoc.y, finalLoc.z, 0.05, 10, currentPed, 7);
			}

			WORLDPROBE::_GET_RAYCAST_RESULT(directHitPed, &didHit, &hitCoords, &dummyVector, &objectHit);
			WORLDPROBE::_GET_RAYCAST_RESULT(directHitVeh, &didHit2, &hitCoords2, &dummyVector2, &objectHit2);
			caption4 = "ray end: " + to_string(hitCoords2.x) + "__" + to_string(hitCoords2.y) + "__" + to_string(hitCoords2.z);

			//calculate distance to target
			caption2 = to_string(pedLoc.x) + "__" + to_string(pedLoc.y) + "__" + to_string(pedLoc.z);
			float targetDistance = get_distance(&pedLoc, &impactLoc);

			caption4 = to_string(targetDistance);
			try
			{
				if (objectHit != NULL && (didHit == 1) && ENTITY::DOES_ENTITY_EXIST(objectHit) &&
					(ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(objectHit, currentPed, true) || ENTITY::IS_ENTITY_DEAD(objectHit)))
				{
					PerformImpact(objectHit, direction, hitCoords, currentWeapon, targetDistance);
				}
				if (objectHit2 != NULL && (didHit2 == 1) && ENTITY::DOES_ENTITY_EXIST(objectHit2) &&
					(ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(objectHit2, currentPed, true) || ENTITY::IS_ENTITY_DEAD(objectHit2)))
				{
					PerformImpact(objectHit2, direction, hitCoords2, currentWeapon, targetDistance);
				}
			}
			catch (...)
			{
				//this is just in case we get an invalid entity and throw exception while perform operation on it
			}
		}
	}
}
bool show_text = false;
int timer = 0;
void main()
{
	statList = LoadConfigFile();
	while (true)
	{
		//catch for key input to reload config
		if (IsKeyJustUp(VK_CONTROL) && IsKeyJustUp(0x4F))
		{
			statList = LoadConfigFile();
			AUDIO::PLAY_SOUND_FRONTEND(-1, "CONTINUE", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
			show_text = true;
			timer = 0;
		}
		if (timer < 80 && show_text)
		{
			draw_menu_line("Weapon Config Reloaded", 250, 15, 138, 0.0, 7.0, false, true, 1);
			timer++;
		}
		if (timer > 80 && show_text)
		{
			show_text = false;
			timer = 0;
		}

		update_features();
		WAIT(0);		
	}
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}