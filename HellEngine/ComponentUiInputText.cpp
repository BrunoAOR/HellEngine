#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentType.h"
#include "ComponentUiInputText.h"
#include "ComponentUiLabel.h"
#include "ModuleInput.h"
#include "globals.h"


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

void ComponentUiInputText::UpdateTextField()
{
	if (hasFocus)
	{
		/* Handle keys */
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_DOWN)
		{
			if (cursorPosition > 0)
				--cursorPosition;
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_DOWN)
		{
			if (cursorPosition < strnlen_s(textContent, 256) && cursorPosition < 255)
				++cursorPosition;
		}

		/* Handle new text */
		const char* newText = App->input->GetText();
		uint newTextSize = strnlen_s(newText, 32);
		if (newTextSize > 0 && currentCharCount + newTextSize < 256)
		{
			char* tail = nullptr;
			uint tailSize = 0;
			if (cursorPosition != currentCharCount)
			{
				tailSize = currentCharCount - cursorPosition;
				tail = new char[tailSize];
				memcpy_s(tail, tailSize, textContent + cursorPosition, tailSize);
			}
			memcpy_s(textContent + cursorPosition, 256 - cursorPosition, newText, newTextSize);
			cursorPosition += newTextSize;
			currentCharCount += newTextSize;
			if (tail)
			{
				memcpy_s(textContent + cursorPosition, 256 - cursorPosition, tail, tailSize);
			}
			textContent[currentCharCount] = '\0';

			delete tail;
		}

		textLabel->SetLabelText(textContent);
	}
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

bool ComponentUiInputText::GetFocusState() const
{
	return hasFocus;
}

void ComponentUiInputText::SetFocusState(bool focusState)
{
	if (hasFocus != focusState)
	{
		hasFocus = focusState;
		if (hasFocus)
		{
			textLabel->SetActive(true);
			placeholderLabel->SetActive(false);
		}
		else if (IsEmptyString(textContent))
		{
			textLabel->SetActive(false);
			placeholderLabel->SetActive(true);
		}
	}
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
