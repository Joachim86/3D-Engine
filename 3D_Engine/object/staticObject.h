#pragma once

#include "object.h"




class staticObject : public Object
{
public:
	staticObject(std::string Name, std::string meshFile, olc::Sprite* sprite, uint8_t _texturecode, olc::vec3d _coordinates_);
	~staticObject();

	void update() override;
};

staticObject::staticObject(std::string Name, std::string meshFile, olc::Sprite* sprite, uint8_t _texturecode, olc::vec3d _coordinates_) : Object(Name, meshFile, sprite, _texturecode, _coordinates_)
{

}

staticObject::~staticObject()
{

}

void staticObject::update()
{

}