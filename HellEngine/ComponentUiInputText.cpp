#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentType.h"
#include "ComponentUiImage.h"
#include "ComponentUiInputText.h"
#include "ComponentUiLabel.h"
#include "ComponentTransform2D.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "SerializableObject.h"
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

		ImGui::DragFloat("Caret blink rate", &caretBlinkRate, 0.1f, 0.0f, 5.0f);
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

		UpdateSelection();
		UpdateCaret();
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
	if (selectionImage)
	{
		selectionImage->SetImagePath("assets/images/whiteSquare.png");
		selectionImage->SetColor(0.5f, 1.0f, 1.0f, 0.25f);
		selectionImage->transform2D->SetSize(0, 0);
	}
}

void ComponentUiInputText::SetTargetCaretImage(ComponentUiImage* newCaretImage)
{
	caretImage = newCaretImage;
	if (caretImage)
	{
		caretImage->SetImagePath("assets/images/whiteSquare.png");
		caretImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
		caretImage->transform2D->SetSize(0, 0);
	}
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
			selectionImage->SetActive(true);
		}
		else
		{
			caretImage->SetActive(false);
			selectionImage->SetActive(false);
			selectionStart = selectionEnd = cursorPosition = currentCharCount;
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

void ComponentUiInputText::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	SerializableObject transitionsObj = obj.BuildSerializableObject("Transitions");
	transitionHandler.Save(transitionsObj);
	
	obj.Addu32("placeholderLabelUUID", placeholderLabel ? placeholderLabel->GetUUID() : 0);
	obj.Addu32("TextLabelUUID", textLabel ? textLabel->GetUUID() : 0);
	obj.Addu32("SelectionImageUUID", selectionImage ? selectionImage->GetUUID() : 0);
	obj.Addu32("CaretImageUUID", caretImage ? caretImage->GetUUID() : 0);

	obj.AddString("TextContent", textContent);
	obj.AddInt("CurrentCharCount", currentCharCount);
	obj.AddFloat("CaretBlinkRate", caretBlinkRate);
	obj.AddBool("HasFocus", hasFocus);
	obj.AddBool("IsPassword", isPassword);

	std::vector<int> objWidths;
	objWidths.insert(objWidths.begin(), widths, widths + currentCharCount);
	obj.AddVectorInt("Widths", objWidths);
}

void ComponentUiInputText::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	SerializableObject transitionsObj = obj.GetSerializableObject("Transitions");
	transitionHandler.Load(transitionsObj);

	std::string objTextContent = obj.GetString("TextContent");
	memcpy_s(textContent, 256, objTextContent.c_str(), objTextContent.length());
	textContent[objTextContent.length()] = '\0';

	currentCharCount = obj.GetInt("CurrentCharCount");
	caretBlinkRate = obj.GetFloat("CaretBlinkRate");
	hasFocus = obj.GetBool("HasFocus");
	isPassword = obj.GetBool("IsPassword");

	std::vector<int> objWidths = obj.GetVectorInt("Widths");
	for (uint i = 0; i < currentCharCount; ++i)
	{
		widths[i] = objWidths[i];
	}
}

