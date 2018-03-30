#ifndef __H_COMPONENT_UI_IMAGE__
#define __H_COMPONENT_UI_IMAGE__
#include "ComponentUIElement.h"
class ComponentTransform2D;

class ComponentUiImage : public ComponentUIElement
{
public:

	ComponentUiImage(GameObject* owner);
	virtual ~ComponentUiImage();

	virtual void OnEditor() override;

	const float* GetColor() const;
	float GetColorIntensity() const;

	unsigned int GetTextureID();

	bool SetImagePath(const std::string& newImagePath);

public:
	ComponentTransform2D* transform2D = nullptr;

private:
	bool LoadImage();

private:
	unsigned int textureID = 0;
	char imagePath[256] = "";
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float intensity = 0.0f;
};

#endif __H_COMPONENT_UI_IMAGE__

