#include <assert.h>
#include <deque>
#include <stack>
#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleAnimation.h"
#include "ModuleEditorCamera.h"
#include "ModuleTrueFont.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "Serializer.h"
#include "SerializableArray.h"
#include "SerializableObject.h"
/* For TestCollisionChecks */
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
/* For TestLineSegmentChecks */
#include "MathGeoLib/src/Geometry/LineSegment.h"
#include "openGL.h"
#include "physicsFunctions.h"

uint ModuleScene::gameObjectsCount = 0;

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
	sceneLoader.LoadPlaneMesh();
	sceneLoader.LoadCubeMesh();
	sceneLoader.LoadSphereMesh();
	return true;
}

bool ModuleScene::CleanUp()
{
	editorInfo.selectedGameObject = nullptr;

	quadTree.CleanUp();

	delete root;
	root = nullptr;

	modelPaths.clear();

	for (MeshInfo* meshInfo : meshes)
	{
		/* Delete VRAM buffers */
		glDeleteVertexArrays(1, &meshInfo->vao);
		glDeleteBuffers(1, &meshInfo->vbo);
		glDeleteBuffers(1, &meshInfo->ebo);

		/* Delete Bones */
		for (Bone* bone : meshInfo->bones)
		{
			/* DO NOT delete it->first because it points to the same memLocation as bone->name which will be deleted next */
			delete[] bone->name;
			delete[] bone->weights;
			delete bone;
		}

		delete meshInfo;
	}
	meshes.clear();

	return true;
}

UpdateStatus ModuleScene::Update()
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID && DEBUG_MODE)
	{
		quadTree.DrawTree(); 
	}
	BROFILER_CATEGORY("ModuleScene::DrawHierarchy", Profiler::Color::PapayaWhip);
	DrawHierarchy();
	BROFILER_CATEGORY("ModuleScene::Update", Profiler::Color::PapayaWhip);
	root->Update();
	BROFILER_CATEGORY("ModuleScene::UpdateEnd", Profiler::Color::PapayaWhip);

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleScene::OnEditorHierarchy(float mainMenuBarHeight, bool* pOpen)
{
	float windowWidth = 400;
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, App->window->GetHeight() - mainMenuBarHeight));
	ImGui::Begin("Hierarchy", pOpen, ImGuiWindowFlags_NoCollapse);

	root->OnEditorRootHierarchy();

	ImGui::End();
}

void ModuleScene::OnEditorInspector(float mainMenuBarHeight, bool * pOpen)
{
	float windowWidth = 400;
	ImGui::SetNextWindowPos(ImVec2(App->window->GetWidth() - windowWidth, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, App->window->GetHeight() - mainMenuBarHeight));
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

void ModuleScene::OnEditorPlayButtonWindow(float mainMenuBarHeight)
{
	static char* sceneTempFile = "assets/tempPlayModeScene.json";
	static const float width = 250;
	static const float height = 70;
	static bool isPlaying = false;
	static bool isPaused = false;
	static bool playing1Frame = false;
	static float originalTimeScale = 1.0f;
	static float currentTimeScale = 1.0f;
	static ImVec4 gray = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
	static ImVec4 red = ImVec4(0.75f, 0.0f, 0.0f, 1.0f);

	float windowWidth = (float)App->window->GetWidth();
	ImGui::SetNextWindowPos(ImVec2((windowWidth - width) / 2, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGui::Begin("Play Window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (playing1Frame)
	{
		App->time->SetTimeScale(0);
		playing1Frame = false;
	}

	if (!isPlaying)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, gray);
		if (ImGui::Button("Play"))
		{
			isPlaying = true;
			/* Should Save Scene */
			Save(sceneTempFile);
			originalTimeScale = App->time->GetTimeScale();
			App->time->ResetReferenceTime();
		}
		ImGui::PopStyleColor();

	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, isPaused ? red : gray);
		if (ImGui::Button("Pause"))
		{
			isPaused = !isPaused;
			App->time->SetTimeScale(isPaused ? 0 : currentTimeScale);
		}
		ImGui::PopStyleColor();


		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, red);
		if (ImGui::Button("Stop"))
		{
			isPlaying = false;
			isPaused = false;
			/* Should Load Scene */
			App->time->SetTimeScale(originalTimeScale);
			currentTimeScale = originalTimeScale;
			Load(sceneTempFile);
			DeleteFileByPath(sceneTempFile);
		}
		ImGui::PopStyleColor();

		if (isPaused)
		{
			ImGui::SameLine();
			if (ImGui::Button("1 Frame"))
			{
				playing1Frame = true;
				App->time->SetTimeScale(currentTimeScale);
			}
		}
	}


	if (ImGui::SliderFloat("Time scale", &currentTimeScale, 0.0f, 3.0f, "%.1f"))
	{
		if (!isPaused)
			App->time->SetTimeScale(currentTimeScale);
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
		FindAllStaticGameObjects(staticGOs);
		quadTree.Insert(staticGOs);
	}
	return;
}

