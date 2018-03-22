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
	TRANSFORM
};

extern const ComponentType COMPONENT_TYPES[7];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
