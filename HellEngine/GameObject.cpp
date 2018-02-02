#include <assert.h>
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "ModuleScene.h"
#include "globals.h"


GameObject::GameObject(const char* name, GameObject* parentGameObject) : name(name)
{
	LOGGER("Constructing GameObject '%s'", name);
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
	std::vector<Component*> components;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			components.push_back(*it);
		}
	}

	return components;
}

Component* GameObject::AddComponent(ComponentType type)
{
	return nullptr;
}

bool GameObject::RemoveComponent(Component* component)
{
	bool success = false;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (component == (*it))
		{
			components.erase(it);
			success = true;
		}
	}

	return success;
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