#include "Application.h"
#include "ComponentUiImage.h"
#include "ModuleTextureManager.h"
#include "SerializableArray.h"
#include "SerializableObject.h"
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
		TransitionStateInfo& stateInfo = stateInfos[activeStateInfo];
		TransitionStateInfo& defautStateInfo = stateInfos[(int)TransitionState::DEFAULT];
		if (currentTransitionType == TransitionType::COLOR)
		{
			if(defautStateInfo.textureId != 0)
				uiImage->SetImagePath(defautStateInfo.imagePath);
			uiImage->SetColor(stateInfo.color);
		}
		else if (currentTransitionType == TransitionType::SPRITE)
			if (stateInfo.textureId != 0)
				uiImage->SetImagePath(stateInfo.imagePath);
	}
}

void UITransitionsHandler::Save(SerializableObject& obj) const
{
	obj.Addu32("TransitionImageUUID", uiImage ? uiImage->GetUUID() : 0);

	SerializableArray stateInfosArray = obj.BuildSerializableArray("StateInfos");
	for (uint i = 0; i < 4; ++i)
	{
		TransitionStateInfo info = stateInfos[i];
		SerializableObject sObj = stateInfosArray.BuildSerializableObject();
		sObj.AddColor("Color", Color(info.color[0], info.color[1], info.color[2], info.color[3]));
		sObj.AddString("ImagePath", info.imagePath);
	}
}

void UITransitionsHandler::Load(const SerializableObject& obj)
{
	SerializableArray stateInfosArray = obj.GetSerializableArray("StateInfos");
	for (uint i = 0; i < 4; ++i)
	{
		TransitionStateInfo info = stateInfos[i];
		SerializableObject sObj = stateInfosArray.GetSerializableObject(i);
		Color objColor = sObj.GetColor("Color");
		SetTransitionColor((TransitionState)i, objColor);
				
		std::string objImagePath = sObj.GetString("ImagePath");
		SetTransitionImage((TransitionState)i, objImagePath.c_str());
	}
}

void UITransitionsHandler::LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated)
{
	u32 uiImageUUID = obj.Getu32("TransitionImageUUID");
	if (uiImageUUID != 0)
	{
		assert(componentsCreated.count(uiImageUUID) == 1);
		ComponentUiImage* newUiImage = (ComponentUiImage*)componentsCreated.at(uiImageUUID);
		SetTargetImage(newUiImage);
	}
}
