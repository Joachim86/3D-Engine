#pragma once

#include "engine.h"
#include "camera.h"
#include "object/object.h"
#include "object/staticObject.h"

#define DEBUG_MODE false

class Renderer
{
public:
	Renderer(olc::PixelGameEngine* enginePointer_, Camera* cameraPointer_);

	void update();

	bool NormalizeTriangle(olc::mesh& meshObject, olc::mat4x4& matObject);

	void TexturedTriangle(	int x1, int y1, float u1, float v1, float w1,
							int x2, int y2, float u2, float v2, float w2,
							int x3, int y3, float u3, float v3, float w3,
							olc::Sprite* tex);

	void colouredTriangle(	int x1, int y1, float w1,
							int x2, int y2, float w2,
							int x3, int y3, float w3,
							olc::Pixel _colour);

	void renderTriangles();

private:
	Camera* cameraPointer;

	olc::PixelGameEngine* enginePointer;

	float* pDepthBuffer = nullptr;
};

Renderer::Renderer(olc::PixelGameEngine* enginePointer_, Camera* cameraPointer_)
{
	enginePointer = enginePointer_;
	cameraPointer = cameraPointer_;

	pDepthBuffer = new float[enginePointer->ScreenWidth() * enginePointer->ScreenHeight()];
}


void Renderer::update()
{
	// Clear Depth Buffer
	for (int i = 0; i < enginePointer->ScreenWidth() * enginePointer->ScreenHeight(); i++)
	{
		pDepthBuffer[i] = 0.0f;
	}

	// Clear triangle buffer
	Object::vecTrianglesToRaster.clear();

	for (auto& object_ : Object::objectVector)
	{
		object_->updateRotation();
		object_->update();
		NormalizeTriangle(object_->objectMesh, object_->objectMatrix);
	}

	renderTriangles();
}


bool Renderer::NormalizeTriangle(olc::mesh& meshObject, olc::mat4x4& matObject)
{
	// Normalize Triangles
	for (auto tri : meshObject.tris)
	{

		olc::triangle triProjected, triTransformed, triViewed;

		// World Matrix Transform
		triTransformed.p[0] = Matrix_MultiplyVector(matObject, tri.p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matObject, tri.p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matObject, tri.p[2]);
		triTransformed.t[0] = tri.t[0];
		triTransformed.t[1] = tri.t[1];
		triTransformed.t[2] = tri.t[2];

		// Calculate triangle Normal
		olc::vec3d normal, line1, line2;

		// Get lines either side of triangle
		line1 = olc::Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = olc::Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		// You normally need to normalise a normal!
		normal = Vector_Normalise(normal);

		// Get Ray from triangle to camera
		olc::vec3d vCameraRay = olc::Vector_Sub(triTransformed.p[0], cameraPointer->vCamera);

		// If ray is aligned with normal, then triangle is visible
		if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Illumination

			olc::vec3d light_direction = { 0.0f, 0.7f, -1.0f };
			light_direction = Vector_Normalise(light_direction);

			// How "aligned" are light direction and triangle surface normal?
			float dp = std::max(0.1f, Vector_DotProduct(light_direction, normal));

			// Choose console colours as required (much easier with RGB)
			// dp gives a nuber between 0 and 1
			// we need to convert this number to RGB values (0 - 255);
			//triTransformed.colour = { (uint8_t)(dp * 255.0f), (uint8_t)(dp * 255.0f), (uint8_t)(dp * 255.0f) };
			triTransformed.colour = olc::BLUE;

			// Convert World Space --> View Space
			triViewed.p[0] = Matrix_MultiplyVector(cameraPointer->matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(cameraPointer->matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(cameraPointer->matView, triTransformed.p[2]);
			triViewed.colour = triTransformed.colour;
			triViewed.t[0] = triTransformed.t[0];
			triViewed.t[1] = triTransformed.t[1];
			triViewed.t[2] = triTransformed.t[2];

			// Clip Viewed Triangle against near plane, this could form two additional
			// additional triangles. 
			int nClippedTriangles = 0;
			olc::triangle clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

			// We may end up with multiple triangles form the clip, so project as
			// required
			for (int n = 0; n < nClippedTriangles; n++)
			{
				// Project triangles from 3D --> 2D
				triProjected.p[0] = Matrix_MultiplyVector(cameraPointer->matProj, clipped[n].p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(cameraPointer->matProj, clipped[n].p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(cameraPointer->matProj, clipped[n].p[2]);
				triProjected.colour = clipped[n].colour;
				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];


				// Correct UV-Coordinates
				triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
				triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
				triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

				triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
				triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
				triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;
				

				triProjected.t[0].w = 1.0f / triProjected.p[0].w;
				triProjected.t[1].w = 1.0f / triProjected.p[1].w;
				triProjected.t[2].w = 1.0f / triProjected.p[2].w;


				// Scale into view, we moved the normalising into cartesian space
				// out of the matrix.vector function from the previous videos, so
				// do this manually
				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.0f;
				triProjected.p[1].x *= -1.0f;
				triProjected.p[2].x *= -1.0f;
				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;

				// Offset verts into visible normalised space
				olc::vec3d vOffsetView = { 1,1,0 };
				triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
				triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
				triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);	  //There was float casting before enginePointer->ScreenWidth
				triProjected.p[0].x *= 0.5f * (float)enginePointer->ScreenWidth();		  //(float)
				triProjected.p[0].y *= 0.5f * (float)enginePointer->ScreenHeight();	  //(float)
				triProjected.p[1].x *= 0.5f * (float)enginePointer->ScreenWidth();		  //(float)
				triProjected.p[1].y *= 0.5f * (float)enginePointer->ScreenHeight();	  //(float)
				triProjected.p[2].x *= 0.5f * (float)enginePointer->ScreenWidth();		  //(float)
				triProjected.p[2].y *= 0.5f * (float)enginePointer->ScreenHeight();	  //(float)

				// Copy texture info
				// Not perfect because it copies the information from the mesh rather than the triangle itself
				triProjected.textureCodeTri = meshObject.textureCodeMesh;

				// Copy colour info
				triProjected.colour = tri.colour;


				// Store triangle for sorting
				Object::vecTrianglesToRaster.push_back(triProjected);
			}
		}
	}


	return true;
}




void Renderer::TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	olc::Sprite* tex)
{

	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
		std::swap(w1, w2);
	}

	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(u1, u3);
		std::swap(v1, v3);
		std::swap(w1, w3);
	}

	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(u2, u3);
		std::swap(v2, v3);
		std::swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * enginePointer->ScreenWidth() + j])
				{
					enginePointer->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * enginePointer->ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * enginePointer->ScreenWidth() + j])
				{
					enginePointer->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * enginePointer->ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}
		}
	}

}


