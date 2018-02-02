#include "Component.h"


Component::Component(GameObject* owner) : gameObject(owner)
{
}

Component::~Component()
{
}

ComponentType Component::GetType()
{
	return type;
}