#include "globals.h"
#include "HelloWorld.h"
#include "ScriptType.h"

HelloWorld::HelloWorld(GameObject* owner) : ComponentScript(owner)
{
	scriptType = ScriptType::HELLO_WORLD;
	editorInfo.idLabel = std::string(GetScriptString(scriptType)) + "##" + std::to_string(editorInfo.id);
}

HelloWorld::~HelloWorld()
{
}

void HelloWorld::UpdateScript()
{
	LOGGER("Hello World!");
}
