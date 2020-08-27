#define OLC_PGE_APPLICATION
#include "engine.h"

#include "camera.h"
#include "renderer.h"
#include "object/construct.h"



class Engine3D : public olc::PixelGameEngine
{
public:
	Engine3D()
	{
		sAppName = "3D Demo";
	}


private:
	Camera* Camera1;
	Renderer* Renderer1;

	float fieldOfView = 90;

	// Testing
	//obj::Construct* construct1;


	enum
	{
		GS_LOADING,
		GS_TITLE,
		GS_MAIN,
		GS_COMPLETE,
	} nGameState = GS_LOADING;







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
		case GS_MAIN: GameState_Main(fElapsedTime); break;
		case GS_COMPLETE: GameState_Complete(fElapsedTime); break;
		}

		return true;

	}



	bool GameState_Loading(float fElapsedTime)
	{
		Object::spriteContainer.push_back(new olc::Sprite);
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/texture_1.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/texture_2.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/gun.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/Haus1.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/Gras.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/Fadenkreuz.png"));


		Object::objectVector.push_back(new Object("object 1", "Ressources/cube_2.obj", Object::spriteContainer[1], (uint8_t)1, olc::vec3d{ 0, 0, 0 }));
		Object::objectVector.push_back(new Object("object 2", "Ressources/cube_2.obj", Object::spriteContainer[2], (uint8_t)2, olc::vec3d{ 5, 0, 0 }));

		Object::objectVector.push_back(new Object("object 3", "Ressources/gun.obj", Object::spriteContainer[3], (uint8_t)3, olc::vec3d{ 5, 0, 2 }));
		Object::objectVector[2]->rotateMeshY((3.1415926535 / 360) * (160));
		Object::objectVector[2]->changeMeshCenter(-0.8f, -0.2f, 1.5f);

		Object::objectVector.push_back(new Object("Haus 1", "Ressources/Haus1.obj", Object::spriteContainer[4], (uint8_t)4, olc::vec3d{ 10, 0, 15 }));

		Object::objectVector.push_back(new Object("Haus 2", "Ressources/Haus1.obj", Object::spriteContainer[4], (uint8_t)4, olc::vec3d{ -10, 0, 15 }));
		Object::objectVector[4]->rotateMeshY((3.1415926535 / 360) * (180));

		Object::objectVector.push_back(new Object("Haus 3", "Ressources/Haus1.obj", Object::spriteContainer[4], (uint8_t)4, olc::vec3d{ -30, 0, 15 }));

		Object::objectVector.push_back(new Object("Gras 1", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ -32, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 2", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ -16, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 3", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ -32, 0, 16 }));
		Object::objectVector.push_back(new Object("Gras 4", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ -16, 0, 16 }));

		Object::objectVector.push_back(new Object("Gras 5", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 0, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 6", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 16, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 7", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 0, 0, 16 }));
		Object::objectVector.push_back(new Object("Gras 8", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 16, 0, 16 }));

		Object::objectVector.push_back(new Object("Gras 9", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 16, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 10", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 32, 0, 0 }));
		Object::objectVector.push_back(new Object("Gras 11", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 16, 0, 16 }));
		Object::objectVector.push_back(new Object("Gras 12", "Ressources/Gras.obj", Object::spriteContainer[5], (uint8_t)5, olc::vec3d{ 32, 0, 16 }));


		//Create camera
		Camera1 = new Camera(this);
		Camera1->setCameraPosition(olc::vec3d{ 0, 0, -5, 1 });


		//Create renderer
		Renderer1 = new Renderer(this, Camera1);



		//Set Mouse up
		SetMouseCenter(false);


		//Set Pixel Mode, may be costly
		SetPixelMode(olc::Pixel::ALPHA);





		nGameState = GS_TITLE;
		return true;
	}



	bool GameState_Title(float fElapsedTime)
	{
		nGameState = GS_MAIN;
		return true;
	}



	bool GameState_Main(float fElapsedTime)
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
			Camera1->rotateLeft(3.0f, fElapsedTime);
		}

		if (GetKey(olc::RIGHT).bHeld) // Rotate Right -2.0f is default
		{
			Camera1->rotateRight(-3.0f, fElapsedTime);
		}

		if (GetKey(olc::UP).bHeld) // Rotate Up
		{
			Camera1->rotateUp(2.0f, fElapsedTime);
		}

		if (GetKey(olc::DOWN).bHeld) // Rotate Down
		{
			Camera1->rotateDown(-2.0f, fElapsedTime);
		}

		if (GetKey(olc::Q).bHeld) // Increase FOV
		{
			fieldOfView += 20 * fElapsedTime; Camera1->setFieldOfView(fieldOfView);
		}

		if (GetKey(olc::E).bHeld) // Decrease FOV
		{
			fieldOfView -= 20 * fElapsedTime; Camera1->setFieldOfView(fieldOfView);
		}

		if (GetKey(olc::ESCAPE).bHeld) //Exit Game
		{
			nGameState = GS_COMPLETE;
			SetMouseCenter(false);
		}

		if (GetKey(olc::ENTER).bHeld) //Set Mouse Center
		{
			SetMouseCenter(true);
		}


		// Gun movement
		
		Object::objectVector[2]->objectCoordinates.x = Camera1->vCamera.x;
		Object::objectVector[2]->objectCoordinates.y = Camera1->vCamera.y;
		Object::objectVector[2]->objectCoordinates.z = Camera1->vCamera.z;

		Object::objectVector[2]->setRotationY(Camera1->fYaw);
		Object::objectVector[2]->setRotationX(Camera1->fPitch);
		Object::objectVector[2]->updateRotation();
		
		
		

		// They have to be called because they return the mouse-delta since last call
		int mouseX = GetMouseDeltaX();
		int mouseY = GetMouseDeltaY();
		Camera1->rotateWithMouse(mouseX, mouseY, fElapsedTime);



		// Testing
		//construct1->update();

		
		
		Clear(olc::CYAN);

		Camera1->update(fElapsedTime);

		Renderer1->update();



		// Draw Center Crosshair (Always render last)
		DrawSprite(ScreenWidth() / 2 - Object::spriteContainer[6]->width / 2, ScreenHeight() / 2 - Object::spriteContainer[6]->height / 2, Object::spriteContainer[6], 1);



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

	//ShowCursor(NULL);

	if (demo.Construct(960, 540, 1, 1, false))
		demo.Start();

	return 0;
}