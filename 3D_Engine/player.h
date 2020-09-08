#pragma once

#include "camera.h"
#include "boundingBox.h"
#include "engine.h"
#include "object/object.h"
#include "object/construct.h"

class Player
{
private:
	olc::PixelGameEngine* olcEnginePointer;
	Camera* cameraPointer;


public:
	Player();

	void update();
};

Player::Player()
{
	return;
}

void Player::update()
{

}