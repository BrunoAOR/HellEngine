#include <algorithm>
#include <assert.h>
#include "HelloWorld.h"
#include "Jump.h"
#include "ModuleScripting.h"
#include "ScriptType.h"

ModuleScripting::ModuleScripting()
{
}

ModuleScripting::~ModuleScripting()
{
}

bool ModuleScripting::Init()
{
	return true;
}

bool ModuleScripting::CleanUp()
{
	scripts.clear();
	return true;
}

UpdateStatus ModuleScripting::Update()
{
	for (ComponentScript* script : scripts)
	{
		if (script->GetActive())
			script->UpdateScript();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

ComponentScript* ModuleScripting::BuildComponentScript(GameObject* owner, ScriptType type)
{
	ComponentScript* script = nullptr;

	switch (type)
	{
	case ScriptType::HELLO_WORLD:
		script = new HelloWorld(owner);
		break;
	case ScriptType::JUMP:
		script = new Jump(owner);
		break;
	default:
		assert(false);
	}

	if (script)
		scripts.push_back(script);

	return script;
}

bool ModuleScripting::DestroyScript(ComponentScript* script)
{
	std::vector<ComponentScript*>::iterator it = std::find(scripts.begin(), scripts.end(), script);

	if (it == scripts.end())
		return false;

	scripts.erase(it);
	delete script;

	return true;
}

const void ModuleScripting::GetScriptNames(std::vector<const char*>& scriptNames) const
{
	for (ScriptType type : SCRIPT_TYPES)
		scriptNames.push_back(GetScriptString(type));
}
