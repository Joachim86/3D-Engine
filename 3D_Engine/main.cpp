#define OLC_PGE_APPLICATION
#include "engine.h"

#include "camera.h"
#include "renderer.h"
#include "object/construct.h"
#include "J_physics.h"



class Engine3D : public olc::PixelGameEngine
{
public:
	Engine3D()
	{
		sAppName = "3D Demo";
	}


private:
	Camera*			Camera1;
	Renderer*		Renderer1;
	jph::Jphysics*	physicsEngine;

	float fieldOfView = 90;

	Object* Ball;
	Object* Cube;
	Object* Plane;
	Object* Font;
	Object* Pong;
	Object* Pong2;

	uint32_t playerScore = 0;
	uint32_t aiScore = 0;


	enum
	{
		GS_LOADING,
		GS_TITLE,
		GS_PREPARE,
		GS_MAIN,
		GS_COMPLETE,
	} nGameState = GS_LOADING;


	void movement(float fElapsedTime);


public:
	bool OnUserCreate() override
	{
		return true;
	}



	bool OnUserUpdate(float fElapsedTime) override
	{

		switch (nGameState)
		{
		case GS_LOADING: GameState_Loading(fElapsedTime); break;
		case GS_TITLE: GameState_Title(fElapsedTime); break;
		case GS_PREPARE: GameState_Prepare(fElapsedTime); break;
		case GS_MAIN: GameState_Main(fElapsedTime); break;
		case GS_COMPLETE: GameState_Complete(fElapsedTime); break;
		}

		return true;

	}



	bool GameState_Loading(float fElapsedTime)
	{
		Object::spriteContainer.push_back(new olc::Sprite);
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/cursor.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/Pong.png"));

		Ball =	new Object("ball", "Ressources/ball3.obj", (uint8_t)0, olc::vec3d{ 0, 0, 0 }, olc::GREEN);
		Cube =	new Object("Cube", "Ressources/cube_2.obj", (uint8_t)0, olc::vec3d{ 0, 0, 0 }, olc::GREEN);
		Plane = new Object("Plane", "Ressources/PONG_plane2.obj", (uint8_t)2, olc::vec3d{ 12.5, -1, 12.5 }, olc::MAGENTA);
		Font =	new Object("Font", "Ressources/Font.obj", (uint8_t)0, olc::vec3d{ 12.5, 2, 12.5 + 25 }, olc::DARK_GREEN);
		Pong = new Object("Pong", "Ressources/pong_player.obj", (uint8_t)0, olc::vec3d{ 12.5, 2, 12.5 + 25 }, olc::RED);
		Pong2 = new Object("Pong", "Ressources/pong_player.obj", (uint8_t)0, olc::vec3d{ 12.5, 2, 12.5 + 25 }, olc::BLUE);
		
		//Create camera
		Camera1 = new Camera(this);
		
		//Create renderer
		Renderer1 = new Renderer(this, Camera1);

		//Set Mouse up
		SetMouseCenter(true);

		//Set Pixel Mode, may be costly
		SetPixelMode(olc::Pixel::ALPHA);

		//Create physics engine
		physicsEngine = new jph::Jphysics;


		//Adjust Font for Title
		Object::objectVector.push_back(new Object(*Font));
		Object::objectVector[0]->objectCoordinates = olc::vec3d{ 0, 0, 0 };
		Object::objectVector[0]->setRotationX(olc::PI / 2);
		Camera1->setCameraPosition(olc::vec3d{ 14, -0.8, -4 }); // 14, -0.8, -4


		nGameState = GS_TITLE;
		return true;
	}



	bool GameState_Title(float fElapsedTime)
	{
		//Exit Game
		if (GetKey(olc::ESCAPE).bHeld)
		{
			nGameState = GS_COMPLETE;
		}

		//Fast Forward
		if (GetKey(olc::SPACE).bHeld || GetKey(olc::ENTER).bHeld)
		{
			Object::objectVector.erase(Object::objectVector.begin());

			Camera1->setCameraPosition(olc::vec3d{ 0, 16, 25 }); //-6 25 25
			Camera1->fYaw = olc::PI * 1.5f;
			Camera1->fPitch = 1.2f;

			nGameState = GS_PREPARE;
		}

		if (Camera1->vCamera.x <= -14)
		{
			Object::objectVector.erase(Object::objectVector.begin());

			Camera1->setCameraPosition(olc::vec3d{ 0, 16, 25 }); //-6 25 25
			Camera1->fYaw = olc::PI * 1.5f;
			Camera1->fPitch = 1.2f;

			nGameState = GS_PREPARE;
		}
		else
		{
			Camera1->vCamera.x -= 2.5f * fElapsedTime;
		}

		/*
		if (GetKey(olc::NP4).bHeld)
			Object::objectVector[0]->objectCoordinates.x += 2 * fElapsedTime;
		if (GetKey(olc::NP6).bHeld)
			Object::objectVector[0]->objectCoordinates.x += -2 * fElapsedTime;
		if (GetKey(olc::NP8).bHeld)
			Object::objectVector[0]->objectCoordinates.z += 2 * fElapsedTime;
		if (GetKey(olc::NP2).bHeld)
			Object::objectVector[0]->objectCoordinates.z += -2 * fElapsedTime;
		*/

		//movement(fElapsedTime);
		//std::cout << "Kamera XYZ: " << Camera1->vCamera.x << " " << Camera1->vCamera.y << " " << Camera1->vCamera.z << "\n";

		// Clear the background
		Clear(olc::BLACK);

		// Update Camera
		Camera1->update(fElapsedTime);

		// Render Objects
		Renderer1->update();

		// Draw Cursor (Always render last)
		DrawSprite(GetMouseX(), GetMouseY(), Object::spriteContainer[1], 1);
		
		return true;
	}


