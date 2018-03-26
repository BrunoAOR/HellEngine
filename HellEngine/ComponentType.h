#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	ANIMATION,
	CAMERA,
	GRASS,
	MATERIAL,
	MESH,
	TRANSFORM,
	UI_ELEMENT,
	UI_IMAGE
};

extern const ComponentType COMPONENT_TYPES[8];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
