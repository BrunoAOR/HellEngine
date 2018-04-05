#ifndef __H_COMPONENT_UI_INPUT_TEXT__
#define __H_COMPONENT_UI_INPUT_TEXT__
#include "ComponentUIElement.h"
#include "Point.h"
#include "UITransitionsHandler.h"
class ComponentUiImage;
class ComponentUiLabel;
class SerializableObject;

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

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;
	virtual void LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated) override;

public:
	UITransitionsHandler transitionHandler;

private:
	void HandleCursorMotion();
	void HandleDeletion();
	void HandleClipboard();
	void UpdateSelection();
	void UpdateCaret();
	void UpdateTextOffset();
	void AddNewText(const char* newText);
	void DeleteSelection();
	void HandleShiftKey();

private:
	static const unsigned int maxChars = 256;

	ComponentUiLabel* placeholderLabel;
	ComponentUiLabel* textLabel;
	ComponentUiImage* selectionImage;
	ComponentUiImage* caretImage;

	char textContent[maxChars] = "";
	unsigned int currentCharCount = 0;
	unsigned int cursorPosition = 0;
	unsigned int selectionStart = 0;
	unsigned int selectionEnd = 0;
	iPoint textOffset;
	
	int widths[maxChars];
	float caretBlinkRate = 1;
	float caretElapsedTime = 0;

	bool hasFocus = false;
	bool isPassword = false;
};

#endif // !__H_COMPONENT_UI_INPUT_TEXT__
