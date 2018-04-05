#ifndef __H_COMPONENT_UI_BUTTON__
#define __H_COMPONENT_UI_BUTTON__
#include "ComponentUIElement.h"
#include "UITransitionsHandler.h"
class SerializableObject;

class ComponentUiButton : public ComponentUIElement
{
public:

	ComponentUiButton(GameObject* owner);
	virtual ~ComponentUiButton();

	virtual void OnEditor() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;
	virtual void LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated) override;

public:
	UITransitionsHandler transitionHandler;
};

#endif // !__H_COMPONENT_UI_BUTTON__
