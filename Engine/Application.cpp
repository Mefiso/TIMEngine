#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"
#include "Leaks.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(input = new ModuleInput());
	//modules.push_back(program = new ModuleProgram());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(debugdraw = new ModuleDebugDraw());
	modules.push_back(editor = new ModuleEditor());
	//modules.push_back(textureLoader = new ModuleTexture());
	modules.push_back(camera = new ModuleCamera());
	
}

Application::~Application()
{
	for(vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();
	if (ret)
		ret = ModuleTexture::Init();

	for (vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Start();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::BroadcastEvent(const Event& event)
{
	for (vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->ReceiveEvent(event);
}

void Application::Log(const char* input) const
{
	editor->Log(input);
}
