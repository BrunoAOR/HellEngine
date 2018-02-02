#ifndef __H_COMPONENT_MATERIAL__
#define __H_COMPONENT_MATERIAL__

#include <string>
#include "Component.h"
#include "globals.h"
class
	Shader;

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial();
	virtual ~ComponentMaterial() override;

	/* Recieves the vertex shader file path and tries to compile it */
	bool SetVertexShaderPath(const std::string& sourcePath);

	/* Recieves the fragment shader file path and tries to compile it */
	bool SetFragmentShaderPath(const std::string& sourcePath);

	/* Recieves the texture file path and tries to load it using Devil */
	bool SetTexturePath(const std::string& sourcePath);

	/* Recieves the shader data file path and checks its validity */
	bool SetShaderDataPath(const std::string& sourcePath);

	/* Returns whether the Material has a loaded texture and a linked shader */
	bool IsValid();

	/* Attemps to link the shader, if a valid texture has been set */
	bool Apply();

	/* Reloads the vertex shader, fragment shader and texture, recompiles the shaders and relinks the shader program */
	bool Reapply();

	virtual void OnEditor() override;

	/* Draws a certain model using the Material's shader and texture */
	bool DrawArray(float* modelMatrix, uint drawDataBufferId, uint vertexCount);

private:

	bool GenerateUniforms();
	void UpdateUniforms();

private:

	std::string name;
	Shader* shader = nullptr;
	uint textureBufferId = 0;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string texturePath;
	std::string shaderDataPath;
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
	std::vector<Uniform> uniforms;
};

#endif // !__H_COMPONENT_MATERIAL__
