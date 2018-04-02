#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentUiLabel.h"
#include "ComponentType.h"
#include "ModuleTrueFont.h"
#include "globals.h"
#include "openGL.h"


#include "ComponentTransform2D.h"

ComponentUiLabel::ComponentUiLabel(GameObject* owner) : ComponentUIElement(owner)
{
	type = ComponentType::UI_LABEL;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentUiLabel::~ComponentUiLabel()
{
	if (textTextureID)
	{
		glDeleteTextures(1, &textTextureID);
		textTextureID = 0;
	}
	if (font)
	{
		App->fonts->ReleaseFont(font);
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
			SetFontName(fonts[selectedFontOption]);

		guiFontSize = fontSize;
		if (ImGui::InputInt("Size", &guiFontSize))
			SetFontSize(guiFontSize);

		if (ImGui::Checkbox("Adapt Size to text", &adaptSizeToText))
			ResizeTransform();

		if (ImGui::InputTextMultiline("Label text", labelText, 1024))
			UpdateTexture();

		ImGui::ColorEdit4("Color", color);
	}
}

const char* ComponentUiLabel::GetLabelText() const
{
	return labelText;
}

bool ComponentUiLabel::SetLabelText(const std::string& newText)
{
	if (newText.length() > 1023)
		return false;

	if (strcmp(newText.c_str(), labelText) != 0)
	{
		memcpy_s(labelText, 1024, newText.c_str(), newText.length());
		labelText[newText.length()] = '\0';
		UpdateTexture();
	}
	return true;
}

int ComponentUiLabel::GetFontSize() const
{
	return fontSize;
}

void ComponentUiLabel::SetFontSize(int newFontSize)
{
	if (newFontSize < 0)
		fontSize = 0;
	else
		fontSize = newFontSize;

	UpdateFont();
}

const float* ComponentUiLabel::GetColor() const
{
	return color;
}

void ComponentUiLabel::SetColor(float r, float g, float b, float a)
{
	color[0] = r < 0 ? 0 : (r > 1.0f ? 1.0f : r);
	color[1] = g < 0 ? 0 : (g > 1.0f ? 1.0f : g);
	color[2] = b < 0 ? 0 : (b > 1.0f ? 1.0f : b);
	color[3] = a < 0 ? 0 : (a > 1.0f ? 1.0f : a);
}

const std::string& ComponentUiLabel::GetFontName() const
{
	return fontName;
}

bool ComponentUiLabel::SetFontName(const std::string& newFontName)
{
	static const std::vector<std::string>& fonts = App->fonts->GetAvailableFonts();
	for (unsigned int i = 0; i < fonts.size(); ++i)
	{
		const std::string& name = fonts[i];
		if (newFontName.compare(name) == 0)
		{
			fontName = newFontName;
			UpdateFont();
			return true;
		}
	}
	return false;
}

bool ComponentUiLabel::GetAdaptSizeToText() const
{
	return adaptSizeToText;
}

void ComponentUiLabel::SetAdaptSizeToText(bool shouldAdapt)
{
	adaptSizeToText = shouldAdapt;
	ResizeTransform();
}

int ComponentUiLabel::GetTextureWidth() const
{
	return textureSize.x;
}

int ComponentUiLabel::GetTextureHeight() const
{
	return textureSize.y;
}

unsigned int ComponentUiLabel::GetTextureID()
{
	return textTextureID;
}

void ComponentUiLabel::UpdateFont()
{
	if (font)
	{
		App->fonts->ReleaseFont(font);
	}

	if (!fontName.empty())
	{
		font = App->fonts->GetFont(fontName, fontSize);
		assert(font);
		UpdateTexture();
	}
}

void ComponentUiLabel::UpdateTexture()
{
	static SDL_Color sdlColor{ 255, 255, 255, 0 };

	if (!font)
		return;

	if (textTextureID)
	{
		glDeleteTextures(1, &textTextureID);
		textTextureID = 0;
		textureSize.x = textureSize.y = 0;
	}

	if (IsEmptyString(labelText))
		return;

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

	/*
	TTF_Render functions return texture where the origin is in the top left,
	while OpenGL expects the origin to be in the bottom left.
	So, the resulting pixels must be flipped
	*/
	char* originalPixels = (char*)surface->pixels;
	char* flippedPixels = new char[surface->h * surface->w * colorComponents];
	uint maxW = surface->w * colorComponents;
	uint maxH = surface->h;
	for (uint w = 0; w < maxW; ++w)
		for (uint h = 0; h < maxH; ++h)
			flippedPixels[h * maxW + w] = originalPixels[(maxH - h - 1) * maxW + w];

	glGenTextures(1, &textTextureID);
	glBindTexture(GL_TEXTURE_2D, textTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, colorComponents, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, flippedPixels);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	textureSize.x = surface->w;
	textureSize.y = surface->h;
	ResizeTransform();

	SDL_FreeSurface(surface);
	surface = nullptr;
	delete[] flippedPixels;
	flippedPixels = nullptr;
}

void ComponentUiLabel::ResizeTransform()
{
	if (adaptSizeToText)
	{
		transform2D->SetSize(textureSize.x, textureSize.y);
	}
}
