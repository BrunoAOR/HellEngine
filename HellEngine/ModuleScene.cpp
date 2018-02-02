#include <assert.h>
#include "ComponentType.h"
#include "ModuleScene.h"
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

UpdateStatus ModuleScene::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
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

void ModuleScene::CreateTestGameObjects()
{
	new GameObject("Child 1", root);
	GameObject* testGameObject = new GameObject("Child 2", root);
	new GameObject("Child 2.1", testGameObject);
	new GameObject("Child 3", root);
	GameObject *test2 = new GameObject("Child 4", root);
	new GameObject("Child 4.1", test2);
	Destroy(test2);
}
