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

	void UpdateTextField();

	void SetTargetPlaceholderLabel(ComponentUiLabel* placeholderLabel);
	void SetTargetTextLabel(ComponentUiLabel* textLabel);
	void SetTargetCaretImage(ComponentUiImage* caretImage);

	bool GetFocusState() const;
	void SetFocusState(bool focusState);

	unsigned int GetMaxChars();
	void SetMaxChars(unsigned int maxCharsValue);
	bool GetIsPassword();
	void SetIsPassword(bool isPasswordValue);

public:
	UITransitionsHandler transitionHandler;

private:
	void AddNewText(const char* newText);

private:
	ComponentUiImage* backgroundImage;
	ComponentUiLabel* placeholderLabel;
	ComponentUiLabel* textLabel;
	ComponentUiImage* caretImage;

	char textContent[256] = "";
	unsigned int currentCharCount = 0;
	unsigned int cursorPosition = 0;
	
	bool hasFocus = false;
	unsigned int maxChars;
	bool isPassword;
};

#endif // !__H_COMPONENT_UI_INPUT_TEXT__
