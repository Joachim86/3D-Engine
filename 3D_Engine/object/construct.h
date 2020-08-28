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


		Keyframe()
		{
			coordinateOffset = { 0, 0, 0, 0 };
			timeStamp = 0;
			partRotationX = 0;
			partRotationY = 0;
			partRotationZ = 0;
		}

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
	std::string					sAnimationName;

	std::chrono::system_clock::time_point timePoint;

	//Object*						objectPointer; // what if the object is deleted? going to need to fix this
	std::vector<Keyframe>		keyframeVector;


private:

	bool loadAnimationFromFile(std::string _filename);
	bool safeStof(std::string _input, float& output);

public:

	Construct(std::string _filename);
	~Construct();


	void update(Object* _objectPointer);

	void start();
	void stop();

	//bool addObject(Object* _addObject);
	//bool removeObject(int _objectId);

	void startInterpolate();
	void stopInterpolate();
	void performAnimation(Object* _objectPointer);

};

inline bool Construct::loadAnimationFromFile(std::string _filename)
{
	std::ifstream file(_filename);

	if (!file.is_open())
		return false;


	for (int i = 0; !file.eof(); i++)
	{
		char temp[128];
		file.getline(temp, 128);

		std::string line = temp;

		if (i == 0)
		{
			sAnimationName = line;
		}
		else if (i == 1)
		{
			//If stof fails it would otherwise throw an exception and crash the program
			if (!safeStof(line, fLoopTime))
				return false;
		}
		else
		{
			if (line[0] == 'k')
			{
				Keyframe keyframeTemp;

				if (line[1] == 't') //t stands for timestamp
				{
					if (!safeStof(line.substr(1, line.size() - 1), fLoopTime))
						return false;
				}
				else if (line[1] == 'x') //if its a coordinate
				{
					if (line[2] == 'r') //if its for rotation
					{
						if (!safeStof(line.substr(2, line.size() - 2), keyframeTemp.partRotationX))
							return false;
					}
					else
					{
						if (!safeStof(line.substr(1, line.size() - 1), keyframeTemp.coordinateOffset.x))
							return false;
					}
				}
				else if (line[1] == 'y')
				{
					if (line[2] == 'r') //if its for rotation
					{
						if (!safeStof(line.substr(2, line.size() - 2), keyframeTemp.partRotationY))
							return false;
					}
					else
					{
						if (!safeStof(line.substr(1, line.size() - 1), keyframeTemp.coordinateOffset.y))
							return false;
					}
				}
				else if (line[1] == 'z')
				{
					if (line[2] == 'r') //if its for rotation
					{
						if (!safeStof(line.substr(2, line.size() - 2), keyframeTemp.partRotationZ))
							return false;
					}
					else
					{
						if (!safeStof(line.substr(1, line.size() - 1), keyframeTemp.coordinateOffset.z))
							return false;
					}
				}
			}
		}
	}


	return true;
}

inline bool Construct::safeStof(std::string _input, float& output)
{
	try
	{
		output = std::stoi(_input);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return false;
	}
	return true;
}

inline Construct::Construct(std::string _filename)
{
	sFilename = _filename;
	//objectPointer = _objectPointer;

	if (!loadAnimationFromFile(sFilename))
		return;

	nKeyframeStep = 0;

	timePoint = std::chrono::system_clock::now();
}

inline Construct::~Construct()
{
	//delete objectVectorPointer;
}

inline void Construct::update(Object* _objectPointer)
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
						_objectPointer->setCoordinates(olc::vec3d{
						_objectPointer->objectCoordinates.x + keyframeVector[i].coordinateOffset.x,
						_objectPointer->objectCoordinates.y + keyframeVector[i].coordinateOffset.y,
						_objectPointer->objectCoordinates.z + keyframeVector[i].coordinateOffset.z
							});

						//Also bad
						_objectPointer->setRotationX(_objectPointer->objectRotationX + keyframeVector[i].partRotationX);
						_objectPointer->setRotationY(_objectPointer->objectRotationY + keyframeVector[i].partRotationY);
						_objectPointer->setRotationZ(_objectPointer->objectRotationZ + keyframeVector[i].partRotationZ);

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

inline void Construct::performAnimation(Object* _objectPointer)
{
}



// Structure of Animation.ani file
/*
- Animation Name
- Loop Time
- Keyframes
	o Offset and Rotation
	o timestamp
*/