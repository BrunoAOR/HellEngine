#ifndef __H_COMPONENT_UI_LABEL__
#define __H_COMPONENT_UI_LABEL__
#include <SDL_ttf.h>
#include "ComponentUIElement.h"
#include "Point.h"


class ComponentUiLabel : public ComponentUIElement
{
public:
	ComponentUiLabel(GameObject* owner);
	virtual ~ComponentUiLabel();

	virtual void OnEditor() override;

	const char* GetLabelText() const;
	bool SetLabelText(const std::string& newText);
	int GetFontSize() const;
	void SetFontSize(int newFontSize);
	const float* GetColor() const;
	void SetColor(float r, float g, float b, float a);
	const std::string& GetFontName() const;
	bool SetFontName(const std::string& newFontName);
	bool GetAdaptSizeToText() const;
	void SetAdaptSizeToText(bool shouldAdapt);
	
	int GetTextureWidth() const;
	int GetTextureHeight() const;

	unsigned int GetTextureID();

private:
	void UpdateFont();
	void UpdateTexture();
	void ResizeTransform();

private:
	unsigned int textTextureID = 0;
	iPoint textureSize;
	std::string fontName;
	int fontSize = 18;
	bool adaptSizeToText = false;
	char labelText[1024] = "";
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	TTF_Font* font = nullptr;
};

#endif // !__H_COMPONENT_UI_LABEL__
