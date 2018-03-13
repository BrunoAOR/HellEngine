#ifndef __H_MODULE_UI__
#define __H_MODULE_UI__

#include "Module.h"

class ComponentUIElement;
class ComponentUiButton;
class ComponentUiImage;
class ComponentUiLabel;
class ComponentUiInputText;
class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI();
	virtual ~ModuleUI();

    UpdateStatus Update();

	//UIElement Factory
	//static ComponentUIElement * NewUIElement(const UIElementType &uiType, GameObject * goOwner)
	//{
	//	if (uiType == UIElementType::IMG)
	//		return new ComponentUiImage(goOwner);
	//	if (uiType == UIElementType::BUTTON)
	//		return new ComponentUiButton(goOwner);
	//	if (uiType == UIElementType::LABEL)
	//		return new ComponentUiLabel(goOwner);
	//	if (uiType == UIElementType::INPUT_TEXT)
	//		return new ComponentUiInputText(goOwner);
	//	return nullptr;
	//}
private:
    GameObject* canvas = nullptr;
};
#endif __H_MODULE_UI__

