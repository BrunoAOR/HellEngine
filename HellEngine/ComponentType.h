#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	ANIMATION,
	CAMERA,
	GRASS,
	MATERIAL,
	MESH,
	PARTICLE_SYSTEM,
	TRANSFORM,
	TRANSFORM_2D,
	UI_IMAGE,
	UI_INPUT_TEXT,
	UI_LABEL,
	UI_BUTTON
};

extern const ComponentType COMPONENT_TYPES_3D[7];
extern const ComponentType COMPONENT_TYPES_2D[5];

const char* GetString(ComponentType componentType);
ComponentType GetComponentType(const char* componentTypeString);

#endif // !__H_COMPONENT_TYPE__
