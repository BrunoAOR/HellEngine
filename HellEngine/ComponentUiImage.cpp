#include "ComponentUiImage.h"

ComponentUiImage::ComponentUiImage(GameObject * owner) : ComponentUIElement(owner)
{
}


ComponentUiImage::~ComponentUiImage()
{
}

void ComponentUiImage::OnEditor()
{

}

GLuint ComponentUiImage::GetTextureID()
{
	return textureID;
}

void ComponentUiImage::SetTextureID(GLuint textureIDValue)
{
	textureID = textureIDValue;
}
