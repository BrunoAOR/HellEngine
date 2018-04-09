#include "ImGui_GameDLL/imgui.h"
#include "ComponentScript.h"
#include "ComponentType.h"
#include "ModuleScripting.h"
#include "ScriptType.h"

ComponentScript::ComponentScript(GameObject * owner) : Component(owner)
{
	type = ComponentType::SCRIPT;
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::UpdateScript()
{
}

void ComponentScript::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);
	}
}

int ComponentScript::MaxCountInGameObject()
{
	return -1;
}

ScriptType ComponentScript::GetScriptType()
{
	return scriptType;
}
