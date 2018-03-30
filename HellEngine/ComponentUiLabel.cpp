#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUiLabel.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleTrueFont.h"
#include "globals.h"
#include "openGL.h"


#include "ComponentTransform2D.h"

ComponentUiLabel::ComponentUiLabel(GameObject* owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_LABEL;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	transform2D = (ComponentTransform2D*)gameObject->GetComponent(ComponentType::TRANSFORM_2D);
	assert(transform2D);
}

ComponentUiLabel::~ComponentUiLabel()
{
	if (textTextureID != 0)
	{
		/* Release the textTexture */
		textTextureID = 0;
	}
}

void ComponentUiLabel::OnEditor()
{
	static int selectedFontOption = 0;
	static std::string options = "";
	static const std::vector<std::string>& fonts = App->fonts->GetAvailableFonts();
	static int guiFontSize = 0;

	selectedFontOption = -1;
	options = "";
	for (unsigned int i = 0; i < fonts.size(); ++i)
	{
		const std::string& name = fonts[i];
		if (fontName.compare(name) == 0)
			selectedFontOption = i;
		options += name;
		options += '\0';
	}
	options += '\0';

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		if (ImGui::Combo("Font", &selectedFontOption, options.c_str()))
			UpdateFontName(fonts[selectedFontOption]);

		guiFontSize = fontSize;
		if (ImGui::InputInt("Size", &guiFontSize))
			UpdateFontSize(guiFontSize);


		if (ImGui::InputTextMultiline("Label text", labelText, 1024))
			UpdateTexture();

		ImGui::ColorEdit4("Color", color);
		ImGui::DragFloat("Transparency", &intensity, 0.01f, 0.0f, 1.0f, "%.2f");
	}
}

const float* ComponentUiLabel::GetColor() const
{
	return color;
}

float ComponentUiLabel::GetColorIntensity() const
{
	return intensity;
}

unsigned int ComponentUiLabel::GetTextureID()
{
	return textTextureID;
}

void ComponentUiLabel::UpdateTexture()
{
	if (!font || IsEmptyString(labelText))
		return;

	static SDL_Color sdlColor{ 255, 255, 255, 0 };
	//sdlColor.r = (Uint8)(color[0] < 1.0f ? color[0] * 256 : 255);
	//sdlColor.g = (Uint8)(color[1] < 1.0f ? color[1] * 256 : 255);
	//sdlColor.b = (Uint8)(color[2] < 1.0f ? color[2] * 256 : 255);
	
	/*sdlColor.r = 255;
	sdlColor.g = 128;
	sdlColor.b = 64;
	sdlColor.a = 100;*/

	if (textTextureID)
	{
		glDeleteTextures(1, &textTextureID);
		textTextureID = 0;
	}

	SDL_Surface* surface = TTF_RenderText_Blended(font, labelText, sdlColor);
	assert(surface);

	int colorComponents = surface->format->BytesPerPixel;
	bool hasAlpha = colorComponents == 4;
	GLenum textureFormat;
	if (hasAlpha)
	{
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGBA;
		else
			textureFormat = GL_BGRA;
	}
	else
	{
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGB;
		else
			textureFormat = GL_BGR;
	}

	GLubyte* pix = (GLubyte*)surface->pixels;
	glGenTextures(1, &textTextureID);
	glBindTexture(GL_TEXTURE_2D, textTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, colorComponents, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	transform2D->SetSize(surface->w, surface->h);
	SDL_FreeSurface(surface);
	surface = nullptr;
}

void ComponentUiLabel::UpdateFontName(const std::string & newFontName)
{
	fontName = newFontName;

	UpdateFont();
}

void ComponentUiLabel::UpdateFontSize(int newFontSize)
{
	if (newFontSize < 0)
		fontSize = 0;
	else
		fontSize = newFontSize;

	UpdateFont();
}

void ComponentUiLabel::UpdateFont()
{
	if (font)
	{
		App->fonts->ReleaseFont(font);
	}

	font = App->fonts->GetFont(fontName, fontSize);
	assert(font);
	UpdateTexture();
}