	bool GameState_Prepare(float fElapsedTime)
	{
		Object::objectVector.push_back(new Object(*Ball));

		Plane->objectCoordinates = olc::vec3d{ 12.5, -1, 25 };
		Object::objectVector.push_back(new Object(*Plane));

		Pong->objectCoordinates = olc::vec3d{ 12.5, 0, 0 };
		Pong->setRotationY(olc::PI / 2);
		Object::objectVector.push_back(new Object(*Pong));

		Pong2->objectCoordinates = olc::vec3d{ 12.5, 0, 50 };
		Pong2->setRotationY(olc::PI / 2);
		Object::objectVector.push_back(new Object(*Pong2));





		//Remember that the physics engine is only in 2d
		//The default orientation is looking down so y and z co ordinates
		//correlate to x and y coordinates in the physics engine

		//Start physics engine
		physicsEngine->Start();

		

		// left player
		physicsEngine->addLine(jph::jVector2D{ Object::objectVector[2]->objectCoordinates.x - 3, 0 }, jph::jVector2D{ Object::objectVector[2]->objectCoordinates.x + 3, 0 }, 1);

		// right ai
		physicsEngine->addLine(jph::jVector2D{ Object::objectVector[3]->objectCoordinates.x - 3, 50 }, jph::jVector2D{ Object::objectVector[3]->objectCoordinates.x + 3, 50 }, 1);


		//physicsEngine->addLine(jph::jVector2D{ 0, 0 }, jph::jVector2D{ 25, 0 }, 0);	// Links
		//physicsEngine->addLine(jph::jVector2D{ 0, 50 }, jph::jVector2D{ 25, 50 }, 0);	// Rechts

		physicsEngine->addLine(jph::jVector2D{ 0, 0 }, jph::jVector2D{ 0, 50 }, 0);		// Unten
		physicsEngine->addLine(jph::jVector2D{ 25, 0 }, jph::jVector2D{ 25, 50 }, 0);	// Oben

		physicsEngine->addCircle(jph::jVector2D{ 12.5, 25 }, 0.5f, jph::jVector2D{ 0, -15 }, jph::jVector2D{ 0, 0 }, 1, 0, 1.01);



		nGameState = GS_MAIN;

		return true;
	}


