#ifndef __H_APPLICATION__
#define __H_APPLICATION__

#include <list>
class Module;
class ModuleAnimation;
class ModuleAudio; 
class ModuleDebugDraw;
class ModuleEditorCamera;
class ModuleImGui;
class ModuleInput;
class ModuleRender;
class ModuleScene;
class ModuleTime;
class ModuleWindow;
class ModuleTextureManager;
class ModuleShaderManager;
class ModuleTrueFont;
class ModuleUI;
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
	ModuleTextureManager * textureManager = nullptr;
	ModuleShaderManager* shaderManager = nullptr;
	ModuleInput * input = nullptr;
	ModuleTime* time = nullptr;
	ModuleWindow* window = nullptr;
	ModuleEditorCamera* editorCamera = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleScene* scene = nullptr;
	ModuleAnimation* animation = nullptr;
	ModuleImGui* imgui = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTrueFont* fonts = nullptr;
	ModuleUI* ui = nullptr;

private:
	std::list<Module*> modules;
};

extern Application* App;

#endif /* __H_APPLICATION__ */
