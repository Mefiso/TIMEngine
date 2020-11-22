#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "Model.h"
#include "SDL.h"
#include "ModuleDebugDraw.h"
#include "debugdraw.h"
#include "Leaks.h"

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

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	
	// Create an OpenGL context associated with the window.
	context = SDL_GL_CreateContext(App->window->window);
	//SDL_GL_MakeCurrent(App->window->window, context);
	GLenum err = glewInit();
	// c check for errors
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	// Should be 2.0

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// They are enabled inside ImGui_ImplOpenGL3_RenderDrawData so setting them here is useless
	glEnable(GL_DEPTH_TEST); // Enable depth test
	//glEnable(GL_CULL_FACE); // Enable cull backward faces
	//glFrontFace(GL_CCW); // Front faces will be counter clockwise
	//glDisable(GL_CULL_FACE);
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	defaultProgram = App->program->CreateProgramFromFile("shaders\\vertex_shader.glsl", "shaders\\fragment_shader.glsl");
	// Load models
	bakerHouse = new Model("models/baker_house/BakerHouse.fbx");

	return true;
}

update_status ModuleRender::PreUpdate()
{
	float currentFrame = SDL_GetTicks()/1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	App->editor->ProcessFPS(deltaTime);

	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP) {
		//SDL_CaptureMouse(SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	if (eventOcurred) {
		TranslateCamera(deltaTime);
		RotateCameraKeys(deltaTime);
	}

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	dd::axisTriad(float4x4::identity, 0.1f, 1.0f);
	dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, dd::colors::Gray);

	bakerHouse->Draw(defaultProgram);

	App->debugdraw->Draw(App->camera->ViewMatrix(), App->camera->ProjectionMatrix(), App->window->width, App->window->height);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	delete bakerHouse;
	glDeleteProgram(defaultProgram);
	
	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRender::WindowResized(unsigned int width, unsigned int height)
{
	App->camera->onResize((float) width / (float) height);

	App->window->width = width;
	App->window->height = height;
	App->editor->UpdateWindowSizeSettings();
}

void ModuleRender::RotateCameraMouse(float xoffset, float yoffset)
{
	if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
		//SDL_CaptureMouse(SDL_TRUE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	App->camera->ProcessMouseMovement(xoffset, yoffset);
}

void ModuleRender::MouseWheel(float xoffset, float yoffset)
{
	App->camera->ProcessMouseScroll(yoffset);
}

void ModuleRender::DropFile(const char* file)
{
	delete bakerHouse;
	bakerHouse = new Model(file);
}

void ModuleRender::TranslateCamera(float deltaTime)
{
	// Translate camera
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		App->camera->ProcessKeyboard(UP, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		App->camera->ProcessKeyboard(DOWN, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->camera->ProcessKeyboard(FORWARD, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		App->camera->ProcessKeyboard(LEFT, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		App->camera->ProcessKeyboard(RIGHT, deltaTime);

	// Speed increase/decrease
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		App->camera->MovementSpeed *= 2;
		App->editor->UpdateCameraSettings();
	}
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
		App->camera->MovementSpeed /= 2;
		App->editor->UpdateCameraSettings();
	}
}

void ModuleRender::RotateCameraKeys(float deltaTime)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_UP, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_DOWN, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_LEFT, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_RIGHT, deltaTime);
}

