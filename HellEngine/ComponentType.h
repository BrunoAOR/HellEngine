#ifndef __H_COMPONENT_TYPE__
#define __H_COMPONENT_TYPE__

enum class ComponentType
{
	MATERIAL,
	AUDIOSOURCE,
	AUDIOLISTENER,
	MESH,
	TRANSFORM,
	CAMERA,
	ANIMATION
};

extern const ComponentType COMPONENT_TYPES[7];
const char* GetString(ComponentType componentType);

#endif // !__H_COMPONENT_TYPE__
