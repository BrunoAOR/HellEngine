#ifndef __H_MODULE_SCENE__
#define __H_MODULE_SCENE__

#include <vector>
#include "Module.h"
class ComponentCamera;
class GameObject;
/* TEMPORARY CODE START */
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

	ComponentCamera* activeGameCamera;
	std::vector<GameObject*> gameObjects;

/* TEMPORARY CODE START */
private:
	void TestQuadTree();
	SpaceQuadTree quadTree;
/* TEMPORARY CODE END */
	void FindAllSceneGameObjects(GameObject* go);
};

#endif // !__H_MODULE_SCENE__
