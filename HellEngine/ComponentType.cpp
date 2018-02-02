#include "ComponentType.h"

const char * GetEditorTitle(ComponentType componentType)
{
	static const char* material = "Material";
	static const char* mesh = "Mesh";
	static const char* transform = "Local Transformation";
	switch (componentType)
	{
	case ComponentType::MATERIAL:
		return material;
	case ComponentType::MESH:
		return mesh;
	case ComponentType::TRANSFORM:
		return transform;
	default:
		return "";
	}
}
