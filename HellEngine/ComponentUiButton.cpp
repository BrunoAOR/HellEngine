#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUiButton.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleTextureManager.h"
#include "globals.h"

ComponentUiButton::ComponentUiButton(GameObject * owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_BUTTON;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	
	/* Set up default colors for states */
	float color = 1.0f;
	for (uint s = 0; s < 4; ++s)
	{
		for (uint i = 0; i < 3; ++i)
		{
			buttonStates[s].color[i] = color - 0.25f * s;
		}
		buttonStates[s].color[3] = 1.0f;
	}
}

ComponentUiButton::~ComponentUiButton()
{
	for (uint i = 0; i < 4; ++i)
	{
		ButtonState& buttonState = buttonStates[i];
		if (buttonState.textureId != 0)
			App->textureManager->ReleaseTexture(buttonState.textureId);

		buttonState.textureId = 0;
	}
}

void ComponentUiButton::OnEditor()
{
	static int transitionTypeIndex = 0;

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		transitionTypeIndex = (int)currentButtonTransitionType;
		if (ImGui::Combo("Transition Type", &transitionTypeIndex, "Color\0Sprite Change\0\0"))
		{
			SetTransitionType((ButtonTransitionType)transitionTypeIndex);
		}

		switch (currentButtonTransitionType)
		{
		case ButtonTransitionType::COLOR:
			if (ImGui::ColorEdit4("Default color", buttonStates[0].color))
				SetTransitionColor(ButtonStatus::DEFAULT, buttonStates[0].color);
			if (ImGui::ColorEdit4("Hover color", buttonStates[1].color))
				SetTransitionColor(ButtonStatus::HOVER, buttonStates[1].color);
			if (ImGui::ColorEdit4("Pressed color", buttonStates[2].color))
				SetTransitionColor(ButtonStatus::PRESSED, buttonStates[2].color);
			if (ImGui::ColorEdit4("Disabled color", buttonStates[3].color))
				SetTransitionColor(ButtonStatus::DISABLED, buttonStates[3].color);
			break;
		case ButtonTransitionType::SPRITE:
			ImGui::InputText("Default Sprite", buttonStates[0].imagePath, 256);
			ImGui::InputText("Hover Sprite", buttonStates[1].imagePath, 256);
			ImGui::InputText("Pressed Sprite", buttonStates[2].imagePath, 256);
			ImGui::InputText("Disabled Sprite", buttonStates[3].imagePath, 256);
			if (ImGui::Button("Load"))
			{
				SetTransitionImage(ButtonStatus::DEFAULT, buttonStates[0].imagePath);
				SetTransitionImage(ButtonStatus::HOVER, buttonStates[1].imagePath);
				SetTransitionImage(ButtonStatus::PRESSED, buttonStates[2].imagePath);
				SetTransitionImage(ButtonStatus::DISABLED, buttonStates[3].imagePath);
			}
			break;
		}
	}
}

void ComponentUiButton::SetTargetImage(ComponentUiImage* componentUiImage)
{
	uiImage = componentUiImage;
	UpdateUiImage();
}

ButtonStatus ComponentUiButton::GetButtonStatus()
{
	return currentButtonStatus;
}

void ComponentUiButton::SetButtonStatus(ButtonStatus buttonStatus)
{
	if (currentButtonStatus != buttonStatus)
	{
		activeButtonState = (int)buttonStatus;
		UpdateUiImage();
		currentButtonStatus = buttonStatus;
	}
}

ButtonTransitionType ComponentUiButton::GetTransitionType() const
{
	return currentButtonTransitionType;
}

void ComponentUiButton::SetTransitionType(ButtonTransitionType buttonTransitionType)
{
	if (currentButtonTransitionType != buttonTransitionType)
	{
		currentButtonTransitionType = buttonTransitionType;
		UpdateUiImage();
	}
}

void ComponentUiButton::SetTransitionColor(ButtonStatus buttonStatus, const Color& color)
{
	SetTransitionColor(buttonStatus, color.ptr());
}

void ComponentUiButton::SetTransitionColor(ButtonStatus buttonStatus, const float* color)
{
	ButtonState& buttonState = buttonStates[(int)buttonStatus];
	buttonState.color[0] = color[0];
	buttonState.color[1] = color[1];
	buttonState.color[2] = color[2];
	buttonState.color[3] = color[3];

	if (buttonStatus == currentButtonStatus)
		UpdateUiImage();
}

bool ComponentUiButton::SetTransitionImage(ButtonStatus buttonStatus, const char* path)
{
	bool success = false;
	ButtonState& buttonState = buttonStates[(int)buttonStatus];
	uint textureId = App->textureManager->GetTexture(path);


	if (textureId != 0)
	{
		if (buttonState.textureId != 0)
		{
			App->textureManager->ReleaseTexture(buttonState.textureId);
		}

		buttonState.textureId = textureId;
		memcpy_s(buttonState.imagePath, 256, path, 256);
		success = true;
	}
	else
	{
		memset(buttonState.imagePath, 0, 256);
		success = false;
	}

	if (currentButtonStatus == buttonStatus)
		UpdateUiImage();

	return success;
}

void ComponentUiButton::UpdateUiImage()
{
	if (uiImage)
	{
		if (currentButtonTransitionType == ButtonTransitionType::COLOR)
		{
			uiImage->SetImagePath(buttonStates[(int)ButtonStatus::DEFAULT].imagePath);
			uiImage->SetColor(buttonStates[activeButtonState].color);
		}
		else if (currentButtonTransitionType == ButtonTransitionType::SPRITE)
			uiImage->SetImagePath(buttonStates[activeButtonState].imagePath);
	}
}