	bool GameState_Main(float fElapsedTime)
	{
		// They have to be called because they return the mouse-delta since last call
		int mouseX = GetMouseDeltaX();
		int mouseY = GetMouseDeltaY();


		//Exit Game
		if (GetKey(olc::ESCAPE).bHeld)
		{
			nGameState = GS_COMPLETE;
		}


		// Update movement
		/*
		movement(fElapsedTime);
		Camera1->rotateWithMouse(mouseX, mouseY, fElapsedTime);
		*/


		if (GetKey(olc::UP).bHeld)
		{
			physicsEngine->vecLines[0].start.x += 4 * fElapsedTime;
			physicsEngine->vecLines[0].end.x += 4 * fElapsedTime;

			physicsEngine->vecLines[1].start.x += 4 * fElapsedTime;
			physicsEngine->vecLines[1].end.x += 4 * fElapsedTime;
		}
			
		if (GetKey(olc::DOWN).bHeld)
		{
			physicsEngine->vecLines[0].start.x -= 4 * fElapsedTime;
			physicsEngine->vecLines[0].end.x -= 4 * fElapsedTime;

			physicsEngine->vecLines[1].start.x -= 4 * fElapsedTime;
			physicsEngine->vecLines[1].end.x -= 4 * fElapsedTime;
		}
			
		

		// Update physics Engine
		physicsEngine->update(fElapsedTime);
		Object::objectVector[0]->setCoordinates(olc::vec3d{ physicsEngine->vecCircles[0].position.x, 0, physicsEngine->vecCircles[0].position.y });
		Object::objectVector[2]->setCoordinates(olc::vec3d{ physicsEngine->vecLines[0].start.x + 3, Object::objectVector[2]->objectCoordinates.y, Object::objectVector[2]->objectCoordinates.z });
		Object::objectVector[3]->setCoordinates(olc::vec3d{ physicsEngine->vecLines[1].start.x + 3, Object::objectVector[3]->objectCoordinates.y, Object::objectVector[3]->objectCoordinates.z });
		

		// Game rules
		if (physicsEngine->vecCircles[0].position.y <= 0)
		{
			aiScore++;

			physicsEngine->vecCircles[0].position.x = 12.5;
			physicsEngine->vecCircles[0].position.y = 25;
		}
		else if (physicsEngine->vecCircles[0].position.y >= 50)
		{
			playerScore++;

			physicsEngine->vecCircles[0].position.x = 12.5;
			physicsEngine->vecCircles[0].position.y = 25;
		}


		// Clear the background
		Clear(olc::BLACK);

		// Update Camera
		Camera1->update(fElapsedTime);

		// Render Objects
		Renderer1->update();

		
		uint8_t textScale = 4;
		std::string playerText = "PLAYER:" + std::to_string(playerScore);
		std::string aiText = "AI:" + std::to_string(aiScore);

		DrawString(0, ScreenHeight() - textScale * 8, playerText, olc::WHITE, textScale);
		DrawString(ScreenWidth() - textScale * 8 * aiText.size(), ScreenHeight() - textScale * 8, aiText, olc::WHITE, textScale);


		return true;
	}


	bool GameState_Complete(float fElapsedTime)
	{
		this->Close(); // Closes the Window: sets "bAtomActive" false
		return true;
	}

}; //ende



int main()
{
	Engine3D demo;

	
	ShowCursor(false);
	if (demo.Construct(960, 540, 1, 1, true))
		demo.Start();
	

	/*
	ShowCursor(false);
	if (demo.Construct(1920, 1080, 1, 1, true))
		demo.Start();
	*/
		
	return 0;
}

void Engine3D::movement(float fElapsedTime)
{
	if (GetKey(olc::SPACE).bHeld) // Upwards
	{
		Camera1->moveCamera(olc::vec3d{ 0, 8.0f, 0, 1 }, fElapsedTime);
	}

	if (GetKey(olc::SHIFT).bHeld) // Downwards
	{
		Camera1->moveCamera(olc::vec3d{ 0, -8.0f, 0, 1 }, fElapsedTime);
	}

	if (GetKey(olc::W).bHeld) // Forwards
	{
		Camera1->moveForward(8.0f, fElapsedTime, true);
	}

	if (GetKey(olc::S).bHeld) // Backwards
	{
		Camera1->moveForward(-8.0f, fElapsedTime, true);
	}

	if (GetKey(olc::A).bHeld) // Left
	{
		Camera1->moveSide(8.0f, fElapsedTime);
	}

	if (GetKey(olc::D).bHeld) // Right
	{
		Camera1->moveSide(-8.0f, fElapsedTime);
	}

	if (GetKey(olc::LEFT).bHeld) // Rotate Left
	{
		//Camera1->rotateLeft(3.0f, fElapsedTime);
		//physicsEngine.vecCircles[1].velocity.x += (20.0f * fElapsedTime);
	}

	if (GetKey(olc::RIGHT).bHeld) // Rotate Right
	{
		//Camera1->rotateRight(-3.0f, fElapsedTime);
		//physicsEngine.vecCircles[1].velocity.x -= (20.0f * fElapsedTime);
	}

	if (GetKey(olc::UP).bHeld) // Rotate Up
	{
		//Camera1->rotateUp(2.0f, fElapsedTime);
	}

	if (GetKey(olc::DOWN).bHeld) // Rotate Down
	{
		//Camera1->rotateDown(-2.0f, fElapsedTime);
	}

	if (GetKey(olc::Q).bHeld) // Increase FOV
	{
		fieldOfView += 20 * fElapsedTime; Camera1->setFieldOfView(fieldOfView);
	}

	if (GetKey(olc::E).bHeld) // Decrease FOV
	{
		fieldOfView -= 20 * fElapsedTime; Camera1->setFieldOfView(fieldOfView);
	}

	
	if (GetKey(olc::ENTER).bReleased) //Set Mouse Center
	{
		//physicsEngine.worldGravity *=  -1;
		//SetMouseCenter(false);
		physicsEngine->removeObjectByID(0);
	}
}