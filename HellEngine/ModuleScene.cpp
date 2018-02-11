#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentCamera.h"
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
	root->AddComponent(ComponentType::CAMERA);
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
	/* TEMPORARY CODE START */
	static bool init = false;
	if (!init)
	{
		TestQuadTree();
		init = true;
	}
	quadTree.DrawTree();
	/* TEMPORARY CODE END */

	root->Update();
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleScene::OnEditorHierarchy(float mainMenuBarHeight, bool* pOpen)
{
	float windowWidth = 400;
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, App->window->getHeight() - mainMenuBarHeight));
	ImGui::Begin("Hierarchy", pOpen, ImGuiWindowFlags_NoCollapse);

	root->OnEditorRootHierarchy();

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
		editorInfo.selectedGameObject->OnEditorInspector();
	}

	ImGui::End();
}

void ModuleScene::SetActiveGameCamera(ComponentCamera* camera)
{
	if (activeGameCamera)
		activeGameCamera->isActiveCamera = false;

	activeGameCamera = camera;
}

ComponentCamera * ModuleScene::GetActiveGameCamera() const
{
	return activeGameCamera;
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

/* TEMPORARY CODE START */
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
void ModuleScene::TestQuadTree()
{
	std::vector<GameObject*> gos;
	{
		GameObject* go = new GameObject("Sphere -5", root);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(-5, -5, -5);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}
	GameObject* go5;
	{
		GameObject* go = new GameObject("Sphere 5", root);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(5, 5, 5);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
		go5 = go;
	}
	{
		GameObject* go = new GameObject("Sphere 1.5", root);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(1.5f, 1.5f, 1.5f);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}
	quadTree.Create(gos);
	quadTree.Remove(go5);
}
/* TEMPORARY CODE END */
