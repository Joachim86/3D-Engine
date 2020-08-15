#pragma once

#include "../engine.h"
#include "object.h"

namespace obj
{

	struct Keyframe;
	struct Part;
	class Construct;



	struct Keyframe
	{
	private:
		olc::vec3d					coordinateOffset;

		float						timeStamp; // The time when the changes are applied. In milliseconds
		float						partRotationX;
		float						partRotationY;
		float						partRotationZ;

		Keyframe(float _timeStamp, float _partRotationX, float _partRotationY, float _partRotationZ)
		{
			timeStamp = _timeStamp;
			partRotationX = _partRotationX;
			partRotationY = _partRotationY;
			partRotationZ = _partRotationZ;
		}
	};


	// A construct consists of multiple parts and their purpose is to update istelf and every other part in the list
	struct Part
	{
	private:
		int32_t						Id;



		std::vector<Keyframe>		keyframeVector;
		Object*						thisObject = nullptr;

		// ++++++++++++++++++++++++++++ maybe not necessery ++++++++++++++++++++++++++++
		/*
		olc::vec3d					coordinateOffset; // Offset of said part

		float						partRotationX; // Rotation of part. Point of view is coordinateOrigin + coordinateOffset
		float						partRotationY;
		float						partRotationZ;
		*/
		// ++++++++++++++++++++++++++++ maybe not necessery ++++++++++++++++++++++++++++

	public:
		Part(Object* _thisObject, std::vector<Keyframe> _keyframVector, uint32_t _Id)
		{

		}

		~Part()
		{

		}

		void update(Object* _parentObject)
		{

		}

		int32_t getId() { return Id; };

		/*
		void setRotationX(float _partRotationX) { partRotationX = _partRotationX; }
		float getRotationX() { return partRotationX; }

		void setRotationY(float _partRotationY) { partRotationY = _partRotationY; }
		float getRotationY() { return partRotationY; }

		void setRotationZ(float _partRotationZ) { partRotationZ = _partRotationZ; }
		float getRotationZ() { return partRotationZ; }

		void setOffset(olc::vec3d _coordinateOffset) { coordinateOffset = _coordinateOffset; }
		olc::vec3d getOffset() { return coordinateOffset; }
		*/
	};



	// A construct is just a class containing objects, that moves its content according to certain requirements
	class Construct
	{
		float						loopTime; // max time for whole animation-rotation. in seconds
		float						elapsedTime;

		std::chrono::system_clock::time_point timePoint1;

		uint32_t					partIdCount = 0;
		bool						stopAnimation = false;

		Object* parentObject;

		static	std::vector<Construct*> constructVector;
		std::vector<Part*> partVector;

	public:

		Construct(Object* _parentObject, float _loopTime);
		~Construct();




		//bool createConstruct(Object* _parentObject, float _loopTime);
		//bool deleteConstruct();
		bool addPart(Object* _thisObject, std::vector<Keyframe> _keyframeVector);
		bool addPart(Object* _thisObject, std::string _loadFromAnimationFile);
		bool deletePart(uint32_t _partId);
		void update();
		void toggleAnimation(bool _stopAnimation = true);
	};
	std::vector<Construct*> Construct::constructVector;



	Construct::Construct(Object* _parentObject, float _loopTime)
	{
		timePoint1 = std::chrono::system_clock::now();

		parentObject = _parentObject;

		loopTime = _loopTime;
	}



	/*
	bool Construct::createConstruct(Object* _parentObject, float _loopTime)
	{
		return false;
	}
	*/



	bool Construct::addPart(Object* _thisObject, std::vector<Keyframe> _keyframeVector)
	{
		return false;
	}



	bool Construct::addPart(Object* _thisObject, std::string _loadFromAnimationFile)
	{
		return false;
	}



	bool Construct::deletePart(uint32_t _partId)
	{
		return false;
	}



	void Construct::update()
	{
		if (!stopAnimation)
		{

		}

		std::chrono::duration<float> elapsedTimeChrono = std::chrono::system_clock::now() - timePoint1;
		elapsedTime = elapsedTimeChrono.count();

		if (elapsedTime > loopTime)
			timePoint1 = std::chrono::system_clock::now();

		//std::cout << elapsedTime << "\n";
	}

	void Construct::toggleAnimation(bool _stopAnimation)
	{

	}

}