#pragma once

#include "../engine.h"
#include "object.h"

class Construct
{
private:
	struct Part
	{
		Object*						parentObject;
		Object*						thisObject;
		std::vector<Object*>		childObjectVec;

		olc::vec3d					coordinateOrigin; // This is where the object is attached to. Changes according to other parts or movement of the whole construct
		olc::vec3d					coordinateOffset; // Offset of said part

		float						partRotationX; // Rotation of part. Point of view is coordinateOrigin + coordinateOffset
		float						partRotationY;
		float						partRotationZ;

	};
};