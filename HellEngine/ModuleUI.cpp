#include <algorithm>
#include "SDL/include/SDL.h"
#include "Application.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentUiLabel.h"
#include "ComponentUiInputText.h"
#include "ComponentTransform2D.h"
#include "GameObject.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditorCamera.h"
#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleShaderManager.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "ShaderProgram.h"
#include "openGl.h"


ModuleUI::ModuleUI()
{
}


ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init()
{
	shaderProgram = App->shaderManager->GetShaderProgram("assets/shaders/defaultModelShader.vert", "assets/shaders/defaultModelShader.frag");
	if (!shaderProgram)
		return false;

	GenerateSquareMeshInfo();

	canvas = App->scene->CreateGameObject();
	canvas->name = "Canvas";

	/* Testing */
	GameObject* image = App->scene->CreateGameObject();
	image->name = "Image test";
	image->SetParent(canvas);

	ComponentTransform2D* imageTransform = (ComponentTransform2D*)image->AddComponent(ComponentType::TRANSFORM_2D);
	imageTransform->SetLocalPos(500, 200);
	imageTransform->SetSize(150, 150);

	ComponentUiImage* componentImage = (ComponentUiImage*)image->AddComponent(ComponentType::UI_IMAGE);
	componentImage->SetImagePath("assets/images/lenna.png");

	return true;
}

bool ModuleUI::CleanUp()
{
	App->shaderManager->ReleaseShaderProgram(shaderProgram);
	shaderProgram = nullptr;
	return true;
}

GameObject* ModuleUI::NewUIElement(UIElementType uiType)
{
	GameObject* go = App->scene->CreateGameObject();
	go->name = GetUITypeString(uiType);
	go->AddComponent(ComponentType::TRANSFORM_2D);
	switch (uiType)
	{
	case UIElementType::IMG:
		go->AddComponent(ComponentType::UI_IMAGE);
		break;
	case UIElementType::LABEL:
		go->AddComponent(ComponentType::UI_LABEL);
		break;
	case UIElementType::BUTTON:
		assert(false);
		break;
	case UIElementType::INPUT_TEXT:
		assert(false);
		break;
	}

	return go;
}

UpdateStatus ModuleUI::Update()
{
    if (canvas != nullptr)
    {
        //Check UI states ("pressed, etc...")
        glDisable(GL_DEPTH_TEST);
		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, App->window->GetWidth(), 0, App->window->GetHeight(), -1, 1);
		glMatrixMode(GL_MODELVIEW);*/

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

void ModuleUI::RegisterUiElement(ComponentUIElement* newUiElement)
{
	uiElements.push_back(newUiElement);
}

void ModuleUI::UnregisterUiElement(ComponentUIElement* uiElement)
{
	std::vector<ComponentUIElement*>::iterator it = find(uiElements.begin(), uiElements.end(),uiElement);
	if (it != uiElements.end())
	{
		uiElements.erase(it);
	}
}

void ModuleUI::GenerateSquareMeshInfo()
{
	/* Data has 4 groups of xPos, yPos, zPos, xNorm, yNorm, zNorm, u, v */
	squareMeshInfo.name = "square";
	
	float data[8 * 4] = {
		0, 0, 1,		0, 0, 1,	0, 0,
		1, 0, 1,		0, 0, 1,	1, 0, 
		1, 1, 1,		0, 0, 1,	1, 1,
		0, 1, 1,		0, 0, 1,	0, 1 };

	const int elementsCount = 6;
	squareMeshInfo.elementsCount = elementsCount;
	uint indices[elementsCount] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &squareMeshInfo.vao);
	glGenBuffers(1, &squareMeshInfo.vbo);
	glGenBuffers(1, &squareMeshInfo.ebo);

	glBindVertexArray(squareMeshInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, squareMeshInfo.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 8, data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareMeshInfo.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * squareMeshInfo.elementsCount, indices, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */
}

void ModuleUI::UpdateElements()
{
	if (canvas->GetActive())
	{
		for (ComponentUIElement* uiElement : uiElements)
		{
			if (uiElement->gameObject->GetActive())
				UpdateComponent(uiElement);
		}
	}
}

void ModuleUI::UpdateComponent(ComponentUIElement* component)
{
    switch (component->GetType())
    {
	case ComponentType::UI_IMAGE:
        UpdateImage((ComponentUiImage*) component);
        break;
	case ComponentType::UI_LABEL:
		UpdateLabel((ComponentUiLabel*)component);
		break;
	/*
    case UIElementType::BUTTON:
        UpdateButton((ComponentUiButton*)component);
        break;
    case UIElementType::LABEL:
        UpdateLabel((ComponentUiLabel*)component);
        break;
    case UIElementType::INPUT_TEXT:
        UpdateInputText((ComponentUiInputText*)component);
        break;
	*/
    }
}

void ModuleUI::UpdateImage(ComponentUiImage* image)
{
	uint textureID = image->GetTextureID();
	if (textureID)
	{
		shaderProgram->Activate();

		const fPoint& pos = image->transform2D->GetWorldPos();
		const fPoint& size = image->transform2D->GetSize();
		float rx = pos.x;
		float ry = pos.y;
		float rw = size.x;
		float rh = size.y;


		float modelMatrix[16] = {
			rw,	0,	0, 0,
			0,	rh,	0, 0,
			0,	0,	1, 0,
			rx,	ry,	0, 1
		};

		float identity[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		float w = (float)App->window->GetWidth();
		float h = (float)App->window->GetHeight();
		float n = 100;	// Near plane
		float f = -100;	// Far plane

		float orthoMatrix[16] = {
			2 / w,	0,	0,	-1,
			0,	2 / h,	0,	-1,
			0,	0,	-2 / (f - n),	-(f + n) / (f - n),
			0, 0, 0, 1
		};

		float orthoMatrixTrans[16] = {
			2 / w,	0,	0,	0,
			0,	2 / h,	0,	0,
			0,	0,	-2 / (f - n),	0,
			-1, -1, -(f + n) / (f - n), 1
		};

		/* Matrixes are given in this order: Model, View, Projection */
		shaderProgram->UpdateMatrixUniforms(modelMatrix, identity, orthoMatrixTrans);
		
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(squareMeshInfo.vao);
		glDrawElements(GL_TRIANGLES, squareMeshInfo.elementsCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, 0);

		shaderProgram->Deactivate();
	}
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
