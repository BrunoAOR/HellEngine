#ifndef __H_COMPONENT_UI_INPUT_TEXT__
#define __H_COMPONENT_UI_INPUT_TEXT__

#include "ComponentUIElement.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"

class ComponentUiInputText : public ComponentUIElement
{
public:

	ComponentUiInputText(GameObject * owner);
	virtual ~ComponentUiInputText();

	void OnEditor();

	int GetMaxChars();
	void SetMaxChars(int maxCharsValue);
	bool GetIsPassword();
	void SetIsPassword(bool isPasswordValue);

private:

	ComponentUiImage *uiImage;
	ComponentUiLabel *uiLabel;
	int maxChars;
	bool isPassword;
};

#endif __H_COMPONENT_UI_INPUT_TEXT__