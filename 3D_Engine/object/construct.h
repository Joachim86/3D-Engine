#pragma once

#include "../engine.h"
#include "object.h"

// A construct is just a class containing objects, that moves its content according to time
class Construct
{
	struct Keyframe
	{
	private:
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
	bool						bStopAnimation = false;
	bool						bStopInterpolate = true;
	std::string					sFilename;

	std::chrono::system_clock::time_point timePoint1;

	std::vector<Object*>		*objectVectorPointer;
	std::vector<Keyframe>		keyframeVector;

public:

	Construct(std::string _filename, std::vector<Object*> *_objectVectorPointer);
	~Construct();


	void update();

	void start();
	void stop();

	//bool addObject(Object* _addObject);
	//bool removeObject(int _objectId);

	void startInterpolate();
	void stopInterpolate();
	void performKeyframe(int _keyFrameNumber);

};

inline Construct::Construct(std::string _filename, std::vector<Object*>* _objectVectorPointer)
{
}

inline Construct::~Construct()
{
}

inline void Construct::update()
{
}

inline void Construct::start()
{
}

inline void Construct::stop()
{
}

inline void Construct::startInterpolate()
{
}

inline void Construct::stopInterpolate()
{
}

inline void Construct::performKeyframe(int _keyFrameNumber)
{
}



// Structure of Animation.ani file
/*
- Animation Name
- Loop Time
- Used Objects in Animation by name
- Keyframes
	o Offset and Rotation
	o timestamp
	o Object to steer
*/