#include "ModuleUI.h"
#include "GameObject.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL.h"

ModuleUI::ModuleUI()
{
}


ModuleUI::~ModuleUI()
{
}

ComponentUIElement* ModuleUI::NewUIElement(const UIElementType &uiType, GameObject* goOwner)
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

UpdateStatus ModuleUI::Update() 
{
    if (canvas != nullptr)
    {
        //Check UI states ("pressed, etc...")
        glDisable(GL_DEPTH_TEST);
        UpdateElements();
        glEnable(GL_DEPTH_TEST);
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

GameObject* ModuleUI::GetClickedGameObject()
{
    return clicked;
}
GameObject* ModuleUI::GetHoveringGameObject()
{
    return hovering;
}

void ModuleUI::UpdateElements()
{
    if (!canvas->GetActive()) return;

}

void ModuleUI::UpdateComponent(Component* component) 
{
    switch ((UIElementType)component->GetType())
    {
    case UIElementType::IMG:
        UpdateImage((ComponentUiImage*) component);
        break;
    case UIElementType::BUTTON:
        UpdateButton((ComponentUiButton*)component);
        break;
    case UIElementType::LABEL:
        UpdateLabel((ComponentUiLabel*)component);
        break;
    case UIElementType::INPUT_TEXT:
        UpdateInputText((ComponentUiInputText*)component);
        break;
    }
}

void ModuleUI::UpdateImage(ComponentUiImage* image)
{

}

void ModuleUI::UpdateButton(ComponentUiButton* button)
{

}

void ModuleUI::UpdateLabel(ComponentUiLabel* label)
{

}

void ModuleUI::UpdateInputText(ComponentUiInputText* inputText)
{

}
