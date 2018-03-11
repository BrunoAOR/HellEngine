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

	GLuint GetTextureID();
	void SetTextureID(GLuint textureIDValue);

private:
	GLuint textureID;
};

#endif __H_COMPONENT_UI_IMAGE__

