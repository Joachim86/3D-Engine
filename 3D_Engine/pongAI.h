#pragma once

#include "camera.h"
#include "boundingBox.h"
#include "engine.h"
#include "object/object.h"
#include "object/construct.h"

class pongAI
{
public:
	void update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongCirclePositionStartX);
};


void pongAI::update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongCirclePositionStartX)
{
	

	if (_ballPositionX > _pongCirclePositionStartX && _pongCirclePositionStartX < 22 - 5 * fElapsedTime)
	{
		_pongCirclePositionStartX += 5 * fElapsedTime;
	}
	else if (_ballPositionX < _pongCirclePositionStartX && _pongCirclePositionStartX > 3 + 5 * fElapsedTime)
	{
		_pongCirclePositionStartX -= 5 * fElapsedTime;
	}
}