#ifndef __H_APPLICATION__
#define __H_APPLICATION__

#include <list>
class Module;
class ModuleAudio;
class ModuleEditorCamera;
class ModuleImGui;
class ModuleInput;
class ModuleRender;
class ModuleScene;
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
	ModuleInput * input = nullptr;
	ModuleTime* time = nullptr;
	ModuleWindow* window = nullptr;
	ModuleEditorCamera* editorCamera = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleImGui* imgui = nullptr;
	ModuleScene* scene = nullptr;
	ModuleAudio* audio = nullptr;

private:
	std::list<Module*> modules;
};

extern Application* App;

#endif /* __H_APPLICATION__ */
