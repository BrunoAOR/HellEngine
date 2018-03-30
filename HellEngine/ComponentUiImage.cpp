#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentType.h"
#include "ComponentUIElement.h"
#include "ComponentUiImage.h"
#include "GameObject.h"
#include "ModuleTextureManager.h"
#include "globals.h"

ComponentUiImage::ComponentUiImage(GameObject * owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_IMAGE;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	textureID = 0;
	transform2D = (ComponentTransform2D*)gameObject->GetComponent(ComponentType::TRANSFORM_2D);
	assert(transform2D);
}


ComponentUiImage::~ComponentUiImage()
{
	if (textureID != 0)
	{
		App->textureManager->ReleaseTexture(textureID);
		textureID = 0;
	}
}

void ComponentUiImage::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::InputText("Image path", imagePath, 256);
		if (ImGui::Button("Load image"))
			LoadImage();

		ImGui::ColorEdit4("Color", color);
		ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 1.0f, "%.2f");
	}
}

const float* ComponentUiImage::GetColor() const
{
	return color;
}

float ComponentUiImage::GetColorIntensity() const
{
	return intensity;
}

unsigned int ComponentUiImage::GetTextureID()
{
	return textureID;
}

void ComponentUiImage::SetTextureID(GLuint textureIDValue)
{
	textureID = textureIDValue;
}

bool ComponentUiImage::SetImagePath(const std::string& newImagePath)
{
	if (newImagePath.size() > 255)
	{
		LOGGER("ERROR: ComponentUiImage - imagePath is longer than 255 characters!");
		return false;
	}

	memcpy_s(imagePath, 256, newImagePath.c_str(), newImagePath.size());
	imagePath[newImagePath.size()] = '\0';
	return LoadImage();
}

bool ComponentUiImage::LoadImage()
{
	if (textureID != 0)
	{
		App->textureManager->ReleaseTexture(textureID);
		textureID = 0;
	}
	textureID = App->textureManager->GetTexture(imagePath);
	
	return textureID != 0;
}
