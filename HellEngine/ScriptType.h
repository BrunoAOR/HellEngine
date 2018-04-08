#ifndef __H_SCRIPT_TYPE__
#define __H_SCRIPT_TYPE__

enum class ScriptType
{
	HELLO_WORLD,
	JUMP
};

extern const ScriptType SCRIPT_TYPES[2];

const char* GetScriptString(ScriptType scriptType);
ScriptType GetScriptType(const char* scriptTypeString);

#endif // !__H_COMPONENT_TYPE__
