#include "Component.h"
#include "GameObject.h"
#include "globals.h"

Component::Component(GameObject* owner) : gameObject(owner)
{
	LOGGER("Constructing Component for GameObject '%s'", gameObject->name.c_str());
}

Component::~Component()
{
	LOGGER("Deleted Component from GameObject '%s'", gameObject->name.c_str());
}

ComponentType Component::GetType()
{
	return type;
}