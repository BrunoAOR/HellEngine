#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentType.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "GameObject.h"


ModuleScene::ModuleScene() 
{
}

ModuleScene::~ModuleScene() 
{
}

bool ModuleScene::Init()
{
	root = new GameObject("root", nullptr);
	CreateTestGameObjects();
	return true;
}

bool ModuleScene::CleanUp()
{
	delete root;
	root = nullptr;
	return true;
}

#include "Application.h"
#include "ModuleInput.h"
#include "globals.h"
UpdateStatus ModuleScene::Update()
{
	/* Parenting */
	if (App->input->GetKey(SDL_SCANCODE_0) == KeyState::KEY_DOWN)
	{
		go3->SetParent(nullptr);
		LOGGER("New parent: root");
	}
	if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
	{
		go3->SetParent(go1);
		LOGGER("New parent: go1 (Has Transform)");
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN)
	{
		go3->SetParent(go2);
		LOGGER("New parent: go2 (No Transform)");
	}

	/* Simulate Hierarchy selection */
	if (App->input->GetKey(SDL_SCANCODE_KP_0) == KeyState::KEY_DOWN)
	{
		editorInfo.selectedGameObject = nullptr;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KeyState::KEY_DOWN)
	{
		editorInfo.selectedGameObject = go1;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KeyState::KEY_DOWN)
	{
		editorInfo.selectedGameObject = go2;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KeyState::KEY_DOWN)
	{
		editorInfo.selectedGameObject = go3;
	}

	root->Update();
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleScene::OnEditorHierarchy(float mainMenuBarHeight, bool* pOpen)
{
	float windowWidth = 400;
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, App->window->getHeight() - mainMenuBarHeight));
	ImGui::Begin("Hierarchy", pOpen, ImGuiWindowFlags_NoCollapse);

	if (ImGui::TreeNodeEx(root, 0, "Scene"))
	{
		for (GameObject* child : root->GetChildren())
		{
			child->OnEditorHierarchy();
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleScene::OnEditorInspector(float mainMenuBarHeight, bool * pOpen)
{
	float windowWidth = 400;
	ImGui::SetNextWindowPos(ImVec2(App->window->getWidth() - windowWidth, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, App->window->getHeight() - mainMenuBarHeight));
	ImGui::Begin("Inspector", pOpen, ImGuiWindowFlags_NoCollapse);

	if (editorInfo.selectedGameObject == nullptr)
	{
		ImGui::TextWrapped("No GameObject has been selected.");
		ImGui::TextWrapped("Select a GameObject in the Hierachy window to view its properties.");
	}
	else
	{
		editorInfo.selectedGameObject->OnEditor();
	}

	ImGui::End();
}

GameObject* ModuleScene::CreateGameObject()
{
    GameObject* gameObject = new GameObject("name", root);
    return gameObject;
}

void ModuleScene::Destroy(GameObject* gameObject) 
{
	/* Protect against destruction of the root */
	assert(gameObject != root);
	GameObject* parent = gameObject->GetParent();
	if (!parent)
		parent = root;
	
	parent->RemoveChild(gameObject);
	delete gameObject;
}

std::vector<GameObject*> ModuleScene::FindByName(const std::string& name, GameObject* gameObject)
{
    std::vector<GameObject*> ret;
    std::vector<GameObject*> rootChildren;

    if(gameObject == nullptr)
    {
        rootChildren = root->GetChildren();
    }
    else 
    {
        rootChildren = gameObject->GetChildren();
    }

    for (size_t i = 0; i < rootChildren.size(); i++) 
    {
        if (rootChildren[i]->name == name) ret.push_back(rootChildren[i]);
        std::vector<GameObject*> temp = FindByName(name, rootChildren[i]);
        ret.insert(std::end(ret), std::begin(temp), std::end(temp));
    }
    return ret;
}

#include "ComponentMaterial.h"
#include "ComponentTransform.h"
void ModuleScene::CreateTestGameObjects()
{
	go1 = new GameObject("Child 1", root);
	ComponentTransform* go1transform = (ComponentTransform*)go1->AddComponent(ComponentType::TRANSFORM);
	go1transform->SetPosition(0, 0, -2);
	go1transform->SetRotationDeg(0, 90, 0);
	go1transform->SetScale(1, 2, 1);
	{
		go1->AddComponent(ComponentType::MESH);

		ComponentMaterial* mat = (ComponentMaterial*)go1->AddComponent(ComponentType::MATERIAL);
		mat->SetVertexShaderPath("assets/shaders/defaultShader.vert");
		mat->SetFragmentShaderPath("assets/shaders/tintingShader.frag");
		mat->SetTexturePath("assets/images/lenna.png");
		mat->SetShaderDataPath("assets/shaders/tintingShader.shaderData");
		mat->Apply();
	}

	go2 = new GameObject("Child 2", root);
	GameObject* go2_2 = new GameObject("Child 2.1", go2);
	go2->SetParent(go2_2);

	/* go3 setup */
	go3 = new GameObject("Child 3", root);

	ComponentTransform* go3transform = (ComponentTransform*)go3->AddComponent(ComponentType::TRANSFORM);
	go3transform->SetPosition(0, 2, -2);
	go3transform->SetRotationDeg(0, 45, 0);
	go3transform->SetScale(1, 1, 1);
	//go3->SetParent(go1);

	go3->AddComponent(ComponentType::MESH);

	ComponentMaterial* mat = (ComponentMaterial*)go3->AddComponent(ComponentType::MATERIAL);
	mat->SetVertexShaderPath("assets/shaders/defaultShader.vert");
	mat->SetFragmentShaderPath("assets/shaders/tintingShader.frag");
	mat->SetTexturePath("assets/images/lenna.png");
	mat->SetShaderDataPath("assets/shaders/tintingShader.shaderData");
	mat->Apply();

	//go3->SetParent(nullptr);
	/* End of g03 setup */

	go4 = new GameObject("Child 4", root);
	go4->AddComponent(ComponentType::MESH);
	Component* toDelete = go4->AddComponent(ComponentType::MATERIAL);
	go4->RemoveComponent(toDelete);
	new GameObject("Child 4.1", go4);
	Destroy(go4);

	go5 = new GameObject("Child 5", go3);
}
