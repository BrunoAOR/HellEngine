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
