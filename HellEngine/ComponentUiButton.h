#ifndef __H_COMPONENT_UI_BUTTON__
#define __H_COMPONENT_UI_BUTTON__
#include "ComponentUIElement.h"
#include "UITransitionsHandler.h"

class ComponentUiButton : public ComponentUIElement
{
public:

	ComponentUiButton(GameObject* owner);
	virtual ~ComponentUiButton();

	virtual void OnEditor() override;
		
public:
	UITransitionsHandler transitionHandler;
};

#endif // !__H_COMPONENT_UI_BUTTON__
