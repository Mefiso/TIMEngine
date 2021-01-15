#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"
#include "debugdraw.h"
#include "SDL.h"
#include "Leaks.h"
#include "Brofiler.h"

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
	GLenum err = glewInit();
	// c check for errors
	LOG("[info] Using Glew %s", glewGetString(GLEW_VERSION));
	// Should be 2.0

	LOG("[info] Vendor: %s", glGetString(GL_VENDOR));
	LOG("[info] Renderer: %s", glGetString(GL_RENDERER));
	LOG("[info] OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("[info] GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);
	viewport_width = (int)(mode.w * 0.8f);
	viewport_height = (int)(mode.h * 0.8f);
	InitFramebuffer();
	glViewport(0, 0, viewport_width, viewport_height);

	glDepthFunc(GL_LEQUAL); // For the skybox to be visualized at z-depth = (+-)1

	return true;
}

update_status ModuleRender::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdateRenderer", Profiler::Color::Orchid);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	BROFILER_CATEGORY("UpdateRenderer", Profiler::Color::Orchid);
	if (depthTest) glEnable(GL_DEPTH_TEST); // Enable depth test
	else glDisable(GL_DEPTH_TEST);

	if (cullFace) glEnable(GL_CULL_FACE); // Enable cull backward faces
	else glDisable(GL_CULL_FACE);

	dd::axisTriad(float4x4::identity, 0.1f, 1.0f);
	if (showGrid)
		dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, gridColor);
	if (showOctree)
		App->sceneMng->octree.Draw();

	// Render not culled GameObjects
	for (std::vector<GameObject*>::iterator it = objectsToDraw.begin(), end = objectsToDraw.end(); it != end; ++it)
		(*it)->Draw();

	if (App->camera->cullingCamera != App->camera->defaultCamera)
		App->camera->cullingCamera->Draw();

	App->debugdraw->Draw(App->camera->ViewMatrix(), App->camera->ProjectionMatrix(), viewport_width, viewport_height);

	// DEBUG only - MousePicking ray
	dd::line(picking.a, picking.b, dd::colors::Blue);

	// Render Skybox
	// TODO: if (drawskybox)... else glClearColor(bgcolor)
	App->sceneMng->DrawSkybox();

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdateRenderer", Profiler::Color::Orchid);
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRender::PerformFrustumCulling(const float4 frustumPlanes[6], const float3 frustumPoints[8])
{
	objectsToDraw.clear();
	App->sceneMng->octree.CollectFrustumIntersections(objectsToDraw, frustumPlanes, frustumPoints);
}

void ModuleRender::RemoveObjectFromDrawList(GameObject* go)
{
	std::vector<GameObject*>::iterator it = std::find(objectsToDraw.begin(), objectsToDraw.end(), go);
	if (it != objectsToDraw.end())
		objectsToDraw.erase(it);
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
