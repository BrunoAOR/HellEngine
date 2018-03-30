#ifndef __H_COMPONENT_UI_LABEL__
#define __H_COMPONENT_UI_LABEL__
#include <SDL_ttf.h>
#include "ComponentUIElement.h"
class ComponentTransform2D;


class ComponentUiLabel : public ComponentUIElement
{
public:
	ComponentUiLabel(GameObject* owner);
	virtual ~ComponentUiLabel();

	virtual void OnEditor() override;

	const float* GetColor() const;
	float GetColorIntensity() const;

	unsigned int GetTextureID();

public:
	ComponentTransform2D* transform2D = nullptr;

private:
	void UpdateTexture();
	void UpdateFontName(const std::string& newFontName);
	void UpdateFontSize(int newFontSize);
	void UpdateFont();

private:
	unsigned int textTextureID = 0;
	std::string fontName;
	int fontSize = 18;
	char labelText[1024] = "";
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float intensity = 1.0f;

	TTF_Font* font = nullptr;
};

#endif __H_COMPONENT_UI_LABEL__