void ComponentUiInputText::LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated)
{
	SerializableObject transitionsObj = obj.GetSerializableObject("Transitions");
	transitionHandler.LinkComponents(transitionsObj, componentsCreated);

	u32 placeholderLabelUUID = obj.Getu32("placeholderLabelUUID");
	if (placeholderLabelUUID != 0)
	{
		assert(componentsCreated.count(placeholderLabelUUID) == 1);
		placeholderLabel = (ComponentUiLabel*)componentsCreated.at(placeholderLabelUUID);
	}

	u32 textLabelUUID = obj.Getu32("TextLabelUUID");
	if (textLabelUUID != 0)
	{
		assert(componentsCreated.count(textLabelUUID) == 1);
		textLabel = (ComponentUiLabel*)componentsCreated.at(textLabelUUID);
		textLabel->SetLabelText(textContent);
	}

	u32 selectionImageUUID = obj.Getu32("SelectionImageUUID");
	if (selectionImageUUID != 0)
	{
		assert(componentsCreated.count(selectionImageUUID) == 1);
		selectionImage = (ComponentUiImage*)componentsCreated.at(selectionImageUUID);
	}

	u32 caretImageUUID = obj.Getu32("CaretImageUUID");
	if (caretImageUUID != 0)
	{
		assert(componentsCreated.count(caretImageUUID) == 1);
		caretImage = (ComponentUiImage*)componentsCreated.at(caretImageUUID);
	}

	selectionStart = selectionEnd = cursorPosition = currentCharCount;
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
		if (selectionStart == selectionEnd)
		{
			if (cursorPosition > 0)
			{
				memcpy_s(textContent + cursorPosition - 1, currentCharCount - cursorPosition + 1, textContent + cursorPosition, currentCharCount - cursorPosition + 1);
				for (uint w = cursorPosition - 1; w < currentCharCount - 1; ++w)
				{
					widths[w] = widths[w + 1];
				}
				widths[currentCharCount - 1] = 0;
				
				--cursorPosition;
				--currentCharCount;
				textLabel->SetLabelText(textContent);
			}
		}
		else
		{
			DeleteSelection();
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN)
	{
		if (selectionStart == selectionEnd)
		{
			if (cursorPosition < currentCharCount)
			{
				memcpy_s(textContent + cursorPosition, currentCharCount - cursorPosition + 1, textContent + cursorPosition + 1, currentCharCount - cursorPosition + 1);
				for (uint w = cursorPosition; w < currentCharCount - 1; ++w)
				{
					widths[w] = widths[w + 1];
				}
				widths[currentCharCount - 1] = 0;
				--currentCharCount;
				textLabel->SetLabelText(textContent);
			}
		}
		else
		{
			DeleteSelection();
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

void ComponentUiInputText::UpdateSelection()
{
	if (selectionStart != selectionEnd)
	{
		uint selectionLeft = selectionStart < selectionEnd ? selectionStart : selectionEnd;
		uint selectionRight = selectionStart < selectionEnd ? selectionEnd : selectionStart;
		int posX = 0;
		int width = 0;
		for (uint w = 0; w < selectionRight; ++w)
		{
			if (w < selectionLeft)
				posX += widths[w];
			else
				width += widths[w];
		}
		selectionImage->transform2D->SetLocalPos((float)posX, 0.0f);
		selectionImage->transform2D->SetSize(width, textLabel->GetFontSize());
	}
	else
	{
		selectionImage->transform2D->SetSize(0.0f, 0.0f);
	}
}

void ComponentUiInputText::UpdateCaret()
{
	bool doUpdate = true;
	if (caretBlinkRate == 0)
	{
		caretImage->SetActive(true);
	}
	else
	{
		caretElapsedTime += App->time->DeltaTime();
		float cycleDuration = 1 / caretBlinkRate;
		while (caretElapsedTime > cycleDuration)
			caretElapsedTime -= cycleDuration;

		if (caretElapsedTime < 0.5f * cycleDuration)
			caretImage->SetActive(true);
		else
		{
			caretImage->SetActive(false);
			doUpdate = false;
		}
	}

	if (doUpdate)
	{
		caretImage->transform2D->SetSize(1, textLabel->GetFontSize());
		int xPos = 0;
		for (uint i = 0; i < cursorPosition; ++i)
		{
			xPos += widths[i];
		}
		caretImage->transform2D->SetLocalPos((float)xPos, 0);
	}
}

void ComponentUiInputText::AddNewText(const char* newText)
{
	uint newTextSize = strnlen_s(newText, maxChars);
	if (newTextSize > 0 && currentCharCount + newTextSize < maxChars)
	{
		DeleteSelection();
		int charactersCopied = 0;
		while (newTextSize - charactersCopied > 0)
		{
			char* tail = nullptr;
			uint tailSize = 0;
			if (cursorPosition != currentCharCount)
			{
				tailSize = currentCharCount - cursorPosition;
				tail = new char[tailSize];
				memcpy_s(tail, tailSize, textContent + cursorPosition, tailSize);
			}

			textContent[cursorPosition] = newText[charactersCopied];

			++cursorPosition;
			selectionStart = selectionEnd = cursorPosition;
			++currentCharCount;
			
			if (tail)
			{
				memcpy_s(textContent + cursorPosition, maxChars - cursorPosition, tail, tailSize);
			}
			textContent[currentCharCount] = '\0';
			
			int oldWidth = textLabel->GetTextureWidth();
			textLabel->SetLabelText(textContent);
			int charWidth = textLabel->GetTextureWidth() - oldWidth;

			for (uint w = currentCharCount - 1; w >= cursorPosition; --w)
			{
				widths[w] = widths[w - 1];
			}
			widths[cursorPosition - 1] = charWidth;

			delete[] tail;
			++charactersCopied;
		}
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
		for (uint w = selectionLeft; w < currentCharCount - 1; ++w)
		{
			widths[w] = widths[w + selectionSize];
		}
		for (uint w = 0; w < selectionSize; ++w)
			widths[currentCharCount - 1] = 0;

		currentCharCount -= selectionSize;
		cursorPosition = selectionStart = selectionEnd = selectionLeft;
		textLabel->SetLabelText(textContent);
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
