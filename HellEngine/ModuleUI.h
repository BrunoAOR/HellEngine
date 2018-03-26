#ifndef __H_MODULE_UI__
#define __H_MODULE_UI__

#include <vector>
#include <map>
#include "Module.h"
#include "ComponentUIElement.h"
#include "MeshInfo.h"
class ComponentUiImage;
class ComponentUiButton;
class ComponentUiLabel;
class ComponentUiInputText;
class GameObject;
class ShaderProgram;

class ModuleUI : public Module
{
public:
	ModuleUI();
	virtual ~ModuleUI();
	
	virtual bool Init() override;
	virtual bool CleanUp() override;
    UpdateStatus Update();

    //UIElement Factory
	static ComponentUIElement* NewUIElement(UIElementType uiType, GameObject* goOwner);

    GameObject* GetClickedGameObject();
    GameObject* GetHoveringGameObject();
	void RegisterUiElement(ComponentUIElement* newUiElement);
	void UnregisterUiElement(ComponentUIElement* uiElement);

private:
	void GenerateSquareMeshInfo();
    void UpdateElements();
    void UpdateComponent(ComponentUIElement* component);
    void UpdateImage(ComponentUiImage* image);
    void UpdateButton(ComponentUiButton* button);
    void UpdateLabel(ComponentUiLabel* label);
    void UpdateInputText(ComponentUiInputText* inputText);

private:
	std::vector<ComponentUIElement*> uiElements;

    GameObject* canvas = nullptr;
    GameObject* clicked = nullptr;
    GameObject* hovering = nullptr;

	const ShaderProgram* shaderProgram = nullptr;
	MeshInfo squareMeshInfo;
};
#endif __H_MODULE_UI__

