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
	TRANSFORM_2D,
	UI_IMAGE
};

extern const ComponentType COMPONENT_TYPES_3D[6];
extern const ComponentType COMPONENT_TYPES_2D[2];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
