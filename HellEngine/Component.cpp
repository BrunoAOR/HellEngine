#include "Component.h"
#include "GameObject.h"
#include "globals.h"

int Component::nextId = 0;

Component::Component(GameObject* owner) : gameObject(owner)
{
	editorInfo.id = nextId++;
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

void Component::Update()
{}
