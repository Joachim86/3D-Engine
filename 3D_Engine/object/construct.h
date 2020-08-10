#pragma once

#include "../engine.h"
#include "object.h"


struct Part
{
private:
	// A construct consists of multiple parts and their purpose is to update istelf and every other child part
	int32_t Id;


	static int32_t				IdCount = 0;

	Object*						thisObject;

	olc::vec3d					coordinateOffset; // Offset of said part

	float						partRotationX; // Rotation of part. Point of view is coordinateOrigin + coordinateOffset
	float						partRotationY;
	float						partRotationZ;

public:
	Part(Object* _thisObject)
	{

	}

	~Part()
	{
		
	}

	void update()
	{

	}

	int32_t getId() { return Id; };

	void setRotationX(float _partRotationX) { partRotationX = _partRotationX; }
	float getRotationX() { return partRotationX; }

	void setRotationY(float _partRotationY) { partRotationY = _partRotationY; }
	float getRotationY() { return partRotationY; }

	void setRotationZ(float _partRotationZ) { partRotationZ = _partRotationZ; }
	float getRotationZ() { return partRotationZ; }

	void setOffset(olc::vec3d _coordinateOffset) { coordinateOffset = _coordinateOffset; }
	olc::vec3d getOffset() { return coordinateOffset; }
};
int32_t Part::IdCount;


// A construct is just a class containing objects, that moves its content according to certain requirements

class Construct
{
	float						timeCount = 0;

	static	std::vector<Construct*> constructVector;

public:
	Construct();
	~Construct();

	bool createConstruct();

};

std::vector<Construct*> Construct::constructVector;