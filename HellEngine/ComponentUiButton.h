#ifndef __H_COMPONENT_UI_BUTTON__
#define __H_COMPONENT_UI_BUTTON__

#include "ComponentUIElement.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"

enum class ButtonStatusType { STANDARD, HOVER, PRESSED, DISABLED };

class ComponentUiButton : public ComponentUIElement
{
public:

	ComponentUiButton(GameObject * owner);
	virtual ~ComponentUiButton();

	virtual void OnEditor() override;

	ButtonStatusType GetButtonStatus();
	void SetButtonStatus(ButtonStatusType buttonStatusTypeValue);

private:

	ButtonStatusType buttonStatusType;
	ComponentUiImage *uiImage;
	ComponentUiLabel *uiLabel;
};

#endif __H_COMPONENT_UI_BUTTON__