#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentType.h"
#include "ComponentUiImage.h"
#include "ComponentUiInputText.h"
#include "ComponentUiLabel.h"
#include "ComponentTransform2D.h"
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
	static int transitionTypeIndex = 0;

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		static const std::string baseLabel = std::string("Transition setup##TransitionSetup");
		if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
		{
			TransitionType currentTransitionType = transitionHandler.GetTransitionType();

			transitionTypeIndex = (int)currentTransitionType;
			if (ImGui::Combo("Transition Type", &transitionTypeIndex, "Color\0Sprite Change\0\0"))
			{
				transitionHandler.SetTransitionType((TransitionType)transitionTypeIndex);
			}

			switch (currentTransitionType)
			{
			case TransitionType::COLOR:
				if (ImGui::ColorEdit4("Default color", transitionHandler.stateInfos[0].color))
					transitionHandler.SetTransitionColor(TransitionState::DEFAULT);
				if (ImGui::ColorEdit4("Hover color", transitionHandler.stateInfos[1].color))
					transitionHandler.SetTransitionColor(TransitionState::HOVER);
				if (ImGui::ColorEdit4("Pressed color", transitionHandler.stateInfos[2].color))
					transitionHandler.SetTransitionColor(TransitionState::PRESSED);
				if (ImGui::ColorEdit4("Disabled color", transitionHandler.stateInfos[3].color))
					transitionHandler.SetTransitionColor(TransitionState::DISABLED);
				break;
			case TransitionType::SPRITE:
				ImGui::InputText("Default Sprite", transitionHandler.stateInfos[0].imagePath, 256);
				ImGui::InputText("Hover Sprite", transitionHandler.stateInfos[1].imagePath, 256);
				ImGui::InputText("Pressed Sprite", transitionHandler.stateInfos[2].imagePath, 256);
				ImGui::InputText("Disabled Sprite", transitionHandler.stateInfos[3].imagePath, 256);
				if (ImGui::Button("Load"))
				{
					transitionHandler.SetTransitionImage(TransitionState::DEFAULT);
					transitionHandler.SetTransitionImage(TransitionState::HOVER);
					transitionHandler.SetTransitionImage(TransitionState::PRESSED);
					transitionHandler.SetTransitionImage(TransitionState::DISABLED);
				}
				break;
			}

			ImGui::NewLine();
			ImGui::TreePop();
		}
		ImGui::ColorEdit4("Selection color", selectionColor);

	}
}

void ComponentUiInputText::UpdateTextField()
{
	if (hasFocus)
	{
		HandleCursorMotion();
		HandleDeletion();
		HandleClipboard();

		/* HANDLE RETURN KEY */
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RETURN2) == KeyState::KEY_DOWN)
		{
			SetFocusState(false);
		}

		/* HANDLE NEW TEXT */
		const char* newText = App->input->GetText();
		AddNewText(newText);

		UpdateCaret();

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

void ComponentUiInputText::SetTargetSelectionImage(ComponentUiImage* newSelectionImage)
{
	selectionImage = newSelectionImage;
}

void ComponentUiInputText::SetTargetCaretImage(ComponentUiImage* newCaretImage)
{
	caretImage = newCaretImage;
	newCaretImage->transform2D->SetSize(0, 0);
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
			caretImage->SetActive(true);
		}
		else
		{
			caretImage->SetActive(false);
			if (IsEmptyString(textContent))
			{
				textLabel->SetActive(false);
				placeholderLabel->SetActive(true);
			}
		}
	}
}

unsigned int ComponentUiInputText::GetMaxChars()
{
	return maxChars;
}

bool ComponentUiInputText::GetIsPassword()
{
	return isPassword;
}

void ComponentUiInputText::SetIsPassword(bool isPasswordValue)
{
	isPassword = isPasswordValue;
}

void ComponentUiInputText::HandleCursorMotion()
{
	/* Handle moving with arrows and making selections */
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_DOWN)
	{
		if (cursorPosition > 0)
		{
			--cursorPosition;
			HandleShiftKey();
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_DOWN)
	{
		if (cursorPosition < strnlen_s(textContent, maxChars) && cursorPosition < maxChars - 1)
		{
			++cursorPosition;
			HandleShiftKey();
		}
	}

	/* Handle Home and End */
	if (App->input->GetKey(SDL_SCANCODE_HOME) == KeyState::KEY_DOWN)
	{
		cursorPosition = 0;
		HandleShiftKey();
	}
	if (App->input->GetKey(SDL_SCANCODE_END) == KeyState::KEY_DOWN)
	{
		cursorPosition = currentCharCount;
		HandleShiftKey();
	}
}

