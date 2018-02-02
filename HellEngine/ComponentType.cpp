#include "ComponentType.h"

const char * GetEditorTitle(ComponentType componentType)
{
	switch (componentType)
	{
	case ComponentType::MATERIAL:
		return "Material";
	case ComponentType::MESH:
		return "Mesh";
	case ComponentType::TRANSFORM:
		return "Local Transformation";
	default:
		return "";
	}
}
