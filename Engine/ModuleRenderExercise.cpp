#include "GL/glew.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "ModuleDebugDraw.h"
#include "debugdraw.h"
#include "SDL.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	VBO = CreateQuadVBO(); //CreateTriangleVBO();
	texture0 = App->textureLoader->LoadTexture("textures/Lenna.png");
	defaultProgram = App->program->CreateProgramFromFile("shaders\\vertex_shader.glsl", "shaders\\fragment_shader.glsl");

	return true;
}

update_status ModuleRenderExercise::Update()
{
	dd::axisTriad(float4x4::identity, 0.1f, 1.0f);
	dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, dd::colors::Gray);

	glDisable(GL_CULL_FACE);
	RenderVBO();

	App->debugdraw->Draw(App->camera->ViewMatrix(), App->camera->ProjectionMatrix(), App->window->width, App->window->height);

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	LOG("Destroying renderer exercise");
	DestroyVBO();

	glDeleteProgram(defaultProgram);

	return true;
}

// This function must be called one time at creation of vertex buffer
unsigned ModuleRenderExercise::CreateTriangleVBO()
{
	float buffer_data[] = {
		 -1.0f, -1.0f, 0.0f, // ← v0 pos
		 1.0f, -1.0f, 0.0f, // ← v1 pos
		 0.0f, 1.0f, 0.0f, // ← v2 pos
		 
		 0.0f, 0.0f, // ← v0 texcoord
		 1.0f, 0.0f, // ← v1 texcoord
		 0.5f, 1.0f // ← v2 texcoord
	};
	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);

	return vbo;
}

unsigned int ModuleRenderExercise::CreateQuadVBO()
{
	float buffer_data[] = {
		 -1.0f, -1.0f, 0.0f, // ← v0 pos
		 1.0f, -1.0f, 0.0f, // ← v1 pos
		 -1.0f, 1.0f, 0.0f, // ← v2 pos
		 -1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,

		 0.0f, 0.0f, // ← v0 texcoord
		 1.0f, 0.0f, // ← v1 texcoord
		 0.0f, 1.0f, // ← v2 texcoord
		 0.0f, 1.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f
	};
	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);

	return vbo;
}

// This function must be called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO()
{
	glDeleteBuffers(1, &VBO);
}

// This function must be called each frame for drawing the triangle
void ModuleRenderExercise::RenderVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	// size = 3 float per vertex
	// stride = 0 is equivalent to stride = sizeof(float)*3
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 3 * 3)); // for triangle
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * 6)); //for quad
	
	float4x4 model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::identity,
		float3(2.0f, 1.0f, 0.0f));
	App->program->use(defaultProgram);
	App->program->setMat4(defaultProgram, "model", model);
	App->program->setMat4(defaultProgram, "view", App->camera->ViewMatrix());
	App->program->setMat4(defaultProgram, "proj", App->camera->ProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	App->program->setInt(defaultProgram, "mytexture", 0);

	// 1 triangle to draw = 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 6);
}