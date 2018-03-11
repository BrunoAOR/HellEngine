#ifndef __H_COMPONENT_UI_ELEMENT__
#define __H_COMPONENT_UI_ELEMENT__

#include "Component.h"
#include "SDL\include\SDL_rect.h"

enum class UIElementType {IMG, LABEL, BUTTON, INPUT_TEXT};
const UIElementType UI_TYPES[] = { UIElementType::IMG, UIElementType::LABEL, UIElementType::BUTTON, UIElementType::INPUT_TEXT };

class ComponentUIElement : public Component
{
public:

	ComponentUIElement(GameObject* owner);
	~ComponentUIElement() override;

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	static const char* GetUITypeString(UIElementType uiType);
	
	SDL_Rect * GetRect();
	void SetRect(SDL_Rect * rectValue);
	bool GetVisible();
	void SetVisible(bool visibleValue);

private:

	UIElementType uiType;

	SDL_Rect* rect;
	bool visible;
};

#endif __H_COMPONENT_UI_ELEMENT__