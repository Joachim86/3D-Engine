#pragma once

#include "../engine.h"

class Object
{

public:

	static int numberOfObjects;
	static std::vector<Object*> objectVector;
	static std::vector<olc::triangle> vecTrianglesToRaster;
	static std::vector<olc::Sprite*> spriteContainer;


public:
	unsigned int ID;
	float objectRotationX, objectRotationY, objectRotationZ;
	std::string objectName;

	olc::mesh objectMesh;

	olc::vec3d objectCoordinates;
	olc::vec3d objectLookingDirection;

	olc::Sprite* objectSprite; // Not in use right now

	olc::mat4x4 objectRotationMatrixX;
	olc::mat4x4 objectRotationMatrixY;
	olc::mat4x4 objectRotationMatrixZ;
	olc::mat4x4 objectMatrix;

public:
	Object(std::string Name, std::string meshFile, olc::Sprite* sprite, uint8_t _texturecode, olc::vec3d _coordinates_);
	~Object();

	void setCoordinates(void* _coordinates);

	void setRotationX(float theta);
	void setRotationY(float theta);
	void setRotationZ(float theta);

	void changeMeshCenter(olc::vec3d _coordinates);
	void changeMeshCenter(float _x, float _y, float _z);
	void rotateMeshX(float _theta);
	void rotateMeshY(float _theta);
	void rotateMeshZ(float _theta);

	void updateRotation();

	virtual void update();

	static bool deleteObject(unsigned int position);
};

int Object::numberOfObjects = 0;
std::vector<Object*> Object::objectVector;
std::vector<olc::triangle> Object::vecTrianglesToRaster;
std::vector<olc::Sprite*> Object::spriteContainer;



Object::Object(std::string Name, std::string meshFile, olc::Sprite* sprite, uint8_t _texturecode, olc::vec3d _coordinates_)
{
	objectName = Name;
	//objectMesh.LoadFromObjectFile(meshFile, !sprite ? true : false);
	objectMesh.LoadFromObjectFile(meshFile, true);
	objectMesh.textureCodeMesh = _texturecode;

	//objectCoordinates = *(olc::vec3d*)_coordinates_;
	objectCoordinates = _coordinates_;

	if (!sprite)
		objectSprite = sprite;


	ID = Object::numberOfObjects;

	objectRotationMatrixX = olc::Matrix_MakeIdentity();
	objectRotationMatrixY = olc::Matrix_MakeIdentity();
	objectRotationMatrixZ = olc::Matrix_MakeIdentity();

	Object::numberOfObjects++;
}

Object::~Object()
{
	Object::numberOfObjects--;
	delete[] objectSprite;
	//std::cout << "object deleted! objects left: " << Object::numberOfObjects << "\n";

}

void Object::setCoordinates(void* _coordinates)
{
	objectCoordinates = *(olc::vec3d*)_coordinates;
}

void Object::setRotationX(float theta)
{
	objectRotationX = theta;
	objectRotationMatrixX = olc::Matrix_MakeRotationX(theta);
}

void Object::setRotationY(float theta)
{
	objectRotationY = theta;
	objectRotationMatrixY = olc::Matrix_MakeRotationY(theta);
}

void Object::setRotationZ(float theta)
{
	objectRotationZ = theta;
	objectRotationMatrixZ = olc::Matrix_MakeRotationZ(theta);
}

void Object::changeMeshCenter(olc::vec3d _coordinates)
{
	
	for (auto& i : objectMesh.tris)
	{
		for (int j = 0; j < 3; j++)
		{
			i.p[j].x += _coordinates.x;
			i.p[j].y += _coordinates.y;
			i.p[j].z += _coordinates.z;
		}
	}
}

void Object::changeMeshCenter(float _x, float _y, float _z)
{
	for (auto& i : objectMesh.tris)
	{
		for (int j = 0; j < 3; j++)
		{
			i.p[j].x += _x;
			i.p[j].y += _y;
			i.p[j].z += _z;
		}
	}
}

void Object::rotateMeshX(float _theta)
{
	olc::mat4x4 rotateMatTemp = olc::Matrix_MakeRotationX(_theta);

	for (auto& i : objectMesh.tris)
	{
		for (int j = 0; j < 3; j++)
		{
			i.p[j] = Matrix_MultiplyVector(rotateMatTemp, i.p[j]);
		}
	}
	std::cout << "i am called x\n";
}

void Object::rotateMeshY(float _theta)
{
	olc::mat4x4 rotateMatTemp = olc::Matrix_MakeRotationY(_theta);

	for (auto& i : objectMesh.tris)
	{
		for (int j = 0; j < 3; j++)
		{
			i.p[j] = Matrix_MultiplyVector(rotateMatTemp, i.p[j]);
		}
	}
	std::cout << "i am called y\n";
}

void Object::rotateMeshZ(float _theta)
{
	olc::mat4x4 rotateMatTemp = olc::Matrix_MakeRotationZ(_theta);

	for (auto& i : objectMesh.tris)
	{
		for (int j = 0; j < 3; j++)
		{
			i.p[j] = Matrix_MultiplyVector(rotateMatTemp, i.p[j]);
		}
	}
	std::cout << "i am called z\n";
}


void Object::updateRotation()
{
	olc::vec3d vUpObject = { 0,1,0 };
	olc::vec3d vTargetObject = { 0,0,1 };

	olc::mat4x4 objectRotationMatrix = Matrix_MultiplyMatrix(objectRotationMatrixX, objectRotationMatrixY);

	objectLookingDirection = Matrix_MultiplyVector(objectRotationMatrix, vTargetObject);
	vTargetObject = Vector_Add(objectCoordinates, objectLookingDirection);
	objectMatrix = Matrix_PointAt(objectCoordinates, vTargetObject, vUpObject);
}

void Object::update()
{

}

bool Object::deleteObject(unsigned int position)
{
	if (position > Object::objectVector.size() || Object::numberOfObjects <= 0)
	{
		return false;
	}
	else
	{
		delete Object::objectVector[position];
		Object::objectVector.erase(Object::objectVector.begin() + position);
		return true;
	}
	
}
