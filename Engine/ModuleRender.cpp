#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleDebugDraw.h"
#include "debugdraw.h"
#include "Model.h"
#include "SDL.h"
#include "uSTimer.h"
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
	msTimer = MSTimer();
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
	LOG("[info] Using Glew %s", glewGetString(GLEW_VERSION));
	// Should be 2.0

	LOG("[info] Vendor: %s", glGetString(GL_VENDOR));
	LOG("[info] Renderer: %s", glGetString(GL_RENDERER));
	LOG("[info] OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("[info] GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// They are enabled inside ImGui_ImplOpenGL3_RenderDrawData so setting them here is useless
	//glEnable(GL_DEPTH_TEST); // Enable depth test
	//glEnable(GL_CULL_FACE); // Enable cull backward faces
	//glFrontFace(GL_CCW); // Front faces will be counter clockwise
	//glDisable(GL_CULL_FACE);
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	defaultProgram = App->program->CreateProgramFromFile(".\\resources\\shaders\\vertex_shader.glsl", ".\\resources\\shaders\\fragment_shader.glsl");
	
	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);
	viewport_width = mode.w * 0.6f;
	viewport_height = mode.h * 0.6f;
	InitFramebuffer();
	glViewport(0, 0, viewport_width, viewport_height);

	// Load models
	uSTimer test = uSTimer();
	modelLoaded = new Model();
	modelLoaded->Load("./resources/models/baker_house/BakerHouse.fbx");

	msTimer.Start();

	return true;
}

update_status ModuleRender::PreUpdate()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	deltatime = msTimer.Stop() / 1000.f;		// TODO: CAMERA MOVEMENTS ARE NOT FLUID!?
	msTimer.Start();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	if (depthTest) glEnable(GL_DEPTH_TEST); // Enable depth test
	else glDisable(GL_DEPTH_TEST);

	if (cullFace) glEnable(GL_CULL_FACE); // Enable cull backward faces
	else glDisable(GL_CULL_FACE);

	dd::axisTriad(float4x4::identity, 0.1f, 1.0f);
	dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, gridColor);
	
	//glViewport(0, 0, viewport_width, viewport_height);
	modelLoaded->Draw(defaultProgram);

	if (showGrid)
		App->debugdraw->Draw(App->camera->ViewMatrix(), App->camera->ProjectionMatrix(), viewport_width, viewport_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	RELEASE(modelLoaded);
	glDeleteProgram(defaultProgram);
	
	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRender::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::file_dropped:
		DropFile(event.string.ptr);
		break;
	}
}

void ModuleRender::ProcessViewportEvents() {
	TranslateCamera(deltatime);
	RotateCameraKeys(deltatime);
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		App->camera->onFocus(modelLoaded->enclosingSphere.pos, modelLoaded->enclosingSphere.r * 3);
	}
}

bool ModuleRender::DropFile(const std::string& file)
{
	if (file.substr(file.find_last_of('.'), file.size()).compare(".fbx") == 0) {
		modelLoaded->Load(file);
		App->camera->onFocus(modelLoaded->enclosingSphere.pos, modelLoaded->enclosingSphere.r * 3);
		return true;
	}
	else {
		// Replace all textures because we only have one per model
		modelLoaded->ReloadTexture(file.c_str());
		return false;
	}
}

void ModuleRender::InitFramebuffer()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewport_width, viewport_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport_width, viewport_height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("[error] FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::TranslateCamera(float deltaTime) const
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
		App->camera->ProcessSpeed(2);	
	}
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
		App->camera->ProcessSpeed(0.5f);
	}
}

void ModuleRender::RotateCameraKeys(float deltaTime) const
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

