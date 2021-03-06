#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "Color.h"
#include "ComponentType.h"
#include "ComponentUIElement.h"
#include "ComponentUiImage.h"
#include "ModuleTextureManager.h"
#include "SerializableObject.h"
#include "globals.h"

ComponentUiImage::ComponentUiImage(GameObject* owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_IMAGE;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
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
	}
}

const float* ComponentUiImage::GetColor() const
{
	return color;
}

void ComponentUiImage::SetColor(float* newColor)
{
	color[0] = newColor[0];
	color[1] = newColor[1];
	color[2] = newColor[2];
	color[3] = newColor[3];
}

void ComponentUiImage::SetColor(float r, float g, float b, float a)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

unsigned int ComponentUiImage::GetTextureID()
{
	return textureID;
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

void ComponentUiImage::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	obj.AddString("ImagePath", imagePath);
	obj.AddColor("Color", Color(color[0], color[1], color[2], color[3]));
}

void ComponentUiImage::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	std::string objImagePath = obj.GetString("ImagePath");
	SetImagePath(objImagePath);
	Color colorObj = obj.GetColor("Color");
	SetColor(colorObj.r, colorObj.g, colorObj.b, colorObj.a);
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
