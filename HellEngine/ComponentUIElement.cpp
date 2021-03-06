#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUIElement.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleUI.h"


const UIElementType UI_ELEMENT_TYPES[] = { UIElementType::IMG, UIElementType::LABEL, UIElementType::BUTTON, UIElementType::INPUT_TEXT };

const char * GetUITypeString(UIElementType uiType)
{
	switch (uiType)
	{
	case UIElementType::IMG:
		return "Image";
	case UIElementType::BUTTON:
		return "Button";
	case UIElementType::LABEL:
		return "Label";
	case UIElementType::INPUT_TEXT:
		return "Input Text";
	default:
		return "";
	}
}

ComponentUIElement::ComponentUIElement(GameObject * owner) : Component(owner)
{
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	transform2D = (ComponentTransform2D*)gameObject->GetComponent(ComponentType::TRANSFORM_2D);
	assert(transform2D);
	App->ui->RegisterUiElement(this);
}

ComponentUIElement::~ComponentUIElement()
{
	App->ui->UnregisterUiElement(this);
}

void ComponentUIElement::OnEditor()
{
}

int ComponentUIElement::MaxCountInGameObject()
{
	return 1;
}
