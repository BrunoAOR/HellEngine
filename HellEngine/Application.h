#ifndef __H_APPLICATION__
#define __H_APPLICATION__

#include <list>
class Module;
class ModuleAudio;
class ModuleFadeToBlack;
class ModuleInput;
class ModuleRender;
class ModuleTextures;
class ModuleTime;
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
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleFadeToBlack* fade = nullptr;
	ModuleTime* time = nullptr;

private:
	std::list<Module*> modules;
};

extern Application* App;

#endif /* __H_APPLICATION__ */
