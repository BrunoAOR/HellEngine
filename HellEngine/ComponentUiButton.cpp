#include "ImGui/imgui.h"
#include "ComponentUiButton.h"
#include "ComponentType.h"

ComponentUiButton::ComponentUiButton(GameObject * owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_BUTTON;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentUiButton::~ComponentUiButton()
{
}

void ComponentUiButton::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;
	}
}

ButtonStatusType ComponentUiButton::GetButtonStatus()
{
	return buttonStatusType;
}

void ComponentUiButton::SetButtonStatus(ButtonStatusType buttonStatusTypeValue)
{
	buttonStatusType = buttonStatusTypeValue;
}
