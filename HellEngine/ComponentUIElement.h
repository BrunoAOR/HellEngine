#ifndef __H_COMPONENT_UI_ELEMENT__
#define __H_COMPONENT_UI_ELEMENT__

#include "Component.h"
#include "SDL\include\SDL_rect.h"
class ComponentTransform2D;

enum class UIElementType { IMG, LABEL, BUTTON, INPUT_TEXT };

extern const UIElementType UI_ELEMENT_TYPES[4];
const char* GetUITypeString(UIElementType uiType);

class ComponentUIElement : public Component
{
public:

	ComponentUIElement(GameObject* owner);
	virtual ~ComponentUIElement() override;

	
	bool GetVisible() const;
	void SetVisible(bool visibleValue);


	// Inherited via Component
	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

public:
	ComponentTransform2D* transform2D = nullptr;

protected:
	bool visible;
};

#endif // !__H_COMPONENT_UI_ELEMENT__