void ComponentUiInputText::HandleDeletion()
{
	/* Handle deleting with backspace and delete */
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KeyState::KEY_DOWN)
	{
		if (cursorPosition > 0)
		{
			if (selectionStart == selectionEnd)
			{
				memcpy_s(textContent + cursorPosition - 1, currentCharCount - cursorPosition + 1, textContent + cursorPosition, currentCharCount - cursorPosition + 1);
				--cursorPosition;
				--currentCharCount;
			}
			else
			{
				DeleteSelection();
			}
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN)
	{
		if (cursorPosition < currentCharCount)
		{
			if (selectionStart == selectionEnd)
			{
				memcpy_s(textContent + cursorPosition, currentCharCount - cursorPosition + 1, textContent + cursorPosition + 1, currentCharCount - cursorPosition + 1);
				--currentCharCount;
			}
			else
			{
				DeleteSelection();
			}
		}
	}
}

void ComponentUiInputText::HandleClipboard()
{
	/* Handle copy and cut */
	if ((App->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN || App->input->GetKey(SDL_SCANCODE_X) == KeyState::KEY_DOWN)
		&& (App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KeyState::KEY_REPEAT))
	{
		if (selectionStart != selectionEnd)
		{
			uint selectionLeft = selectionStart < selectionEnd ? selectionStart : selectionEnd;
			uint selectionRight = selectionStart < selectionEnd ? selectionEnd : selectionStart;
			uint selectionSize = selectionRight - selectionLeft;
			char* selectionText = new char[selectionSize + 1];
			memcpy_s(selectionText, selectionSize, textContent + selectionLeft, selectionSize);
			selectionText[selectionSize] = '\0';

			bool success = App->input->SetClipboardText(selectionText);
			assert(success);
			delete[] selectionText;

			if (App->input->GetKey(SDL_SCANCODE_X) == KeyState::KEY_DOWN)
				DeleteSelection();
		}
	}

	/* Handle paste */
	if (App->input->GetKey(SDL_SCANCODE_V) == KeyState::KEY_DOWN && (App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KeyState::KEY_REPEAT))
	{
		std::string clipboardText = App->input->GetClipboardText();
		if (clipboardText.length() > 0)
		{
			uint clipboardSize = clipboardText.length();
			const char* newText = clipboardText.c_str();
			AddNewText(newText);
		}
	}
}

void ComponentUiInputText::UpdateCaret()
{
}

void ComponentUiInputText::AddNewText(const char* newText)
{
	uint newTextSize = strnlen_s(newText, maxChars);
	if (newTextSize > 0 && currentCharCount + newTextSize < maxChars)
	{
		DeleteSelection();
		char* tail = nullptr;
		uint tailSize = 0;
		if (cursorPosition != currentCharCount)
		{
			tailSize = currentCharCount - cursorPosition;
			tail = new char[tailSize];
			memcpy_s(tail, tailSize, textContent + cursorPosition, tailSize);
		}
		memcpy_s(textContent + cursorPosition, maxChars - cursorPosition, newText, newTextSize);
		cursorPosition += newTextSize;
		selectionStart = selectionEnd = cursorPosition;
		currentCharCount += newTextSize;
		if (tail)
		{
			memcpy_s(textContent + cursorPosition, maxChars - cursorPosition, tail, tailSize);
		}
		textContent[currentCharCount] = '\0';

		delete[] tail;
	}
}

void ComponentUiInputText::DeleteSelection()
{
	if (selectionStart != selectionEnd)
	{
		uint selectionLeft = selectionStart < selectionEnd ? selectionStart : selectionEnd;
		uint selectionRight = selectionStart < selectionEnd ? selectionEnd : selectionStart;
		uint selectionSize = selectionRight - selectionLeft;
		memcpy_s(textContent + selectionLeft, currentCharCount - selectionRight + 1, textContent + selectionRight, currentCharCount - selectionRight + 1);
		currentCharCount -= selectionSize;
		cursorPosition = selectionStart = selectionEnd = selectionLeft;
	}
}

void ComponentUiInputText::HandleShiftKey()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KeyState::KEY_REPEAT)
	{
		selectionEnd = cursorPosition;
	}
	else
	{
		selectionStart = cursorPosition;
		selectionEnd = cursorPosition;
	}
}
