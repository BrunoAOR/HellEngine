#include "Application.h"
#include "ComponentUiImage.h"
#include "ModuleTextureManager.h"
#include "UITransitionsHandler.h"
#include "globals.h"


UITransitionsHandler::UITransitionsHandler()
{
	/* Set up default colors for states */
	float color = 1.0f;
	for (uint s = 0; s < 4; ++s)
	{
		for (uint i = 0; i < 3; ++i)
		{
			stateInfos[s].color[i] = color - 0.25f * s;
		}
		stateInfos[s].color[3] = 1.0f;
	}
}


UITransitionsHandler::~UITransitionsHandler()
{
	for (uint i = 0; i < 4; ++i)
	{
		TransitionStateInfo& stateInfo = stateInfos[i];
		if (stateInfo.textureId != 0)
			App->textureManager->ReleaseTexture(stateInfo.textureId);

		stateInfo.textureId = 0;
	}
}

void UITransitionsHandler::SetTargetImage(ComponentUiImage* componentUiImage)
{
	uiImage = componentUiImage;
	UpdateUiImage();
}

TransitionState UITransitionsHandler::GetTransitionState()
{
	return currentTransitionState;
}

void UITransitionsHandler::SetTransitionState(TransitionState transitionState)
{
	if (currentTransitionState != transitionState)
	{
		activeStateInfo = (int)transitionState;
		UpdateUiImage();
		currentTransitionState = transitionState;
	}
}

TransitionType UITransitionsHandler::GetTransitionType() const
{
	return currentTransitionType;
}

void UITransitionsHandler::SetTransitionType(TransitionType transitionType)
{
	if (currentTransitionType != transitionType)
	{
		currentTransitionType = transitionType;
		UpdateUiImage();
	}
}

void UITransitionsHandler::SetTransitionColor(TransitionState transitionState, const Color& color)
{
	SetTransitionColor(transitionState, color.ptr());
}

void UITransitionsHandler::SetTransitionColor(TransitionState transitionState, const float* color)
{
	if (color)
	{
		TransitionStateInfo& stateInfo = stateInfos[(int)transitionState];
		stateInfo.color[0] = color[0];
		stateInfo.color[1] = color[1];
		stateInfo.color[2] = color[2];
		stateInfo.color[3] = color[3];
	}

	if (transitionState == currentTransitionState)
		UpdateUiImage();
}

bool UITransitionsHandler::SetTransitionImage(TransitionState transitionState, const char* path)
{
	bool success = false;
	TransitionStateInfo& stateInfo = stateInfos[(int)transitionState];
	
	uint textureId = 0;
	if (path)
		textureId = App->textureManager->GetTexture(path);
	else
		textureId = App->textureManager->GetTexture(stateInfo.imagePath);

	if (textureId != 0)
	{
		if (stateInfo.textureId != 0)
		{
			App->textureManager->ReleaseTexture(stateInfo.textureId);
		}

		stateInfo.textureId = textureId;
		if (path)
			memcpy_s(stateInfo.imagePath, 256, path, 256);
		success = true;
	}
	else
	{
		memset(stateInfo.imagePath, 0, 256);
		success = false;
	}

	if (currentTransitionState == transitionState)
		UpdateUiImage();

	return success;
}

void UITransitionsHandler::UpdateUiImage()
{
	if (uiImage)
	{
		if (currentTransitionType == TransitionType::COLOR)
		{
			uiImage->SetImagePath(stateInfos[(int)TransitionState::DEFAULT].imagePath);
			uiImage->SetColor(stateInfos[activeStateInfo].color);
		}
		else if (currentTransitionType == TransitionType::SPRITE)
			uiImage->SetImagePath(stateInfos[activeStateInfo].imagePath);
	}
}