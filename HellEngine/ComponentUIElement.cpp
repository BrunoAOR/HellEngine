#include "ComponentUIElement.h"
#include "ComponentType.h"
#include "ImGui\imgui.h"


ComponentUIElement::ComponentUIElement(GameObject * owner) : Component(owner)
{
	type = ComponentType::UIELEMENT;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
    if (rect == nullptr) {
        SDL_Rect* rectTemp = new SDL_Rect;
        rectTemp->h = 10;
        rectTemp->w = 10;
        rectTemp->x = 10;
        rectTemp->y = 10;
        SetRect(rectTemp);
    }
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

		if (rect != nullptr) 
        {
            int pos[2] = { rect->x, rect->y };
            int size[2] = {rect->w, rect->h};
            if (ImGui::DragInt2("Position", pos, 0.3f)) 
            {
                rect->x = pos[0];
                rect->y = pos[1];
            }
            if (ImGui::DragInt2("Size", size, 0.3f))
            {
                rect->w = size[0];
                rect->h = size[1];
            }
        }
		
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
