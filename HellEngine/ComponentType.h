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
	UI_IMAGE,
	UI_LABEL,
	UI_BUTTON
};

extern const ComponentType COMPONENT_TYPES_3D[6];
extern const ComponentType COMPONENT_TYPES_2D[4];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
