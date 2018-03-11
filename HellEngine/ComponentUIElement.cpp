#include "ComponentUIElement.h"
#include "ComponentType.h"
#include "ImGui\imgui.h"


ComponentUIElement::ComponentUIElement(GameObject * owner) : Component(owner)
{
	type = ComponentType::UIELEMENT;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentUIElement::~ComponentUIElement()
{
}

void ComponentUIElement::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;
	}
}

int ComponentUIElement::MaxCountInGameObject()
{
	return -1;
}

const char * ComponentUIElement::GetUITypeString(UIElementType uiType)
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

SDL_Rect * ComponentUIElement::GetRect()
{
	return rect;
}

void ComponentUIElement::SetRect(SDL_Rect * rectValue)
{
	rect = rectValue;
}

bool ComponentUIElement::GetVisible()
{
	return visible;
}

void ComponentUIElement::SetVisible(bool visibleValue)
{
	visible = visibleValue;
}
