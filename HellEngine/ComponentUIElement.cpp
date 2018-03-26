#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUIElement.h"
#include "ComponentType.h"
#include "ModuleUI.h"


ComponentUIElement::ComponentUIElement(GameObject * owner) : Component(owner)
{
	type = ComponentType::UI_ELEMENT;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	rect.x = 10;
	rect.y = 10;
	rect.w = 10;
	rect.h = 10;
    App->ui->RegisterUiElement(this);
}

ComponentUIElement::~ComponentUIElement()
{
	App->ui->UnregisterUiElement(this);
}

void ComponentUIElement::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		int pos[2] = { rect.x, rect.y };
		int size[2] = { rect.w, rect.h };
		if (ImGui::DragInt2("Position", pos, 0.3f))
		{
			rect.x = pos[0];
			rect.y = pos[1];
		}
		if (ImGui::DragInt2("Size", size, 0.3f))
		{
			rect.w = size[0];
			rect.h = size[1];
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

UIElementType ComponentUIElement::GetUiType() const
{
	return uiType;
}

const SDL_Rect& ComponentUIElement::GetRect() const
{
	return rect;
}

void ComponentUIElement::SetRect(const SDL_Rect& rectValue)
{
	rect.x = rectValue.x;
	rect.y = rectValue.y;
	rect.w = rectValue.w;
	rect.h = rectValue.h;
}

bool ComponentUIElement::GetVisible() const
{
	return visible;
}

void ComponentUIElement::SetVisible(bool visibleValue)
{
	visible = visibleValue;
}
