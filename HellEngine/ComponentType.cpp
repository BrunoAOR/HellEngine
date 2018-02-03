#include "ComponentType.h"

const ComponentType COMPONENT_TYPES[] = { ComponentType::MATERIAL, ComponentType::MESH, ComponentType::TRANSFORM };

const char* GetString(ComponentType componentType)
{
	switch (componentType)
	{
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
