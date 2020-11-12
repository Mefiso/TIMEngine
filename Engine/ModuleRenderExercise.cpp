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

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif
	VBO = CreateQuadVBO(); //CreateTriangleVBO();
	texture0 = App->textureLoader->LoadTexture("textures/Lenna.png");
	App->program->CreateProgramFromFile("shaders\\vertex_shader.glsl", "shaders\\fragment_shader.glsl");

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
	App->program->use();
	App->program->setMat4("model", model);
	App->program->setMat4("view", App->camera->ViewMatrix());
	App->program->setMat4("proj", App->camera->ProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	App->program->setInt("mytexture", 0);

	// 1 triangle to draw = 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 6);
}