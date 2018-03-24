#ifndef __H_MODULE_DEBUG_DRAW__
#define __H_MODULE_DEBUG_DRAW__

#include "Module.h"
#include "globals.h"
class ShaderProgram;

class ModuleDebugDraw : public Module
{
public:

	ModuleDebugDraw();
	~ModuleDebugDraw();

public:

	bool Init();
	bool CleanUp();
	
	/* Draws a certain model using the Material's shader and texture, from a Vertex Array Oject WITH indexes */
	bool DrawElements(const float* modelMatrix, uint vao, uint vertexCount);
	
private:
	const ShaderProgram* shaderProgram = nullptr;
};

#endif

