#define OLC_PGE_APPLICATION
#include "engine.h"

#include "camera.h"
#include "renderer.h"



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
		Object::spriteContainer.push_back(new olc::Sprite);
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/texture_1.png"));
		Object::spriteContainer.push_back(new olc::Sprite("Ressources/street_1.png"));



		Object::objectVector.push_back(new Object("object 1", "Ressources/cube_2.obj", Object::spriteContainer[1], (uint8_t)1, olc::vec3d{ 0, 0, 0 }));
		Object::objectVector.push_back(new Object("object 1", "Ressources/cube_2.obj", Object::spriteContainer[2], (uint8_t)2, olc::vec3d{ 5, 0, 0 }));


		//Create camera
		Camera1 = new Camera(this);
		Camera1->setCameraPosition(olc::vec3d{ 0, 0, -5, 1 });


		//Create renderer
		Renderer1 = new Renderer(this, Camera1);

		//Set Mouse up
		SetMouseCenter(true);

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
		if (GetKey(olc::SPACE).bHeld)
			Camera1->moveCamera(olc::vec3d{ 0, 8.0f, 0, 1 }, fElapsedTime); // Upwards

		if (GetKey(olc::SHIFT).bHeld)
			Camera1->moveCamera(olc::vec3d{ 0, -8.0f, 0, 1 }, fElapsedTime); // Downwards

		if (GetKey(olc::W).bHeld) // Forwards
			Camera1->moveForward(8.0f, fElapsedTime, true);

		if (GetKey(olc::S).bHeld) // Backwards
			Camera1->moveForward(-8.0f, fElapsedTime, true);

		if (GetKey(olc::A).bHeld) // Left
			Camera1->moveSide(8.0f, fElapsedTime);

		if (GetKey(olc::D).bHeld) // Right
			Camera1->moveSide(-8.0f, fElapsedTime);

		if (GetKey(olc::LEFT).bHeld) // Rotate Left
			Camera1->rotateLeft(2.0f, fElapsedTime);

		if (GetKey(olc::RIGHT).bHeld) // Rotate Right -2.0f is default
			Camera1->rotateRight(-2.0f, fElapsedTime);

		if (GetKey(olc::UP).bHeld) // Rotate Up
			Camera1->rotateUp(2.0f, fElapsedTime);

		if (GetKey(olc::DOWN).bHeld) // Rotate Down
			Camera1->rotateDown(-2.0f, fElapsedTime);

		if (GetKey(olc::ESCAPE).bHeld) //Exit Game
		{
			//nGameState = GS_COMPLETE;
			SetMouseCenter(false);
		}
			

		// They have to be called because the return the delta since last call
		int mouseX = GetMouseDeltaX();
		int mouseY = GetMouseDeltaY();
		Camera1->rotateWithMouse(mouseX, mouseY, fElapsedTime);



		if (GetKey(olc::ENTER).bHeld) //Delete Object (test)
		{
			//Object::deleteObject(5);


			//SetMouseVisibility(false);
			
			//std::cout << "Yaw: " << Camera1->fYaw << "\n";
		}




		
		
		Clear(olc::CYAN);

		Camera1->update(fElapsedTime);

		Renderer1->update();

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


	if (demo.Construct(960, 540, 1, 1, false))
		demo.Start();

	return 0;
}