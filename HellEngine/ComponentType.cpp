#include "ComponentType.h"

const ComponentType COMPONENT_TYPES_3D[] = { ComponentType::ANIMATION, ComponentType::CAMERA, ComponentType::GRASS, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::TRANSFORM };
const ComponentType COMPONENT_TYPES_2D[] = { ComponentType::TRANSFORM_2D, ComponentType::UI_ELEMENT, ComponentType::UI_IMAGE };

const char* GetString(ComponentType componentType)
{
	switch (componentType)
	{
		/* 3D components*/
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
		/* 2D components*/
	case ComponentType::TRANSFORM_2D:
		return "Transform 2D";
	case ComponentType::UI_ELEMENT:
		return "UI Element";
	case ComponentType::UI_IMAGE:
		return "UI Image";
	default:
		return "";
	}
}
