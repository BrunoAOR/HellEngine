#ifndef __H_MODULE_SCENE__
#define __H_MODULE_SCENE__

#include <vector>
#include "Module.h"
class GameObject;

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

	GameObject* CreateGameObject();
	void Destroy(GameObject* gameObject);
	std::vector<GameObject*> FindByName(const std::string& name, GameObject* gameObject = nullptr);

public:

	GameObject* root;

private:

	void CreateTestGameObjects();

private:

	GameObject* go1;
	GameObject* go2;
	GameObject* go3;
	GameObject* go4;

	struct
	{
		GameObject* selectedGameObject = nullptr;
	} editorInfo;

};

#endif // !__H_MODULE_SCENE__
