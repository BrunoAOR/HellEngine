#include "ImGui/imgui.h"
#include "ComponentType.h"
#include "ComponentUiInputText.h"

ComponentUiInputText::ComponentUiInputText(GameObject * owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_INPUT_TEXT;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentUiInputText::~ComponentUiInputText()
{
}

void ComponentUiInputText::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;
	}
}

void ComponentUiInputText::SetTargetBackgroundImage(ComponentUiImage* newBackgroundImage)
{
	backgroundImage = newBackgroundImage;
}

void ComponentUiInputText::SetTargetPlaceholderLabel(ComponentUiLabel* newPlaceholderLabel)
{
	placeholderLabel = newPlaceholderLabel;
}

void ComponentUiInputText::SetTargetTextLabel(ComponentUiLabel* newTextLabel)
{
	textLabel = newTextLabel;
}

void ComponentUiInputText::SetTargetCaretImage(ComponentUiImage* newCaretImage)
{
	caretImage = newCaretImage;
}

unsigned int ComponentUiInputText::GetMaxChars()
{
	return maxChars;
}

void ComponentUiInputText::SetMaxChars(unsigned int maxCharsValue)
{
	maxChars = maxCharsValue;
}

bool ComponentUiInputText::GetIsPassword()
{
	return isPassword;
}

void ComponentUiInputText::SetIsPassword(bool isPasswordValue)
{
	isPassword = isPasswordValue;
}
