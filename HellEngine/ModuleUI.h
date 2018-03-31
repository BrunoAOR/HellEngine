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
	static GameObject* NewUIElement(UIElementType uiType);

    GameObject* GetClickedGameObject();
    GameObject* GetHoveringGameObject();
	void RegisterUiElement(ComponentUIElement* newUiElement);
	void UnregisterUiElement(ComponentUIElement* uiElement);

private:
	void GenerateSquareMeshInfo();
    void UpdateElements();
    void UpdateComponent(ComponentUIElement* component);
	void UpdateButton(ComponentUiButton* button);
	void UpdateImage(ComponentUiImage* image);
	void UpdateInputText(ComponentUiInputText* inputText);
	void UpdateLabel(ComponentUiLabel* label);

private:
	std::vector<ComponentUIElement*> uiElements;

    GameObject* canvas = nullptr;
    GameObject* clicked = nullptr;
    GameObject* hovering = nullptr;

	const ShaderProgram* shaderProgram = nullptr;
	uint colorUniformLocation = -1;
	uint intensityUniformLocation = -1;

	const ShaderProgram* textShaderProgram = nullptr;
	uint textColorUniformLocation = -1;

	MeshInfo squareMeshInfo;
};
#endif __H_MODULE_UI__

