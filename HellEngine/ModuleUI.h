#ifndef __H_MODULE_UI__
#define __H_MODULE_UI__

#include "Module.h"
#include "ComponentUIElement.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"
#include "ComponentUiInputText.h"

class ModuleUI : public Module
{
public:
	ModuleUI();
	virtual ~ModuleUI();

	//UIElement Factory
	static ComponentUIElement * NewUIElement(const UIElementType &uiType, GameObject * goOwner)
	{
		if (uiType == UIElementType::IMG)
			return new ComponentUiImage(goOwner);
		if (uiType == UIElementType::BUTTON)
			return new ComponentUiButton(goOwner);
		if (uiType == UIElementType::LABEL)
			return new ComponentUiLabel(goOwner);
		if (uiType == UIElementType::INPUT_TEXT)
			return new ComponentUiInputText(goOwner);
		return NULL;
	}
};
#endif __H_MODULE_UI__

