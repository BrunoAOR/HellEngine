#include <assert.h>
#include <string>
#include "ComponentType.h"

const ComponentType COMPONENT_TYPES[] = { ComponentType::ANIMATION, ComponentType::CAMERA, ComponentType::GRASS, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::TRANSFORM };

const char* GetString(ComponentType componentType)
{
	switch (componentType)
	{
	case ComponentType::ANIMATION:
		return "Animation";
	case ComponentType::CAMERA:
		return "Camera";
	case ComponentType::GRASS:
		return "Grass";
	case ComponentType::MATERIAL:
		return "Material";
	case ComponentType::MESH:
		return "Mesh";
	case ComponentType::TRANSFORM:
		return "Transform";
	default:
		return "";
	}
}

ComponentType GetComponentType(const char* componentTypeString)
{
	if (strcmp(componentTypeString, "Animation") == 0)
		return ComponentType::ANIMATION;
	else if (strcmp(componentTypeString, "Camera") == 0)
		return ComponentType::CAMERA;
	else if (strcmp(componentTypeString, "Grass") == 0)
		return ComponentType::GRASS;
	else if (strcmp(componentTypeString, "Material") == 0)
		return ComponentType::MATERIAL;
	else if (strcmp(componentTypeString, "Mesh") == 0)
		return ComponentType::MESH;
	else if (strcmp(componentTypeString, "Transform") == 0)
		return ComponentType::TRANSFORM;

	assert(false);
	return ComponentType::ANIMATION;
}
