#include "ImGui/imgui.h"
#include "MathGeoLib/src/Algorithm/Random/LCG.h"
#include "Component.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "SerializableObject.h"
#include "globals.h"

int Component::nextId = 0;

Component::Component(GameObject* owner) : gameObject(owner)
{
	LCG lcg;
	uuid = lcg.IntFast();
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

u32 Component::GetUUID() const
{
	return uuid;
}

void Component::Save(SerializableObject& obj) const
{
	obj.Addu32("UUID", uuid);
	obj.AddString("Type", GetString(type));
	obj.AddBool("Active", isActive);
}

void Component::Load(const SerializableObject& obj)
{
	uuid = obj.Getu32("UUID");
	/* type variable is not loaded because it was set in the specific Component's constructor */
	isActive = obj.GetBool("Active");
}

void Component::LinkComponents(const SerializableObject & obj, const std::map<u32, Component*>& componentsCreated)
{
	/* Method left empty to allow inheriting classes to not declare it */
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
