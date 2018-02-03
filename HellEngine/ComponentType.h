#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	MATERIAL,
	MESH,
	TRANSFORM
};

extern const ComponentType COMPONENT_TYPES[3];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
