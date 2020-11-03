#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	unsigned int VBO;
	unsigned CreateTriangleVBO();
	void DestroyVBO(unsigned vbo);
	void RenderVBO(unsigned vbo, unsigned program);
	void* context;
};