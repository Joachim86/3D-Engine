#pragma once

#include "../engine.h"
#include "object.h"

// A construct is just a class containing objects, that moves its content according to time
class Construct
{
private:
	struct Keyframe
	{
		olc::vec3d					coordinateOffset;

		float						timeStamp; // The time when the changes are applied. In milliseconds
		float						partRotationX;
		float						partRotationY;
		float						partRotationZ;

		Keyframe(float _timeStamp, olc::vec3d _coordinateOffset, float _partRotationX, float _partRotationY, float _partRotationZ)
		{
			timeStamp = _timeStamp;
			coordinateOffset = _coordinateOffset;
			partRotationX = _partRotationX;
			partRotationY = _partRotationY;
			partRotationZ = _partRotationZ;
		}
	};



	float						fLoopTime; // max time for whole animation-rotation. in seconds
	float						fElapsedTime;
	uint32_t					nKeyframeStep; //Used to know where in the animation you are
	bool						bStopAnimation = true;
	bool						bStopInterpolate = true;
	std::string					sFilename;

	std::chrono::system_clock::time_point timePoint;

	Object*						objectPointer; // what if the object is deleted? going to need to fix this
	std::vector<Keyframe>		keyframeVector;


private:

	bool loadAnimationFromFile(std::string _filename);

public:

	Construct(std::string _filename, Object* _objectPointer);
	~Construct();


	void update();

	void start();
	void stop();

	//bool addObject(Object* _addObject);
	//bool removeObject(int _objectId);

	void startInterpolate();
	void stopInterpolate();
	void performAnimation(int _keyFrameNumber);

};

inline bool Construct::loadAnimationFromFile(std::string _filename)
{
	return false;
}

inline Construct::Construct(std::string _filename, Object* _objectPointer)
{
	sFilename = _filename;
	objectPointer = _objectPointer;

	if (!loadAnimationFromFile(sFilename))
		return;

	bStopAnimation = false;
	nKeyframeStep = 0;

	timePoint = std::chrono::system_clock::now();
}

inline Construct::~Construct()
{
	//delete objectVectorPointer;
}

inline void Construct::update()
{
	if (!bStopAnimation)
	{
		//Calculate time since start or last loopthrough
		std::chrono::duration<float> elapsedTimeChrono = std::chrono::system_clock::now() - timePoint;
		fElapsedTime = elapsedTimeChrono.count();

		//Check if the animation should be interpolated
		if (bStopInterpolate)
		{
			//loop through all keyframes
			for (int i = 0; i < keyframeVector.size(); i++)
			{
				//nKeyframeStep starts at 0, so it checks for the keyframe in the vector at position 0
				if (i == nKeyframeStep)
				{
					//Check if the timestamp matches the elapsed time
					if (fElapsedTime >= keyframeVector[i].timeStamp)
					{
						//Thats bad, should check if pointer is valid
						objectPointer->setCoordinates(olc::vec3d{
						objectPointer->objectCoordinates.x + keyframeVector[i].coordinateOffset.x,
						objectPointer->objectCoordinates.y + keyframeVector[i].coordinateOffset.y,
						objectPointer->objectCoordinates.z + keyframeVector[i].coordinateOffset.z
							});

						//Also bad
						objectPointer->setRotationX(objectPointer->objectRotationX + keyframeVector[i].partRotationX);
						objectPointer->setRotationY(objectPointer->objectRotationY + keyframeVector[i].partRotationY);
						objectPointer->setRotationZ(objectPointer->objectRotationZ + keyframeVector[i].partRotationZ);

						nKeyframeStep++;
					}
				}
			}
		}
		else //Interpolate!
		{

		}

		//If the elapsed time is greater than the loop time, set the timepoint to 0
		if (fElapsedTime >= fLoopTime)
			timePoint = std::chrono::system_clock::now();
	}
	else
	{
		fElapsedTime = 0;
	}
}

inline void Construct::start()
{
	bStopAnimation = false;
}

inline void Construct::stop()
{
	bStopAnimation = true;
}

inline void Construct::startInterpolate()
{
	bStopInterpolate = true;
}

inline void Construct::stopInterpolate()
{
	bStopInterpolate = false;
}

inline void Construct::performAnimation(int _keyFrameNumber)
{
}



// Structure of Animation.ani file
/*
- Animation Name
- Loop Time
- Object to steer
- Keyframes
	o Offset and Rotation
	o timestamp
*/