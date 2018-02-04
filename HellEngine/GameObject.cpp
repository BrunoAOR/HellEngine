#include <assert.h>
#include "ImGui/imgui.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "ModuleScene.h"
#include "globals.h"


int GameObject::nextId = 0;

GameObject::GameObject(const char* name, GameObject* parentGameObject) : name(name)
{
	LOGGER("Constructing GameObject '%s'", name);
	id = nextId++;
	/* If the root is nullptr, then that means that we are currently creating the root and no parent should be set. */
	if (App->scene->root != nullptr)
	{
		parent = App->scene->root;
		App->scene->root->children.push_back(this);

		if (parentGameObject != nullptr && parentGameObject != App->scene->root)
			SetParent(parentGameObject);
	}
}

GameObject::~GameObject()
{
	LOGGER("Began deletion of GameObject '%s'", name.c_str());
	for (Component* component : components)
	{
		delete component;
	}
	components.clear();

	for (GameObject* child : children)
	{
		delete child;
	}
	children.clear();
	LOGGER("Deleted GameObject '%s'", name.c_str());
}

void GameObject::Update()
{
	if (!isActive)
		return;

	for (Component* component : components)
	{
		component->Update();
	}
	for (GameObject* child : children)
	{
		child->Update();
	}
}

void GameObject::OnEditor()
{
	ImGui::Checkbox("", &isActive);
	ImGui::SameLine();
	static char nameBuf[32] = "";
	memcpy_s(nameBuf, 32, name.c_str(), name.length());
	nameBuf[name.length()] = '\0';
	ImGui::InputText(" ", nameBuf, 32);
	name = nameBuf;
	if (ImGui::BeginMenu("Add new Component"))
	{
		for (ComponentType componentType : COMPONENT_TYPES)
		{
			if (ImGui::MenuItem(GetString(componentType), ""))
			{
				AddComponent(componentType);
			}
		}
		ImGui::EndMenu();
	}
	for (Component* component : components)
	{
		component->OnEditor();
	}
}

void GameObject::OnEditorHierarchy()
{
	ImGuiTreeNodeFlags selectedFlag = App->scene->editorInfo.selectedGameObject == this ? ImGuiTreeNodeFlags_Selected : 0;
	if (children.size() == 0)
	{
		ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | selectedFlag, name.c_str());
		if (ImGui::IsItemClicked())
			App->scene->editorInfo.selectedGameObject = this;
	}
	else
	{
		bool open = ImGui::TreeNodeEx(this, selectedFlag, name.c_str());
		if (ImGui::IsItemClicked())
			App->scene->editorInfo.selectedGameObject = this;
		if (open)
		{
			for (GameObject* child : children)
			{
				child->OnEditorHierarchy();
			}
			ImGui::TreePop();
		}
	}
}

GameObject* GameObject::GetParent()
{
	/* The scene root remains protected from use */
	if (parent == App->scene->root)
		return nullptr;
	else
		return parent;
}

bool GameObject::SetParent(GameObject* newParent)
{
	/* Protect against manipulation of the root */
	if (this == App->scene->root)
		return false;
	
	/* All non-root gameObjects are guaranteed to have a parent and the root should never be directly used. But always check. */
	assert(parent);

	/* Verify for nullptr. Parent to Scene's root if so */
	if (newParent == nullptr)
	{
		/* Inform the transform (if it exists) */
		std::vector<Component*> transforms = GetComponents(ComponentType::TRANSFORM);
		if (transforms.size() > 0)
		{
			ComponentTransform* transform = (ComponentTransform*)transforms[0];
			transform->SetParent(nullptr);
		}

		/* Remove from current parent. */
		parent->RemoveChild(this);

		LOGGER("Changed parent of '%s' from '%s' to the root GameObject", name.c_str(), parent->name.c_str());

		/* Assign new parent */
		parent = App->scene->root;
		App->scene->root->children.push_back(this);
		return true;
	}

	/* Ensure the newParent is NOT a child of this GameObject */
	if (HasGameObjectInChildrenHierarchy(newParent))
	{
		LOGGER("Warning: Attempted to change the parent of GameObject '%s' to '%s'. Operation was aborted because '%s' is a child of '%s'", name.c_str(), newParent->name.c_str(), newParent->name.c_str(), name.c_str());
		return false;
	}
	else
	{
		/* Inform the transform (if it exists) about the newParent's transform (if it exists, else nullptr) */
		std::vector<Component*> transforms = GetComponents(ComponentType::TRANSFORM);
		if (transforms.size() > 0)
		{
			ComponentTransform* transform = (ComponentTransform*)transforms[0];

			std::vector<Component*> parentTransforms = newParent->GetComponents(ComponentType::TRANSFORM);
			ComponentTransform* parentTransform = nullptr;
			
			if (parentTransforms.size() > 0)
				parentTransform = (ComponentTransform*)parentTransforms[0];

			transform->SetParent(parentTransform);
		}

		/* Remove from current parent. */
		parent->RemoveChild(this);

		LOGGER("Changed parent of '%s' from '%s' to '%s'", name.c_str(), parent->name.c_str(), newParent->name.c_str());

		/* Assign new parent */
		parent = newParent;
		newParent->children.push_back(this);
		return true;
	}
}

std::vector<Component*> GameObject::GetComponents(ComponentType type)
{
	std::vector<Component*> foundComponents;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			foundComponents.push_back(*it);
		}
	}

	return foundComponents;
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;

	switch (type)
	{
	case ComponentType::MATERIAL:
		component = new ComponentMaterial(this);
		break;
	case ComponentType::MESH:
		component = new ComponentMesh(this);
		break;
	case ComponentType::TRANSFORM:
		component = new ComponentTransform(this);
		break;
	}

	if (component != nullptr)
		components.push_back(component);

	return component;
}

bool GameObject::RemoveComponent(Component* component)
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (component == (*it))
		{
			delete component;
			components.erase(it);
			return true;
		}
	}

	return false;
}

bool GameObject::HasGameObjectInChildrenHierarchy(GameObject * testGameObject)
{
	for (GameObject* child : children)
	{
		if (child == testGameObject || child->HasGameObjectInChildrenHierarchy(testGameObject))
			return true;
	}
	return false;
}

bool GameObject::RemoveChild(GameObject * childToRemove)
{
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (*it == childToRemove)
		{
			children.erase(it);
			return true;
		}
	}
	return false;
}

std::vector<GameObject*> GameObject::GetChildren() 
{
    return children;
}