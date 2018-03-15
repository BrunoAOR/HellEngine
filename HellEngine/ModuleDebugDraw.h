#ifndef __H_MODULE_DEBUG_DRAW__
#define __H_MODULE_DEBUG_DRAW__

#include <map>
#include <string>
#include <vector>
#include "Module.h"
#include "globals.h"

class Shader;

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
	/* Applies the default material configuration */
	void SetDefaultMaterialConfiguration();

	bool LoadVertexShader();
	bool LoadFragmentShader();
	bool LoadShaderData();
	bool GenerateUniforms();
	void UpdatePublicUniforms();
	
private:

	bool isValid = false;
	Shader* shader = nullptr;
	uint textureBufferId = 0;
	char vertexShaderPath[256] = "";
	char fragmentShaderPath[256] = "";
	char texturePath[256] = "";
	char shaderDataPath[256] = "";
	std::string shaderData;

	struct Uniform {
		std::string name;
		uint location;
		enum class UniformType {
			FLOAT,
			COLOR4
		} type;
		uint size;
		float values[4];
	};
	std::vector<Uniform> publicUniforms;
	std::map<std::string, uint> privateUniforms;
};

#endif

