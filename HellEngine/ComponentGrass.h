#ifndef __H_COMPONENT_GRASS__
#define __H_COMPONENT_GRASS__

#include "globals.h"

class Shader;

class ComponentGrass
{
public:
	ComponentGrass();
	~ComponentGrass();

private:
	Shader* shader = nullptr;

	uint textureID;
};

#endif
