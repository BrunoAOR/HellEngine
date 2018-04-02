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
	void SetTargetSelectionImage(ComponentUiImage* selectionImage);
	void SetTargetCaretImage(ComponentUiImage* caretImage);

	bool GetFocusState() const;
	void SetFocusState(bool focusState);

	unsigned int GetMaxChars();
	bool GetIsPassword();
	void SetIsPassword(bool isPasswordValue);

public:
	UITransitionsHandler transitionHandler;

private:
	void HandleCursorMotion();
	void HandleDeletion();
	void HandleClipboard();
	void UpdateCaret();
	void AddNewText(const char* newText);
	void DeleteSelection();
	void HandleShiftKey();

private:
	static const unsigned int maxChars = 256;

	ComponentUiImage* backgroundImage;
	ComponentUiLabel* placeholderLabel;
	ComponentUiLabel* textLabel;
	ComponentUiImage* selectionImage;
	ComponentUiImage* caretImage;

	char textContent[maxChars] = "";
	unsigned int currentCharCount = 0;
	unsigned int cursorPosition = 0;
	unsigned int selectionStart = 0;
	unsigned int selectionEnd = 0;
	
	int widths[maxChars];
	float caretBlinkRate = 1;
	float caretElapsedTime;

	bool hasFocus = false;
	bool isPassword;

	float selectionColor[4]{ 0.5f, 1.0f, 1.0f, 0.5f };
};

#endif // !__H_COMPONENT_UI_INPUT_TEXT__
