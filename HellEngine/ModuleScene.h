#ifndef __H_MODULE_SCENE__
#define __H_MODULE_SCENE__

#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "Module.h"
#include "SpaceQuadTree.h"
class ComponentCamera;
class GameObject;

#include "SceneLoader.h"
class ModuleScene :
	public Module
{
public:

	ModuleScene();
	~ModuleScene();

	bool Init();
	bool CleanUp();
	UpdateStatus Update();

	void OnEditorHierarchy(float mainMenuBarHeight, bool* pOpen);
	void OnEditorInspector(float mainMenuBarHeight, bool* pOpen);

	void UnloadSceneFixedQuadTree();
	void GenerateSceneFixedQuadTree(float* minPoint, float* maxPoint);
	void GenerateSceneAdaptiveQuadTree();
	void ChangeStaticStatus(ComponentTransform* transform, bool isStatic);
	void TestCollisionChecks(float3 aabbMinPoint, float3 aabbMaxPoint, float3 spawnMinPoint, float3 spawnMaxPoint, int spawnedObjectsCount);
	void QuadTreeFrustumCulling(std::vector<GameObject*> &insideFrustum, Frustum frustum);
	bool UsingQuadTree();
	const SpaceQuadTree& GetQuadTree();
	void FindAllStaticGameObjects(std::vector<GameObject*>& staticGameObjects, GameObject* go = nullptr);
	void FindAllDynamicGameObjects(std::vector<GameObject*>& dynamicGameObjects, GameObject* go = nullptr);

	void TestLineSegmentChecks(float3 lineStartPoint, float3 lineEndPoint);

	void SetActiveGameCamera(ComponentCamera* camera);
	ComponentCamera* GetActiveGameCamera() const;
	void SetSelectedGameObject(GameObject* go);

	bool LoadModel(const char* modelPath, GameObject* parent);
	GameObject* CreateGameObject();
	void Destroy(GameObject* gameObject);
	std::vector<GameObject*> FindByName(const std::string& name, GameObject* gameObject = nullptr);

public:

	GameObject* root;
	std::vector<VaoInfo*> meshes;

	struct
	{
		GameObject* selectedGameObject = nullptr;
	} editorInfo;

	static uint gameObjectsCount;

private:

	template<typename T>
	void Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);

private:

	ComponentCamera* activeGameCamera = nullptr;
	SpaceQuadTree quadTree;
	SceneLoader sceneLoader;

};

template<typename T>
inline void ModuleScene::Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	std::vector<GameObject*> staticGameObjects;
	FindAllStaticGameObjects(staticGameObjects);
	std::vector<ComponentTransform*> staticTransforms;
	for (GameObject* go : staticGameObjects)
	{
		staticTransforms.push_back((ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM));
	}

	BROFILER_CATEGORY("Brute Force check start", Profiler::Color::Brown);
	int checksPerformed = 0;
	for (std::vector<ComponentTransform*>::iterator it = staticTransforms.begin(); it != staticTransforms.end(); ++it)
	{
		++checksPerformed;
		if (primitive.Intersects((*it)->GetBoundingBox()))
			intersectedGameObjects.push_back((*it)->gameObject);
	}
	BROFILER_CATEGORY("Brute Force check end", Profiler::Color::White);
	LOGGER("Brute force checks: %i", checksPerformed);
}

#endif // !__H_MODULE_SCENE__
