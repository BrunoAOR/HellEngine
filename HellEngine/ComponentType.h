#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	MATERIAL,
	MESH,
	TRANSFORM,
	CAMERA,
	ANIMATION,
	UIELEMENT
};

extern const ComponentType COMPONENT_TYPES[6];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
