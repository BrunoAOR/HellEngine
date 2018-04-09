#ifndef __H_SCRIPT_TYPE__
#define __H_SCRIPT_TYPE__

#include "dllControl.h"

enum class GAMEDLL_API ScriptType
{
	HELLO_WORLD,
	JUMP
};

GAMEDLL_API extern const ScriptType SCRIPT_TYPES[2];

GAMEDLL_API const char* GetScriptString(ScriptType scriptType);
GAMEDLL_API ScriptType GetScriptType(const char* scriptTypeString);

#endif // !__H_COMPONENT_TYPE__