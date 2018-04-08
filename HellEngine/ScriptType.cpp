#include <assert.h>
#include <string>
#include "ScriptType.h"

const ScriptType SCRIPT_TYPES[] = { ScriptType::HELLO_WORLD, ScriptType::JUMP };

const char* GetScriptString(ScriptType scriptType)
{
	switch (scriptType)
	{
	case ScriptType::HELLO_WORLD:
		return "Hello World";
	case ScriptType::JUMP:
		return "Jump";
	default:
		return "";
	}
}

ScriptType GetScriptType(const char * scriptTypeString)
{
	if (strcmp(scriptTypeString, "Hello World") == 0)
		return ScriptType::HELLO_WORLD;
	else if (strcmp(scriptTypeString, "Jump") == 0)
		return ScriptType::JUMP;

	assert(false);
	return ScriptType();
}
