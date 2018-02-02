#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	MATERIAL,
	MESH,
	TRANSFORM
};

const char* GetEditorTitle(ComponentType componentType)
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

#endif // !__H_COMPONENT_TYPE__
