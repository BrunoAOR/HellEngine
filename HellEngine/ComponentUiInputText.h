#ifndef __H_COMPONENT_UI_INPUT_TEXT__
#define __H_COMPONENT_UI_INPUT_TEXT__

#include "ComponentUIElement.h"
#include "UITransitionsHandler.h"
class ComponentUiImage;
class ComponentUiLabel;

class ComponentUiInputText : public ComponentUIElement
{
public:
	ComponentUiInputText(GameObject * owner);
	virtual ~ComponentUiInputText();

	void OnEditor();

	void SetTargetPlaceholderLabel(ComponentUiLabel* placeholderLabel);
	void SetTargetTextLabel(ComponentUiLabel* textLabel);
	void SetTargetCaretImage(ComponentUiImage* caretImage);

	unsigned int GetMaxChars();
	void SetMaxChars(unsigned int maxCharsValue);
	bool GetIsPassword();
	void SetIsPassword(bool isPasswordValue);

public:
	UITransitionsHandler transitionHandler;

private:
	ComponentUiImage* backgroundImage;
	ComponentUiLabel* placeholderLabel;
	ComponentUiLabel* textLabel;
	ComponentUiImage* caretImage;

	unsigned int maxChars;
	bool isPassword;
};

#endif // !__H_COMPONENT_UI_INPUT_TEXT__