void ModuleScene::GenerateSceneAdaptiveQuadTree()
{
	if (quadTree.GetType() != SpaceQuadTree::QuadTreeType::ADAPTIVE)
	{
		std::vector<GameObject*> staticGOs;
		FindAllStaticGameObjects(staticGOs);
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
		mesh->SetActiveModelInfo(1);
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
		BROFILER_CATEGORY("QuadTreeCulling", Profiler::Color::RosyBrown);
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

void ModuleScene::SetActiveGameLight(ComponentLight* light)
{
	if (activeGameLight)
		activeGameLight->isActiveLight = false;

	activeGameLight = light;
}

ComponentLight * ModuleScene::GetActiveGameLight() const
{
	return activeGameLight;
}

void ModuleScene::SetSelectedGameObject(GameObject * go)
{
	editorInfo.selectedGameObject = go;
}

bool ModuleScene::LoadModel(const char* modelPath, GameObject* parent, bool meshesOnly)
{
	bool success;
	success = sceneLoader.Load(modelPath, parent, meshesOnly);

	if (success)
		modelPaths.push_back(modelPath);

	return success;
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

void ModuleScene::Save(const char* filePath)
{
	Serializer serializer;
	SerializableObject sObject = serializer.GetEmptySerializableObject();

	/* Save animations info */
	App->animation->Save(sObject);
	/* Save fonts info */
	App->fonts->Save(sObject);

	sObject.AddVectorString("ModelPaths", modelPaths);
	SerializableArray gameObjectsArray = sObject.BuildSerializableArray("GameObjects");

	std::deque<GameObject*> goDeque;	

	GameObject* go = root;
	goDeque.push_back(go);

	/* The root otself is NOT serialized */
	while (!goDeque.empty())
	{
		go = goDeque.front();
		goDeque.pop_front();

		SerializableObject goSObject = gameObjectsArray.BuildSerializableObject();

		go->Save(goSObject);

		for (GameObject* child : go->GetChildren())		
			goDeque.push_back(child);

	}

	serializer.Save(filePath);
}

void ModuleScene::Load(const char* jsonPath)
{
	/* First we clean up the scene */
	
	CleanUp();

	Serializer serializer;
	SerializableObject sObject = serializer.Load(jsonPath);

	/* Load animations info */
	App->animation->Load(sObject);
	/* Load fonts info */
	App->fonts->Load(sObject);

	/* Load Meshes */
	sceneLoader.LoadPlaneMesh();
	sceneLoader.LoadCubeMesh();
	sceneLoader.LoadSphereMesh();
	std::vector<std::string> allPaths = sObject.GetVectorString("ModelPaths");
	/* allPaths is not copied into modelPaths because that happens inside the LoadModel method */
	for (std::string path : allPaths)
	{
		LoadModel(path.c_str(), nullptr, true);
	}

	/* Now we load GameObjects */
	std::map<u32, GameObject*> gameObjectsByUuid;
	std::vector<Component*> meshesCreated;
	meshesCreated.reserve(64);

	SerializableArray gameObjectsArray = sObject.GetSerializableArray("GameObjects");
	uint gameObjectsCount = gameObjectsArray.Size();

	std::vector<GameObject*> gameObjectsCreated;
	gameObjectsCreated.reserve(gameObjectsCount);
	std::map<u32, Component*> componentsByUuid;
	
	/* Create GameObjects */
	for (uint i = 0; i < gameObjectsCount; ++i)
	{
		SerializableObject goData = gameObjectsArray.GetSerializableObject(i);
		GameObject* go = CreateGameObject();
		gameObjectsCreated.push_back(go);

		if (i == 0)
			root = go;

		go->Load(goData, componentsByUuid);

		std::vector<Component*> goMeshes = go->GetComponents(ComponentType::MESH);
		meshesCreated.insert(meshesCreated.end(), goMeshes.begin(), goMeshes.end());

		gameObjectsByUuid[go->GetUUID()] = go;
		u32 parentUuid = go->GetParentUUID();
		/* Check done to leave root out */
		if (parentUuid != 0)
		{
			/*
			Since GameObjects are saved by layers within the root
			(meaning that all GameObjects at a depth in the Hierarchy are saved before going to a child depth),
			it is safe to assume that any parent will have already been loaded
			*/
			assert(gameObjectsByUuid.count(parentUuid) > 0);
			go->SetParent(gameObjectsByUuid[parentUuid]);
		}
	}

	/* Link components */
	for (uint i = 0; i < gameObjectsCount; ++i)
	{
		SerializableObject goData = gameObjectsArray.GetSerializableObject(i);
		gameObjectsCreated[i]->LinkComponents(goData, componentsByUuid);
	}

	/* Optimize Meshes */
	for (Component* component : meshesCreated)
	{
		((ComponentMesh*)component)->StoreBoneToTransformLinks();
	}

	/* Re-initialize ModuleEditorCamera */
	App->editorCamera->Init();
}

void ModuleScene::DrawHierarchy()
{
	std::stack<GameObject*> goStack;
	GameObject* go = root;
	goStack.push(go);

	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	float blue[4]{ 0, 0, 1, 1 };
	glColor4fv(blue);

	while (!goStack.empty())
	{
		go = goStack.top();
		goStack.pop();

		ComponentTransform* parentTransform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		float3 parentPosition;
		if (parentTransform)
		{
			DecomposeMatrixPosition(parentTransform->GetModelMatrix4x4(), parentPosition);
		}

		for (GameObject* child : go->GetChildren())
		{
			goStack.push(child);

			ComponentTransform* childTransform = (ComponentTransform*)child->GetComponent(ComponentType::TRANSFORM);
			if (parentTransform && childTransform)
			{
				float3 childPosition;
				DecomposeMatrixPosition(childTransform->GetModelMatrix4x4(), childPosition);

				glBegin(GL_LINES);
				glVertex3f(parentPosition.x, parentPosition.y, parentPosition.z);
				glVertex3f(childPosition.x, childPosition.y, childPosition.z);
				glEnd();
			}

		}
	}
	glColor4fv(currentColor);
}

bool ModuleScene::UsingQuadTree()
{
	return quadTree.GetType() != SpaceQuadTree::QuadTreeType::INVALID;
}

const SpaceQuadTree& ModuleScene::GetQuadTree()
{
	return quadTree;
}

void ModuleScene::FindAllStaticGameObjects(std::vector<GameObject*>& staticGameObjects)
{
	std::stack<GameObject*> stack;

	GameObject* go = root;

	stack.push(go);

	while (!stack.empty()) {
		go = stack.top();
		stack.pop();

		for (GameObject* child : go->GetChildren())
		{
			ComponentTransform* transform = (ComponentTransform*)child->GetComponent(ComponentType::TRANSFORM);
			if (transform && transform->GetIsStatic())
				staticGameObjects.push_back(child);

			if (child->GetChildren().size() != 0)
			{
				stack.push(child);
			}
		}
	}	
}

void ModuleScene::FindAllDynamicGameObjects(std::vector<GameObject*>& dynamicGameObjects)
{
	std::stack<GameObject*> stack;

	GameObject* go = root;

	stack.push(go);

	while (!stack.empty()) {
		go = stack.top();
		stack.pop();

		for (GameObject* child : go->GetChildren())
		{
			ComponentTransform* transform = (ComponentTransform*)child->GetComponent(ComponentType::TRANSFORM);
			if (transform && !transform->GetIsStatic())
				dynamicGameObjects.push_back(child);

			if (child->GetChildren().size() != 0)
			{
				stack.push(child);
			}
		}
	}
}

void ModuleScene::TestLineSegmentChecks(float3 lineStartPoint, float3 lineEndPoint)
{
	assert(lineStartPoint.x != lineEndPoint.x || lineStartPoint.y != lineEndPoint.y || lineStartPoint.z != lineEndPoint.z);

	LineSegment testSegment(lineStartPoint, lineEndPoint);
	GameObject* collidedGO = CalculateRaycast(testSegment);
	LOGGER("The line collided with GameObject: %s", collidedGO ? collidedGO->name.c_str() : "none");
}
