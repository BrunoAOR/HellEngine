#ifndef __H_MODULE_SCRIPTING__
#define __H_MODULE_SCRIPTING__

#include <vector>
#include "ComponentScript.h"
#include "Module.h"

class ModuleScripting :
	public Module
{
public:

	ModuleScripting();
	~ModuleScripting();

	bool Init();
	bool CleanUp();
	UpdateStatus Update();

	ComponentScript* BuildComponentScript(GameObject* owner, ScriptType type);
	bool DestroyScript(ComponentScript* script);

	const void GetScriptNames(std::vector<const char*>& scriptNames) const;

private:
	std::vector<ComponentScript*> scripts;
};

#endif