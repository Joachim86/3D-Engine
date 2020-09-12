#pragma once


#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>

#include "engine.h"



namespace jph
{

	enum jErrorCode
	{
		FAIL = 0,
		OK = 1,
		NO_FILE = -1,
	};

	enum jObjectCode
	{
		CIRCLE = 0,
		TRIANGLE = 1,
		RECTANGLE = 2
	};
	
	struct jVector2D
	{
		float x;
		float y;
	};
	

	struct jLine
	{
		unsigned int ID;

		jVector2D start;
		jVector2D end;

		jVector2D offsetPosition = { 0 , 0 };

		float radius;
	};

	struct jCircle
	{
		unsigned int ID;

		jVector2D position;
		jVector2D positionOld;
		jVector2D velocity;
		jVector2D acceleration;

		float radius;
		float mass;
		float friction;
		float simTimeRemaining;
		float efficiency;

		bool isStatic = false;
	};

	struct jRectangle
	{
		unsigned int ID;

		jVector2D position;
		jVector2D positionOld;
		jVector2D velocity;
		jVector2D acceleration;

		float width;
		float height;
		float mass;
		float friction;
		float simTimeRemaining;
		float efficiency;
	};

	struct jTriangle
	{
		unsigned int ID;

		jVector2D position;
		jVector2D positionOld;
		jVector2D velocity;
		jVector2D acceleration;				//   |+
											// a |   +  c
											//   |_____+
		float a;							//      b
		float b;
		float c;
		float mass;
		float friction;
		float simTimeRemaining;
		float efficiency;
	};






	class Jphysics
	{
	public:
		Jphysics();

	public:
		jph::jErrorCode Start();

		unsigned int addCircle(jVector2D position, float radius, jVector2D velocity, jVector2D acceleration, float mass, float friction, float efficiency, bool _isStatic);

		unsigned int addLine(jVector2D positionStart, jVector2D positionEnd, float radius);

		bool removeObjectByID(unsigned int IDtoErase);

		void update(float fElapsedTime);
	public:
		std::vector<jCircle> vecCircles;
		std::vector<jLine> vecLines;

		float worldDrag = 0.0f; // 0.8 = normal
		float worldGravity = -0.0f; // 100 = normal

	private:
		static std::atomic<bool> bAtomActive; // Thread shared

		unsigned int iNumberOfObjects;

