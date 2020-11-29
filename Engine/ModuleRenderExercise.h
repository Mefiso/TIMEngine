#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleRenderExercise : public Module
{
public:
	unsigned int defaultProgram = 0;
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status Update();
	bool CleanUp();

private:
	unsigned int VBO;
	unsigned int texture0;
private:
	unsigned CreateTriangleVBO();
	unsigned CreateQuadVBO();
	void DestroyVBO();
	void RenderVBO();
};