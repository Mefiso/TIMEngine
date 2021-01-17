#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"
#include "MSTimer.h"
#include "Geometry/LineSegment.h"

class Model;
class GameObject;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	void* context = nullptr;										// Context of the SDL_GL configuration

	bool depthTest	= true;											// Set if depth test is performed
	bool cullFace	= true;											// Set if face culling is performed
	bool showGrid	= true;											// Set if the grid is rendered
	bool showOctree	= false;										// Set if the octree from scene is rendered

	float3 backgroundColor	= { 0.1f, 0.1f, 0.1f };					// Base color of the viewport window
	float3 gridColor		= { 1.0f, 1.0f, 1.0f };					// Base color of the world Grid

private:
	int viewport_width = 0, viewport_height = 0;					// Initial size of Viewport window
	unsigned int FBO = 0u, textureColorbuffer = 0u, RBO = 0u;		// IDs of the Viewport buffer objects and texture
	std::vector<GameObject*> objectsToDraw;							// Objects that pass the frustum culling test

public:
	ModuleRender();													// Constructor
	~ModuleRender();												// Destructor

	//  ----- Module Functions ----- //
	bool			Init() override;								// Initialise Module
	update_status	PreUpdate() override;							// Operations that must be performed just before each frame
	update_status	Update() override;								// Operations performed at each frame
	update_status	PostUpdate() override;							// Operations that must be performed just after each frame
	bool			CleanUp() override;								// Clean memory allocated by this Module

	void PerformFrustumCulling(const float4 _frustumPlanes[6], const float3 _frustumPoints[8]);	// Recalculates the objects to draw. Calls Octree CollectFrustumIntersections
	void RemoveObjectFromDrawList(GameObject* _go);												// Removes a single GameObject from 'objectsToDraw'

	// ---------- Getters ---------- //
	unsigned int GetTextureColorbuffer()	{ return textureColorbuffer; }
	unsigned int GetViewportWidth()			{ return viewport_width; }
	unsigned int GetViewportHeight()		{ return viewport_height; }

private:
	void InitFramebuffer();											// Initialises a framebuffer to 'FBO', 'RBO' and 'textureColorbuffer' variables. Called at Initialisation
};
