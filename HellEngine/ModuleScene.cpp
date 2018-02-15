#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "GameObject.h"
/* For TestCollisionChecks */
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

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
	quadTree.CleanUp();

	delete root;
	root = nullptr;

	return true;
}

#include "Application.h"
#include "ModuleInput.h"
#include "globals.h"
UpdateStatus ModuleScene::Update()
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID)
	{
		//quadTree.DrawTree(); activate with debug checkbox
	}
	BROFILER_CATEGORY("ModuleScene::Update", Profiler::Color::PapayaWhip);
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

void ModuleScene::UnloadSceneFixedQuadTree()
{
	quadTree.CleanUp();
}

void ModuleScene::GenerateSceneFixedQuadTree(float* minPoint, float* maxPoint)
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::FIXED)
	{
		vec vecMinPoint(minPoint[0], minPoint[1], minPoint[2]);
		vec vecMaxPoint(maxPoint[0], maxPoint[1], maxPoint[2]);
		quadTree.Create(vecMinPoint, vecMaxPoint);
		std::vector<GameObject*> staticGOs;
		FindAllSceneStaticGameObjects(staticGOs, root);
		quadTree.Insert(staticGOs);
	}
	return;
}

void ModuleScene::GenerateSceneAdaptiveQuadTree()
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::ADAPTIVE)
	{
		std::vector<GameObject*> staticGOs;
		FindAllSceneStaticGameObjects(staticGOs, root);
		quadTree.Create(staticGOs);
	}
}

void ModuleScene::ChangeStaticStatus(ComponentTransform* transform, bool isStatic)
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID)
	{
		if (isStatic)
			quadTree.Insert(transform->gameObject);
		else
			quadTree.Remove(transform->gameObject);
	}
}

void ModuleScene::TestCollisionChecks(float3 aabbMinPoint, float3 aabbMaxPoint, float3 spawnMinPoint, float3 spawnMaxPoint, int spawnedObjectsCount)
{
	static GameObject* spawnedParent = nullptr;
	static bool wasAdaptive = false;

	wasAdaptive = quadTree.GetType() == SpaceQuadTree::QuadTreeType::ADAPTIVE;
	if (wasAdaptive)
	{
		quadTree.CleanUp();
	}

	if (spawnedParent != nullptr)
	{
		Destroy(spawnedParent);
		spawnedParent = nullptr;
	}
	spawnedParent = CreateGameObject();

	LOGGER("");
	LOGGER("Testing count of intersections checks:");
	AABB testCube(aabbMinPoint, aabbMaxPoint);

	static std::string sphereName = "";

	std::vector<GameObject*> allNewStaticGameObjects;
	for (int i = 0; i < spawnedObjectsCount; ++i)
	{
		sphereName = "Sphere " + std::to_string(i + 1);
		GameObject* go = new GameObject(sphereName.c_str(), spawnedParent);
		ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);
		float x = spawnMinPoint.x + rand() % (int)(spawnMaxPoint.x - spawnMinPoint.x);
		float y = spawnMinPoint.y + rand() % (int)(spawnMaxPoint.y - spawnMinPoint.y);
		float z = spawnMinPoint.z + rand() % (int)(spawnMaxPoint.z - spawnMinPoint.z);
		transform->SetPosition(x, y, z);
		transform->SetIsStatic(true);
		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetActiveVao(1);
		ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
		mat->SetDefaultMaterialConfiguration();
		mat->Apply();
		allNewStaticGameObjects.push_back(go);
	}

	if (wasAdaptive)
		quadTree.Create(allNewStaticGameObjects);

	LOGGER("Spheres tested: %i", spawnedObjectsCount);
	std::vector<GameObject*> intersected;
	Intersects(intersected, testCube);
	LOGGER("Brute force found %i intersection", intersected.size());
	intersected.clear();
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID)
	{
		BROFILER_CATEGORY("Quad Tree check start", Profiler::Color::Yellow);
		quadTree.Intersects(intersected, testCube);
		LOGGER("QuadTree checks: %i", quadTree.lastChecksPerformed);
		LOGGER("QuadTree found %i intersection", intersected.size());
	}

}

void ModuleScene::QuadTreeFrustumCulling(std::vector<GameObject*>& insideFrustum, Frustum frustum)
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID)
	{
		//LOGGER("");
		quadTree.Intersects(insideFrustum, frustum);
		//LOGGER("QuadTree found %i intersection", insideFrustum.size());
	}
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

bool ModuleScene::UsingQuadTree()
{
	return quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID;
}

void ModuleScene::FindAllSceneStaticGameObjects(std::vector<GameObject*>& staticGameObjects, GameObject* go)
{
	if (go == nullptr)
		go = root;

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
			FindAllSceneStaticGameObjects(staticGameObjects, children);
		}
	}
}
