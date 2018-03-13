#ifndef __H_MODULE_UI__
#define __H_MODULE_UI__

#include "Module.h"
#include "ComponentUIElement.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"
#include "ComponentUiInputText.h"

class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI();
	virtual ~ModuleUI();

    UpdateStatus Update();

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
		return nullptr;
	}

    GameObject* GetClickedGameObject();
    GameObject* GetHoveringGameObject();

private:
    void UpdateElements();
    void UpdateComponent(Component* component);
    void UpdateImage(ComponentUiImage* image);
    void UpdateButton(ComponentUiButton* button);
    void UpdateLabel(ComponentUiLabel* label);
    void UpdateInputText(ComponentUiInputText* inputText);

private:
    GameObject* canvas = nullptr;
    GameObject* clicked = nullptr;
    GameObject* hovering = nullptr;
};
#endif __H_MODULE_UI__

