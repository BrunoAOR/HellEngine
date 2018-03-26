#ifndef __H_COMPONENT_UI_IMAGE__
#define __H_COMPONENT_UI_IMAGE__

#include "ComponentUIElement.h"

typedef unsigned int GLuint;

class ComponentUiImage : public ComponentUIElement
{
public:

	ComponentUiImage(GameObject * owner);
	virtual ~ComponentUiImage();

	virtual void OnEditor() override;

	unsigned int GetTextureID();
	void SetTextureID(GLuint textureIDValue);

	bool LoadImage();

	char imagePath[256] = "";


private:
	//bool LoadImage();

private:
	unsigned int textureID = 0;
	//char imagePath[256] = "";
};

#endif __H_COMPONENT_UI_IMAGE__

