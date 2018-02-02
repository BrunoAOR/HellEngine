#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	MATERIAL,
	MESH,
	TRANSFORM
};

const char* GetEditorTitle(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
