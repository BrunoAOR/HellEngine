#ifndef __H_APPLICATION__
#define __H_APPLICATION__

#include <list>
class Module;
class ModuleAudio;
class ModuleInput;
class ModuleRender;
class ModuleTime;
class ModuleTriangle;
class ModuleWindow;
enum class UpdateStatus;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	UpdateStatus Update();
	bool CleanUp();

public:
	ModuleInput * input = nullptr;
	ModuleTime* time = nullptr;
	ModuleWindow* window = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleTriangle* triangle = nullptr;

private:
	std::list<Module*> modules;
};

extern Application* App;

#endif /* __H_APPLICATION__ */
