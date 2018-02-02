#include "GameObject.h"
#include "Component.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
}

GameObject::~GameObject()
{
}

GameObject* GameObject::GetParent()
{
	return parent;
}

bool GameObject::SetParent(GameObject* newParent)
{
	return false;
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
	return false;
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