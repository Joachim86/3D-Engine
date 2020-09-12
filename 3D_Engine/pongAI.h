#pragma once

#include "camera.h"
#include "boundingBox.h"
#include "engine.h"
#include "object/object.h"
#include "object/construct.h"

class pongAI
{
public:
	void update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongPositionStartX, float& _pongPositionEndX);
	void update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongCirclePositionStartX);
};

void pongAI::update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongPositionStartX, float& _pongPositionEndX)
{
	//_pongPositionX += 4 * fElapsedTime;
	
	if (_ballPositionX > (_pongPositionStartX + _pongPositionEndX) / 2)
	{
		_pongPositionStartX += 5 * fElapsedTime;
		_pongPositionEndX += 5 * fElapsedTime;
	}
	else if (_ballPositionX < (_pongPositionStartX + _pongPositionEndX) / 2)
	{
		_pongPositionStartX -= 5 * fElapsedTime;
		_pongPositionEndX -= 5 * fElapsedTime;
	}
}

void pongAI::update(float fElapsedTime, float _ballPositionX, float _ballPositionY, float& _pongCirclePositionStartX)
{
	//_pongPositionX += 4 * fElapsedTime;

	if (_ballPositionX > _pongCirclePositionStartX)
	{
		_pongCirclePositionStartX += 5 * fElapsedTime;
	}
	else if (_ballPositionX < _pongCirclePositionStartX)
	{
		_pongCirclePositionStartX -= 5 * fElapsedTime;
	}
}