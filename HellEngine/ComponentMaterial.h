#ifndef __H_COMPONENT_MATERIAL__
#define __H_COMPONENT_MATERIAL__

#include <map>
#include <string>
#include <vector>
#include "Component.h"
#include "globals.h"
class ComponentMesh;
class ComponentTransform;
class Shader;

class ComponentMaterial :
	public Component
{
public:

	ComponentMaterial(GameObject* owner);
	virtual ~ComponentMaterial() override;

	virtual void Update() override;

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

	/* Draws the Inspector window section for this component */
	virtual void OnEditor() override;

	/* Draws a certain model using the Material's shader and texture */
	bool DrawArray(float* modelMatrix, uint vao, uint vertexCount);

	/* Draws a certain model using the Material's shader and texture, from a Vertex Array Oject WITH indexes */
	bool DrawElements(float* modelMatrix, uint vao, uint vertexCount, int indexesType);

private:

	bool GenerateUniforms();
	void UpdatePublicUniforms();

private:

	/* General */
	ComponentMesh* mesh = nullptr;
	ComponentTransform* transform = nullptr;

	/* Shader related */
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
	std::vector<Uniform> publicUniforms;
	std::map<std::string, uint> privateUniforms;
};

#endif // !__H_COMPONENT_MATERIAL__
