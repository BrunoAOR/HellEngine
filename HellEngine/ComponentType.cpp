#include "ComponentType.h"

const ComponentType COMPONENT_TYPES[] = { ComponentType::ANIMATION, ComponentType::AUDIOSOURCE, ComponentType::AUDIOLISTENER, ComponentType::CAMERA, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::TRANSFORM };

const char* GetString(ComponentType componentType)
{
	switch (componentType)
	{
	case ComponentType::ANIMATION:
		return "Animation";
	case ComponentType::AUDIOSOURCE:
		return "AudioSource";
	case ComponentType::AUDIOLISTENER:
		return "AudioListener";
	case ComponentType::CAMERA:
		return "Camera";
	case ComponentType::MATERIAL:
		return "Material";
	case ComponentType::MESH:
		return "Mesh";
	case ComponentType::TRANSFORM:
		return "Transform";
	default:
		return "";
	}
}
