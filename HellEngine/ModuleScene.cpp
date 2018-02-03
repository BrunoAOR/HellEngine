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
	ComponentTransform* go1transform = (ComponentTransform*)go1->AddComponent(ComponentType::TRANSFORM);
	go1transform->SetPosition(0, 0, -2);
	go1transform->SetRotationDeg(0, 180, 0);
	go1transform->SetScale(1, 2, 1);

	GameObject* go2 = new GameObject("Child 2", root);
	GameObject* go2_2 = new GameObject("Child 2.1", go2);
	go2->SetParent(go2_2);

	/* go3 setup */
	GameObject* go3 = new GameObject("Child 3", root);
	
	ComponentTransform* go3transform = (ComponentTransform*)go3->AddComponent(ComponentType::TRANSFORM);
	go3transform->SetPosition(0, 2, -2);
	go3transform->SetRotationDeg(0, 45, 0);
	go3transform->SetScale(1, 1, 1);

	go3->AddComponent(ComponentType::MESH);

	ComponentMaterial* mat = (ComponentMaterial*)go3->AddComponent(ComponentType::MATERIAL);
	mat->SetVertexShaderPath("assets/shaders/defaultShader.vert");
	mat->SetFragmentShaderPath("assets/shaders/tintingShader.frag");
	mat->SetTexturePath("assets/images/lenna.png");
	mat->SetShaderDataPath("assets/shaders/tintingShaderData.shaderdata");
	mat->Apply();

	go3->SetParent(go1);
	/* End of g03 setup */

	GameObject *go4 = new GameObject("Child 4", root);
	go4->AddComponent(ComponentType::MESH);
	Component* toDelete = go4->AddComponent(ComponentType::MATERIAL);
	go4->RemoveComponent(toDelete);
	new GameObject("Child 4.1", go4);
	Destroy(go4);
}
