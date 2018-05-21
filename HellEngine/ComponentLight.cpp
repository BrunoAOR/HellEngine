#include "ImGui/imgui.h"
#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "SerializableObject.h"
#include "globals.h"

ComponentLight::ComponentLight(GameObject* owner) : Component(owner)
{
	type = ComponentType::LIGHT;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
}

ComponentLight::~ComponentLight()
{
	if (isActiveLight)
		;//Inform scene about light's destruction
}

const float3* ComponentLight::GetPosition()
{
	if (transform)
		return &(transform->GetPosition());
	return nullptr;
}

void ComponentLight::SetAsActiveLight()
{
	isActiveLight = true;
	//Inform scene about light being the active Light
}

void ComponentLight::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		if (ImGui::Checkbox("Active Light", &isActiveLight))
		{
			// Inform scene about light being active or inactive
		}
	}
}

int ComponentLight::MaxCountInGameObject()
{
	return 1;
}

void ComponentLight::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	obj.AddBool("IsActiveLight", isActiveLight);
}

void ComponentLight::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	bool isActiveLight = obj.GetBool("IsActiveLight");
	if (isActiveLight)
		SetAsActiveLight();
}
