#ifndef __H_COMPONENT_UI_IMAGE__
#define __H_COMPONENT_UI_IMAGE__
#include "ComponentUIElement.h"

class ComponentUiImage : public ComponentUIElement
{
public:

	ComponentUiImage(GameObject* owner);
	virtual ~ComponentUiImage();

	virtual void OnEditor() override;

	const float* GetColor() const;
	void SetColor(float* newColor);
	void SetColor(float r, float g, float b, float a);

	unsigned int GetTextureID();

	bool SetImagePath(const std::string& newImagePath);

private:
	bool LoadImage();

private:
	unsigned int textureID = 0;
	char imagePath[256] = "";
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif // !__H_COMPONENT_UI_IMAGE__

