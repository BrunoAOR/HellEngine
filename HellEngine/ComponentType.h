#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

#include "dllControl.h"

enum class ENGINE_API ComponentType
{
	ANIMATION,
	CAMERA,
	GRASS,
	MATERIAL,
	MESH,
	PARTICLE_SYSTEM,
	SCRIPT,
	TRANSFORM,
	TRANSFORM_2D,
	UI_IMAGE,
	UI_INPUT_TEXT,
	UI_LABEL,
	UI_BUTTON
};

extern const ComponentType COMPONENT_TYPES_3D[8];
extern const ComponentType COMPONENT_TYPES_2D[5];

const char* GetString(ComponentType componentType);
ComponentType GetComponentType(const char* componentTypeString);

#endif // !__H_COMPONENT_TYPE__