	private:
		bool DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2);
		bool IsPointInCircle(float x1, float y1, float r1, float px, float py);
	};

	Jphysics::Jphysics()
	{
		iNumberOfObjects = 0;
	}

	jph::jErrorCode Jphysics::Start()
	{
		if (!bAtomActive)
		{
			bAtomActive = true;
		}
		else
		{
			return jph::FAIL;
		}

		return jph::OK;
	}

	inline unsigned int Jphysics::addCircle(jVector2D position, float radius, jVector2D velocity = { 0, 0 }, jVector2D acceleration = { 0, 0 }, float mass = 0, float friction = 0, float efficiency = 1, bool _isStatic = false)
	{
		jCircle circleToAdd;

		circleToAdd.ID = iNumberOfObjects++;

		circleToAdd.position = position;
		circleToAdd.velocity = velocity;
		circleToAdd.acceleration = acceleration;

		circleToAdd.radius = radius;
		circleToAdd.mass = mass;
		circleToAdd.friction = friction;
		circleToAdd.efficiency = efficiency;
		circleToAdd.isStatic = _isStatic;

		vecCircles.emplace_back(circleToAdd);

		return circleToAdd.ID;
	}

	inline unsigned int Jphysics::addLine(jVector2D positionStart, jVector2D positionEnd, float radius)
	{
		jLine lineToAdd;

		lineToAdd.ID = iNumberOfObjects++;

		lineToAdd.start = positionStart;
		lineToAdd.end = positionEnd;
		lineToAdd.radius = radius;

		vecLines.emplace_back(lineToAdd);

		return lineToAdd.ID;
	}

	inline bool Jphysics::removeObjectByID(unsigned int IDtoErase)
	{
		for (int i = 0; i < vecCircles.size(); i++)
		{
			if (vecCircles[i].ID == IDtoErase)
			{
				vecCircles.erase(vecCircles.begin() + i);
				return true;
			}
		}

		for (int j = 0; j < vecLines.size(); j++)
		{
			if (vecLines[j].ID == IDtoErase)
			{
				vecLines.erase(vecLines.begin() + j);
				return true;
			}
		}

		return false;
	}

	inline void Jphysics::update(float fElapsedTime)
	{
		std::vector<std::pair<jCircle*, jCircle*>> vecCollidingPairs;
		std::vector<jCircle*> vecFakeBalls; //not needed yet

		float fStable = 0.001f;

		int nSimulationUpdates = 4;

		int nMaxSimulationSteps = 15;

		float fSimElapsedTime = fElapsedTime / (float)nSimulationUpdates;

		//update with simulation time
		for (int i = 0; i < nSimulationUpdates; i++)
		{
			for (auto& circle : vecCircles)
				circle.simTimeRemaining = fSimElapsedTime;

			// Erode simulation time on a per objec tbasis, depending upon what happens
			// to it during its journey through this epoch
			for (int j = 0; j < nMaxSimulationSteps; j++)
			{
				// Update Ball Positions
				for (auto& circle : vecCircles)
				{
					if (circle.simTimeRemaining > 0.0f)
					{
						circle.positionOld.x = circle.position.x;
						circle.positionOld.y = circle.position.y;

						circle.acceleration.x = -circle.velocity.x * worldDrag;
						circle.acceleration.y = -circle.velocity.y * worldDrag + (circle.isStatic ? 0 : worldGravity);

						circle.velocity.x += circle.acceleration.x * circle.simTimeRemaining;
						circle.velocity.y += circle.acceleration.y * circle.simTimeRemaining;

						circle.position.x += circle.velocity.x * circle.simTimeRemaining;	// Update position
						circle.position.y += circle.velocity.y * circle.simTimeRemaining;

						
						// Stop ball when velocity is neglible
						if (fabs(circle.velocity.x * circle.velocity.x + circle.velocity.y * circle.velocity.y) < fStable)
						{
							circle.velocity.x = 0;
							circle.velocity.y = 0;
						}
						
					}
				}

				// Work out static collisions with walls and displace balls so no overlaps
				for (auto& circle : vecCircles)
				{
					//Against Edges
					for (auto& edge : vecLines)
					{
						// Check that line formed by velocity vector, intersects with line segment
						float fLineX1 = edge.end.x - edge.start.x;
						float fLineY1 = edge.end.y - edge.start.y;

						float fLineX2 = circle.position.x - edge.start.x;
						float fLineY2 = circle.position.y - edge.start.y;

						float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

						// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
						// how much of the segment is in the "shadow" of the object vector. The min and max clamp
						// this to lie between 0 and the line segment length, which is then normalised. We can
						// use this to calculate the closest point on the line segment

						float t = std::max(0.0f, std::min(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

						// Which we do here
						float fClosestPointX = edge.start.x + t * fLineX1;
						float fClosestPointY = edge.start.y + t * fLineY1;

						// And once we know the closest point, we can check if the ball has collided with the segment in the
						// same way we check if two balls have collided
						float fDistance = sqrtf((circle.position.x - fClosestPointX) * (circle.position.x - fClosestPointX) + (circle.position.y - fClosestPointY) * (circle.position.y - fClosestPointY));

						if (fDistance <= (circle.radius + edge.radius))
						{
							// Collision has occurred - treat collision point as a ball that cannot move. To make this
							// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
							// so it behaves like a solid object when the momentum calculations are performed
							jCircle* fakeball = new jCircle();
							fakeball->radius = edge.radius;
							fakeball->mass = circle.mass * circle.efficiency; //worldDrag? 0.8f standard
							fakeball->position.x = fClosestPointX;
							fakeball->position.y = fClosestPointY;
							fakeball->velocity.x = -circle.velocity.x;	// We will use these later to allow the lines to impart energy into ball
							fakeball->velocity.y = -circle.velocity.y;	// if the lines are moving, i.e. like pinball flippers

							// Store Fake Ball
							vecFakeBalls.push_back(fakeball);

							// Add collision to vector of collisions for dynamic resolution
							vecCollidingPairs.push_back({ &circle, fakeball });

							// Calculate displacement required
							float fOverlap = 1.0f * (fDistance - circle.radius - fakeball->radius);

							// Displace Current Ball away from collision
							circle.position.x -= fOverlap * (circle.position.x - fakeball->position.x) / fDistance;
							circle.position.y -= fOverlap * (circle.position.y - fakeball->position.y) / fDistance;
						}
					}

					// Against other balls
					for (auto& target : vecCircles)
					{
						if (circle.ID != target.ID)
						{
							if (DoCirclesOverlap(circle.position.x, circle.position.y, circle.radius, target.position.x, target.position.y, target.radius))
							{
								vecCollidingPairs.push_back({ &circle, &target });

								float fDistance = sqrtf((circle.position.x - target.position.x) * (circle.position.x - target.position.x) + (circle.position.y - target.position.y) * (circle.position.y - target.position.y));

								float fOverlap = 0.5f * (fDistance - circle.radius - target.radius);

								circle.position.x -= fOverlap * (circle.position.x - target.position.x) / fDistance;
								circle.position.y -= fOverlap * (circle.position.y - target.position.y) / fDistance;

								target.position.x += fOverlap * (circle.position.x - target.position.x) / fDistance;
								target.position.y += fOverlap * (circle.position.y - target.position.y) / fDistance;
							}
						}
					}

					float fIntendedSpeed = sqrtf(circle.velocity.x * circle.velocity.x + circle.velocity.y * circle.velocity.y);
					float fIntendedDistance = fIntendedSpeed * circle.simTimeRemaining;
					float fActualDistance = sqrtf((circle.position.x - circle.positionOld.x) * (circle.position.x - circle.positionOld.x) + (circle.position.y - circle.positionOld.y) * (circle.position.y - circle.positionOld.y));
					float fActualTime = fActualDistance / fIntendedSpeed;

					// After static resolution, there may be some time still left for this epoch, so allow simulation to continue
					circle.simTimeRemaining = circle.simTimeRemaining - fActualTime;
				}

				// Now work out dynamic collisions
				for (auto c : vecCollidingPairs)
				{
					jCircle* b1 = c.first;
					jCircle* b2 = c.second;

					// Is circle static
					//if (b1->isStatic) b1->mass = b2->mass * b2->efficiency;
					//if (b2->isStatic) b2->mass = b1->mass * b1->efficiency;


					// Distance between balls
					float fDistance = sqrtf((b1->position.x - b2->position.x) * (b1->position.x - b2->position.x) + (b1->position.y - b2->position.y) * (b1->position.y - b2->position.y));

					// Normal
					float nx = (b2->position.x - b1->position.x) / fDistance;
					float ny = (b2->position.y - b1->position.y) / fDistance;

					// Tangent
					float tx = -ny;
					float ty = nx;

					// Dot Product Tangent
					float dpTan1 = b1->velocity.x * tx + b1->velocity.y * ty;
					float dpTan2 = b2->velocity.x * tx + b2->velocity.y * ty;

					// Dot Product Normal
					float dpNorm1 = b1->velocity.x * nx + b1->velocity.y * ny;
					float dpNorm2 = b2->velocity.x * nx + b2->velocity.y * ny;

					// Conservation of momentum in 1D
					//float m1 = b1->efficiency * (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
					//float m2 = b1->efficiency * (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

					float m1 = b1->efficiency * (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
					float m2 = b2->efficiency * (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

					// Update ball velocities
					/*
					b1->velocity.x = tx * dpTan1 + nx * m1;
					b1->velocity.y = ty * dpTan1 + ny * m1;
					b2->velocity.x = tx * dpTan2 + nx * m2;
					b2->velocity.y = ty * dpTan2 + ny * m2;
					*/


					
					if (b1->isStatic || b2->isStatic)
					{
						if (b1->isStatic)
						{
							b2->velocity.x = tx * dpTan2 + nx * m2 * 3;
							b2->velocity.y = ty * dpTan2 + ny * m2 * 3;
						}
						
						if (b2->isStatic)
						{
							b1->velocity.x = tx * dpTan1 + nx * m1 * 3;
							b1->velocity.y = ty * dpTan1 + ny * m1 * 3;
						}
					}
					else
					{
						b1->velocity.x = tx * dpTan1 + nx * m1;
						b1->velocity.y = ty * dpTan1 + ny * m1;
						b2->velocity.x = tx * dpTan2 + nx * m2;
						b2->velocity.y = ty * dpTan2 + ny * m2;
					}
					

					/*
					if (b1->isStatic)
					{
						b1->velocity.x *= -1;
						b1->velocity.y *= -1;
					}
					else
					{
						b1->velocity.x = tx * dpTan1 + nx * m1;
						b1->velocity.y = ty * dpTan1 + ny * m1;
					}

					if (b2->isStatic)
					{
						b2->velocity.x *= -1;
						b2->velocity.y *= -1;
					}
					else
					{
						b2->velocity.x = tx * dpTan2 + nx * m2;
						b2->velocity.y = ty * dpTan2 + ny * m2;
					}
					*/
					
				}

				// Remove collisions
				vecCollidingPairs.clear();

				// Remove fake balls
				for (auto& b : vecFakeBalls) delete b;
				vecFakeBalls.clear();
			}
		}
	}


		
	inline bool Jphysics::DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= ((r1 + r2) * (r1 + r2));
	}
		

	inline bool Jphysics::IsPointInCircle(float x1, float y1, float r1, float px, float py)
	{
		return fabs((x1 - px) * (x1 - px) + (y1 - py) * (y1 - py)) < (r1 * r1);
	}





		//statics
		std::atomic<bool> Jphysics::bAtomActive{ false };
}

