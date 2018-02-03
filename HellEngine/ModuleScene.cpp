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
	root->Update();
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

#include "ComponentMaterial.h"
#include "ComponentTransform.h"
void ModuleScene::CreateTestGameObjects()
{
	GameObject* go1 = new GameObject("Child 1", root);
	GameObject* go2 = new GameObject("Child 2", root);
	GameObject* go2_2 = new GameObject("Child 2.1", go2);
	go2->SetParent(go2_2);
	GameObject* go3 = new GameObject("Child 3", root);
	
	ComponentTransform* transform = (ComponentTransform*)go3->AddComponent(ComponentType::TRANSFORM);
	transform->SetPosition(0, 4, 0);
	transform->SetRotationDeg(0, 15, 0);
	transform->SetScale(0.5f, 2, 0.5f);

	go3->AddComponent(ComponentType::MESH);

	ComponentMaterial* mat = (ComponentMaterial*)go3->AddComponent(ComponentType::MATERIAL);
	mat->SetVertexShaderPath("assets/shaders/defaultShader.vert");
	mat->SetFragmentShaderPath("assets/shaders/tintingShader.frag");
	mat->SetTexturePath("assets/images/lenna.png");
	mat->SetShaderDataPath("assets/shaders/tintingShaderData.shaderdata");
	mat->Apply();

	go3->SetParent(go1);
	
	GameObject *go4 = new GameObject("Child 4", root);
	go4->AddComponent(ComponentType::MESH);
	Component* toDelete = go4->AddComponent(ComponentType::MATERIAL);
	go4->RemoveComponent(toDelete);
	new GameObject("Child 4.1", go4);
	Destroy(go4);
}
