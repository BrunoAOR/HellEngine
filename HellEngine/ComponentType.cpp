#include "ComponentType.h"

const ComponentType COMPONENT_TYPES[] = { ComponentType::ANIMATION, ComponentType::CAMERA, ComponentType::GRASS, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::TRANSFORM, ComponentType::UI_ELEMENT, ComponentType::UI_IMAGE };

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
	case ComponentType::UI_ELEMENT:
		return "UI Element";
	case ComponentType::UI_IMAGE:
		return "UI Image";
	default:
		return "";
	}
}
