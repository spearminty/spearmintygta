/**
* Copyright (C) 2015 Crosire
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include <ctime>
#include <list>
#include "script.h"

class WeaponStats
{

public:
	Hash wHash;
	float Force;
	float physicsDamage;
	float physicsRadius;
	WeaponStats(Hash hash, float force, float physicsdamage, float physicsradius)
	{
		Force = force;
		wHash = hash;
		physicsDamage = physicsdamage;
		physicsRadius = physicsradius;
	};

	bool operator == (const int &Ref) const
	{
		return(this->wHash == Ref);
	}

	int GetHash() const
	{
		return(this->wHash);
	}
	const float GetDamage() const
	{
		return(this->Force);
	}
	const float GetPhysicsDamage() const
	{
		return(this->physicsDamage);
	}
	const float GetPhysicsRadius() const
	{
		return(this->physicsRadius);
	}
};
std::list<WeaponStats> LoadConfigFile();