void Renderer::colouredTriangle(int x1, int y1, float w1,
	int x2, int y2, float w2,
	int x3, int y3, float w3,
	olc::Pixel _colour)
{

	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(w1, w2);
	}

	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(w1, w3);
	}

	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_sw, tex_ew);
			}

			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * enginePointer->ScreenWidth() + j])
				{
					enginePointer->Draw(j, i, _colour);
					pDepthBuffer[i * enginePointer->ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_sw, tex_ew);
			}

			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * enginePointer->ScreenWidth() + j])
				{
					enginePointer->Draw(j, i, _colour);
					pDepthBuffer[i * enginePointer->ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}
		}
	}

}



void Renderer::renderTriangles()
{
	
	// Loop through all transformed, viewed, projected, and sorted triangles
	for (auto& triToRaster : Object::vecTrianglesToRaster)
	{
		// Clip triangles against all four screen edges
		olc::triangle clipped[2];
		std::list<olc::triangle> listTriangles;

		// Add initial triangle
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				// Take triangle from front of queue
				olc::triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				// Clip it against a plane.
				switch (p)
				{
				case 0:	nTrisToAdd = olc::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = olc::Triangle_ClipAgainstPlane({ 0.0f, (float)enginePointer->ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = olc::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = olc::Triangle_ClipAgainstPlane({ (float)enginePointer->ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}




				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++)
				{
					// copy texture info
					clipped[w].textureCodeTri = test.textureCodeTri;

					listTriangles.push_back(clipped[w]);
				}

			}
			nNewTriangles = listTriangles.size();
		}


		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto& t : listTriangles)
		{
			if (t.textureCodeTri)
			{
				TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
					t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
					t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w, Object::spriteContainer[t.textureCodeTri]);
			}
			else
			{
				// Just for debugging
				// no depth buffer and no sorting of triangles...
				//enginePointer->FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, olc::YELLOW);
				colouredTriangle(t.p[0].x, t.p[0].y, t.t[0].w,
					t.p[1].x, t.p[1].y, t.t[1].w,
					t.p[2].x, t.p[2].y, t.t[2].w,
					t.colour);
			}
			


			if (DEBUG_MODE)
			{
				enginePointer->DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, olc::WHITE);
			}
		}
	}
}
