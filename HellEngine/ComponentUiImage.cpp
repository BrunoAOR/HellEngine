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
	uiType = UIElementType::IMG;
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

		int pos[2] = { rect.x, rect.y };
		int size[2] = { rect.w, rect.h };
		if (ImGui::DragInt2("Position", pos, 0.3f))
		{
			rect.x = pos[0];
			rect.y = pos[1];
		}
		if (ImGui::DragInt2("Size", size, 0.3f))
		{
			rect.w = size[0];
			rect.h = size[1];
		}

		ImGui::InputText("Image path", imagePath, 256);
		if (ImGui::Button("Load image"))
			LoadImage();
	}
}

unsigned int ComponentUiImage::GetTextureID()
{
	return textureID;
}

void ComponentUiImage::SetTextureID(GLuint textureIDValue)
{
	textureID = textureIDValue;
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
