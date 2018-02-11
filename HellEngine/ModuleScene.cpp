#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
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
	if (fixedQuadTreeActive)
	{
		fixedQuadTree.DrawTree();
		
	}
	root->Update();
	/* TEMPORARY CODE END */

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

void ModuleScene::SetFixedQuadTreeActive(bool isActive)
{
	fixedQuadTreeActive = isActive;
}

void ModuleScene::UnloadSceneFixedQuadTree()
{
	SetFixedQuadTreeActive(false);
	fixedQuadTree.~SpaceQuadTree();
	staticGameObjects.clear();
}

void ModuleScene::GenerateSceneFixedQuadTree()
{

	AABB staticGoAABB;
	AABB resultingStaticGosABBB;
	resultingStaticGosABBB.SetNegativeInfinity();

	if (!fixedQuadTreeActive) //It's only necessary at the initialization instant
	{
		FindAllSceneStaticGameObjects(root);

		if (staticGameObjects.size() != 0)
		{

			for (GameObject *staticGO : staticGameObjects) {

				ComponentTransform* staticObjectTransform = (ComponentTransform*)staticGO->GetComponent(ComponentType::TRANSFORM);
				staticGoAABB = staticObjectTransform->GetBoundingBox();

				resultingStaticGosABBB.Enclose(staticGoAABB.minPoint, staticGoAABB.maxPoint);
			}

			fixedQuadTree.Create(resultingStaticGosABBB.minPoint, resultingStaticGosABBB.maxPoint);
			fixedQuadTree.Insert(staticGameObjects);
		}

	}

	fixedQuadTreeActive = true;

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
#include "MathGeoLib/src/Geometry/AABB.h"
#include "MathGeoLib/src/Geometry/Line.h"
#include "MathGeoLib/src/Geometry/Ray.h"
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
		transform->SetIsStatic(true);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}
	{
		GameObject* go = new GameObject("Sphere 5", root);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(5, 5, 5);
		transform->SetIsStatic(false);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}
	{
		GameObject* go = new GameObject("Sphere 1.5", root);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(1.5f, 5, 1.5f);
		transform->SetIsStatic(true);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}

	{
		GameObject* go = new GameObject("Sphere 1.5", gos.at(1));
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		transform->SetPosition(1.7f, 5, 1.5f);
		transform->SetIsStatic(true);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		gos.push_back(go);
	}
	//quadTree.Create(gos);
	
	/*Line line(float3(0, 0, 0), vec(1, 1, 1));
	std::vector<GameObject*> lineIntersections;
	quadTree.Intersects(lineIntersections, line);
	LOGGER("Line hits: %i objects", lineIntersections.size());
	AABB cube(float3(1.99f, 0, 1.99f), float3(5,5,5));
	std::vector<GameObject*> cubeIntersections;
	quadTree.Intersects(cubeIntersections, cube);
	LOGGER("Cube hits: %i objects", cubeIntersections.size());
	Ray ray(float3(0,0,0), vec(1,1,1).Normalized());
	std::vector<GameObject*> rayIntersections;
	quadTree.Intersects(rayIntersections, ray);
	LOGGER("Ray hits: %i objects", rayIntersections.size());*/
}
/* TEMPORARY CODE END */
void ModuleScene::FindAllSceneStaticGameObjects(GameObject* go)
{
	for (GameObject* children : go->GetChildren())
	{
		if (children->GetComponent(ComponentType::TRANSFORM) != nullptr)
		{
			if (((ComponentTransform*)children->GetComponent(ComponentType::TRANSFORM))->GetIsStatic())
			{
				staticGameObjects.push_back(children);
			}
		}

		if (children->GetChildren().size() != 0)
		{
			FindAllSceneStaticGameObjects(children);
		}
	}
}