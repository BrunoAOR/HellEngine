#include "ImGui/imgui.h"
#include "Component.h"
#include "GameObject.h"
#include "globals.h"

int Component::nextId = 0;

Component::Component(GameObject* owner) : gameObject(owner)
{
	editorInfo.id = nextId++;
	//LOGGER("Constructing Component for GameObject '%s'", gameObject->name.c_str());
}

Component::~Component()
{
	//LOGGER("Deleted Component from GameObject '%s'", gameObject->name.c_str());
}

ComponentType Component::GetType()
{
	return type;
}

void Component::Update()
{}

bool Component::GetActive() const
{
	return isActive && gameObject->GetActive();
}

void Component::SetActive(bool activeState)
{
	isActive = activeState;
}

bool Component::OnEditorDeleteComponent()
{
	static std::string label = "";

	float width = ImGui::GetWindowContentRegionMax().x;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
	ImGui::Indent(width - 130);

	label = "Remove Component##" + std::to_string(editorInfo.id);
	bool pushed = ImGui::Button(label.c_str(), ImVec2(120, 20));
	ImGui::Unindent(width - 130);
	ImGui::PopStyleColor();

	if (pushed)
	{
		gameObject->RemoveComponent(this);
		return true;
	}
	return false;
}
