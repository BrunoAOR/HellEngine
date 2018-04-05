#include <assert.h>
#include <string>
#include "ComponentType.h"

const ComponentType COMPONENT_TYPES_3D[] = { ComponentType::ANIMATION, ComponentType::CAMERA, ComponentType::GRASS, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::PARTICLE_SYSTEM, ComponentType::TRANSFORM };
const ComponentType COMPONENT_TYPES_2D[] = { ComponentType::TRANSFORM_2D, ComponentType::UI_BUTTON, ComponentType::UI_IMAGE, ComponentType::UI_INPUT_TEXT, ComponentType::UI_LABEL };
//const ComponentType COMPONENT_TYPES[] = { ComponentType::ANIMATION, ComponentType::CAMERA, ComponentType::GRASS, ComponentType::MATERIAL, ComponentType::MESH, ComponentType::PARTICLE_SYSTEM, ComponentType::TRANSFORM };

const char* GetString(ComponentType componentType)
{
	switch (componentType)
	{
		/* 3D components*/
	case ComponentType::ANIMATION:
		return "Animation";
	case ComponentType::CAMERA:
		return "Camera";
	case ComponentType::GRASS:
		return "Grass";
	case ComponentType::MATERIAL:
		return "Material";
	case ComponentType::MESH:
		return "Mesh";
	case ComponentType::PARTICLE_SYSTEM:
		return "Particle System";
	case ComponentType::TRANSFORM:
		return "Transform";
		/* 2D components*/
	case ComponentType::TRANSFORM_2D:
		return "Transform 2D";
	case ComponentType::UI_BUTTON:
		return "UI Button";
	case ComponentType::UI_IMAGE:
		return "UI Image";
	case ComponentType::UI_INPUT_TEXT:
		return "UI Input Text";
	case ComponentType::UI_LABEL:
		return "UI Label";
	default:
		return "";
	}
}

ComponentType GetComponentType(const char* componentTypeString)
{
	/* 3D components*/
	if (strcmp(componentTypeString, "Animation") == 0)
		return ComponentType::ANIMATION;
	else if (strcmp(componentTypeString, "Camera") == 0)
		return ComponentType::CAMERA;
	else if (strcmp(componentTypeString, "Grass") == 0)
		return ComponentType::GRASS;
	else if (strcmp(componentTypeString, "Material") == 0)
		return ComponentType::MATERIAL;
	else if (strcmp(componentTypeString, "Mesh") == 0)
		return ComponentType::MESH;
	else if (strcmp(componentTypeString, "Particle System") == 0)
		return ComponentType::PARTICLE_SYSTEM;
	else if (strcmp(componentTypeString, "Transform") == 0)
		return ComponentType::TRANSFORM;
	/* 2D components*/
	else if (strcmp(componentTypeString, "Transform 2D") == 0)
		return ComponentType::TRANSFORM_2D;
	else if (strcmp(componentTypeString, "UI Button") == 0)
		return ComponentType::UI_BUTTON;
	else if (strcmp(componentTypeString, "UI Image") == 0)
		return ComponentType::UI_IMAGE;
	else if (strcmp(componentTypeString, "UI Input Text") == 0)
		return ComponentType::UI_INPUT_TEXT;
	else if (strcmp(componentTypeString, "UI Label") == 0)
		return ComponentType::UI_LABEL;

	assert(false);
	return ComponentType::ANIMATION;
}
