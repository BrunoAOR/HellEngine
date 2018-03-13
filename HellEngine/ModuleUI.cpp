#include "ModuleUI.h"
#include "ComponentUIElement.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"
#include "ComponentUiInputText.h"
#include "GameObject.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL.h"

ModuleUI::ModuleUI()
{
}


ModuleUI::~ModuleUI()
{
}

UpdateStatus ModuleUI::Update() 
{
    if (canvas != nullptr)
    {
        //Check UI states ("pressed, etc...")
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_DEPTH_TEST);
    }
    return UpdateStatus::UPDATE_CONTINUE;
}
