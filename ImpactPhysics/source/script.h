#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include "..\..\inc\enums.h"

#include "..\..\inc\main.h"
#include "config.h"
#include <functional>   // std::minus, std::divides
#include <numeric> 
//physics object has dimensions and a center of mass
class PhysicsObject
{
private:
	//mins
	Vector3 pDim1;
	Vector3 pDim2;
	//max

	Vector3 pForwardDir;
public:
	//build physics object based on dimensions output from GET_MODEL_DIMENSIONS
	PhysicsObject(Vector3 dim1, Vector3 dim2, Vector3 forwardDir)
	{
		Dimensions.x = dim2.x + abs(dim1.x);
		Dimensions.y = dim2.y + abs(dim1.y);
		Dimensions.z = dim2.z + abs(dim2.z);
		pForwardDir = forwardDir;

		pDim1 = dim1;
		pDim2 = dim2;
	}

	Vector3 Dimensions;

	Vector3 ApplyForce(Vector3 relativeImpactLocation)
	{
		Vector3 imp = relativeImpactLocation;
				
		//calculate the impact location
		//normalize the center of the object

		//positive value, go with dim2
		float mod[2];
		if (imp.x > 0)
		{
			mod[0] = imp.x / pDim2.x;
		}
		//dim1
		else
		{
			mod[0] = imp.x / pDim1.x;
		}
		if (imp.y > 0)
		{
			mod[1] = imp.y / pDim2.y;
		}
		else
		{
			mod[1] = -imp.y / pDim1.y;
		}
		if (imp.z > 0)
		{
			mod[2] = imp.z / pDim2.z;
		}
		else
		{
			mod[2] = -imp.z / pDim1.z;
		}
		//capping values greater than 1 less than -1
		for (int i = 0; i < sizeof(mod); i++)
		{
			if (abs(mod[i]) > 1)
			{
				mod[i] = abs(mod[i])/mod[i];
			}
		}

		//the mod[] values will let us know how to impact rotation
		Vector3 resultRot;
		resultRot.x = mod[0];
		resultRot.y = mod[1];
		resultRot.z = mod[2];

		return resultRot;
	}
};
void ScriptMain();