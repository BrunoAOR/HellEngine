#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleTextureManager.h"
#include "globals.h"

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
	static int transitionTypeIndex = 0;

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

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
	}
}
