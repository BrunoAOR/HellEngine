#ifndef __H_COMPONENT_UI_ELEMENT__
#define __H_COMPONENT_UI_ELEMENT__

#include "Component.h"
#include "SDL\include\SDL_rect.h"

enum class UIElementType {NONE, IMG, LABEL, BUTTON, INPUT_TEXT};
const UIElementType UI_TYPES[] = { UIElementType::NONE, UIElementType::IMG, UIElementType::LABEL, UIElementType::BUTTON, UIElementType::INPUT_TEXT };

class ComponentUIElement : public Component
{
public:

	ComponentUIElement(GameObject* owner);
	virtual ~ComponentUIElement() override;

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	static const char* GetUITypeString(UIElementType uiType);
	
	UIElementType GetUiType() const;
	const SDL_Rect& GetRect() const;
	void SetRect(const SDL_Rect& rectValue);
	bool GetVisible() const;
	void SetVisible(bool visibleValue);

protected:
	UIElementType uiType;

	SDL_Rect rect;
	bool visible;

};

#endif __H_COMPONENT_UI_ELEMENT__