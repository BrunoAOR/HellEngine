#ifndef __H_MODULE_SCENE__
#define __H_MODULE_SCENE__

#include <vector>
#include "Module.h"
class ComponentCamera;
class GameObject;
/* TEMPORARY CODE START */
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "SpaceQuadTree.h"
/* TEMPORARY CODE END */

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
	void TestCollisionChecks(float* minPoint, float* maxPoint);

	void SetActiveGameCamera(ComponentCamera* camera);
	ComponentCamera* GetActiveGameCamera() const;

	GameObject* CreateGameObject();
	void Destroy(GameObject* gameObject);
	std::vector<GameObject*> FindByName(const std::string& name, GameObject* gameObject = nullptr);

public:

	GameObject* root;
	struct
	{
		GameObject* selectedGameObject = nullptr;
	} editorInfo;

private:

	void FindAllSceneStaticGameObjects(std::vector<GameObject*>& staticGameObjects, GameObject* go = nullptr);
	template<typename T>
	void Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);

private:

	ComponentCamera* activeGameCamera;

/* TEMPORARY CODE START */
private:
	void TestQuadTree();
	SpaceQuadTree  quadTree;;
/* TEMPORARY CODE END */
};

template<typename T>
inline void ModuleScene::Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	std::vector<GameObject*> staticGameObjects;
	FindAllSceneStaticGameObjects(staticGameObjects);
	std::vector<ComponentTransform*> staticTransforms;
	for (GameObject* go : staticGameObjects)
	{
		staticTransforms.push_back((ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM));
	}

	int checksPerformed = 0;
	for (std::vector<ComponentTransform*>::iterator it = staticTransforms.begin(); it != staticTransforms.end(); ++it)
	{
		++checksPerformed;
		if (primitive.Intersects((*it)->GetBoundingBox()))
			intersectedGameObjects.push_back((*it)->gameObject);
	}
	LOGGER("Brute force checks: %i", checksPerformed);
}

#endif // !__H_MODULE_SCENE__
