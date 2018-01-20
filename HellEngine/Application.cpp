#include "Application.h"
#include "globals.h"
#include "Module.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "UpdateStatus.h"

Application::Application()
{
	/* Order matters: they will init/start/pre/update/post in this order */
	modules.push_back(input = new ModuleInput());
	modules.push_back(time = new ModuleTime());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(audio = new ModuleAudio());
}

Application::~Application()
{
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		RELEASE(*it);
}

bool Application::Init()
{
	bool ret = true;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init(); /* we init everything, even if not enabled */

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsEnabled() == true)
			ret = (*it)->Start();
	}

	return ret;
}

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it) {
		if ((*it)->IsEnabled() == true)
			ret = (*it)->PreUpdate();
	}

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it) {
		if ((*it)->IsEnabled() == true)
			ret = (*it)->Update();
	}

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it) {
		if ((*it)->IsEnabled() == true)
			ret = (*it)->PostUpdate();
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it) {
		if ((*it)->IsEnabled() == true)
			ret = (*it)->CleanUp();
	}

	return ret;
}
