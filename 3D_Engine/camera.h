#pragma once

#include "engine.h"



class Camera
{
public:
	Camera(olc::PixelGameEngine* _olcEnginePointer, float fieldOfView = 90.0f)
	{
		olcEnginePointer = _olcEnginePointer;

		// Projection Matrix
		matProj = olc::Matrix_MakeProjection(fieldOfView, (float)olcEnginePointer->ScreenHeight() / (float)olcEnginePointer->ScreenWidth(), 0.1f, 1000.0f);

		fCameraRotation = 0;
		setCameraPosition(olc::vec3d{ 0, 0, 0, 0 });
	}

	void setFieldOfView(float angle)
	{
		matProj = olc::Matrix_MakeProjection(angle, (float)olcEnginePointer->ScreenHeight() / (float)olcEnginePointer->ScreenWidth(), 0.1f, 1000.0f);
	}

	void update(float fElapsedTime) // update every frame; 
	{
		// Create "Point At" Matrix for camera
		olc::vec3d vUp = { 0,1,0 };
		olc::vec3d vTarget = { 0,0,1 };


		
		olc::mat4x4 matCameraRotX = olc::Matrix_MakeRotationX(fPitch);
		olc::mat4x4 matCameraRotY = olc::Matrix_MakeRotationY(fYaw);
		olc::mat4x4 matCameraRotZ = olc::Matrix_MakeRotationZ(fRoll);


		olc::mat4x4 matCameraRot = olc::Matrix_MultiplyMatrix(matCameraRotX, matCameraRotY);


		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);

		vTarget = olc::Vector_Add(vCamera, vLookDir);
		olc::mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);



		// Make view matrix from camera
		matView = Matrix_QuickInverse(matCamera);

		/*
		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
				std::cout << matView.m[i][j] << "|";
		std::cout << "\n";
		*/
	}

	void setCameraPosition(olc::vec3d position) // set camera poistion
	{
		vCamera = position;
	}

	void moveCamera(olc::vec3d offset, float fElapsedTime) // just moves the camera in desired direction
	{
		vCamera.x += offset.x * fElapsedTime;
		vCamera.y += offset.y * fElapsedTime;
		vCamera.z += offset.z * fElapsedTime;
	}

	void moveForward(float offset, float fElapsedTime, bool noYdirection = false) // moves whith view direction in mind; backwards is just negative forward
	{
		if (noYdirection)
		{
			//This is to compute just the z and x axis without y (up/down)
			olc::vec3d vTarget = { 0,0,1 };
			olc::mat4x4 matCameraRotY2 = olc::Matrix_MakeRotationY(fYaw);
			olc::vec3d vLookDir2 = olc::Matrix_MultiplyVector(matCameraRotY2, vTarget);


			vForward = Vector_Mul(vLookDir2, offset * fElapsedTime);
		}
		else
		{
			vForward = olc::Vector_Mul(vLookDir, offset * fElapsedTime);
		}


		vCamera = olc::Vector_Add(vCamera, vForward);
	}

	void moveSide(float offset, float fElapsedTime) // Left is +, Right is -
	{
		olc::vec3d vTarget = { 0,0,1 };
		olc::mat4x4 matCameraRotY2 = olc::Matrix_MakeRotationY(fYaw);
		olc::vec3d vLookDir2 = olc::Matrix_MultiplyVector(matCameraRotY2, vTarget);

		vSide.z = -vLookDir2.x;
		vSide.x = vLookDir2.z;
		vSide.z *= offset * fElapsedTime;
		vSide.x *= offset * fElapsedTime;
		vCamera = olc::Vector_Add(vCamera, vSide);
	}

	void rotateLeft(float offset, float fElapsedTime)
	{
		offset = fabs(offset);

		if (fYaw <= 0)
			fYaw = (olc::PI * offset) - fYaw;

		fYaw -= offset * fElapsedTime;
	}

	void rotateRight(float offset, float fElapsedTime)
	{
		offset = fabs(offset);

		if (fYaw >= olc::PI * offset)
			fYaw -= olc::PI * offset;

		fYaw += offset * fElapsedTime;
	}

	void rotateWithMouse(float offsetX, float offsetY, float fElapsedTime)
	{
		//std::cout << "Yaw: " << fYaw << "\n";

		fYaw += offsetX * fElapsedTime;
		fPitch += offsetY * fElapsedTime;
	}

	void rotateUp(float offset, float fElapsedTime)
	{
		offset = fabs(offset);

		if (fPitch > (-olc::PI / 2) + offset * fElapsedTime)
			fPitch -= offset * fElapsedTime;
	}

	void rotateDown(float offset, float fElapsedTime)
	{
		offset = fabs(offset);

		if (fPitch <= (olc::PI / 2) - 2 * fElapsedTime)
			fPitch += offset * fElapsedTime;
	}

	void tiltLeft(float offset, float fElapsedTime)
	{
		offset = fabs(offset);
		/*
		if (fRoll > (-olc::PI / 2) + offset * fElapsedTime)
			fRoll -= offset * fElapsedTime;
		*/
		fRoll -= offset * fElapsedTime;
	}

	void tiltRight(float offset, float fElapsedTime)
	{
		offset = fabs(offset);
		/*
		if (fRoll <= (olc::PI / 2) - 2 * fElapsedTime)
			fRoll += offset * fElapsedTime;
		*/
		fRoll += offset * fElapsedTime;
	}


	olc::mat4x4 matProj;		// Matrix that converts from view space to screen space
	olc::mat4x4 matView;

	olc::vec3d vCamera;			// Location of camera in world space
	olc::vec3d vLookDir;		// Direction vector along the direction camera points
	olc::vec3d vForward;
	olc::vec3d vSide;

	float fYaw;				// FPS Camera rotation in XZ plane
	float fPitch;			// FPS Camera rotation in XY plane
	float fRoll;

	float fTheta;			// Spins World transform
	float fCameraRotation;	// Camera rotation in radiant

private:
	olc::PixelGameEngine* olcEnginePointer;

};