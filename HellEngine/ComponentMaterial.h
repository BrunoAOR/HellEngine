#ifndef __H_COMPONENT_MATERIAL__
#define __H_COMPONENT_MATERIAL__

#include <map>
#include <string>
#include <vector>
#include "Component.h"
#include "TextureConfiguration.h"
#include "TextureInfo.h"
#include "globals.h"

class ComponentMesh;
class ComponentTransform;
class Shader;
struct VaoInfo;

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

	/* Returns whether the Material has a loaded texture and a linked shader with valid data */
	bool IsValid();

	/* Attemps to apply all of the material setup */
	bool Apply();
	
	/* Applies the default material configuration */
	void SetDefaultMaterialConfiguration();

	/* Draws the Inspector window section for this component */
	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

	/* Draws a certain model using the Material's shader and texture, from a Vertex Array Oject WITH indexes */
	bool DrawElements(float* modelMatrix, uint vao, uint vertexCount);

	uint CreateCheckeredTexture();

	bool LoadVertexShader();
	bool LoadFragmentShader();
	bool LoadShaderData();
	bool LoadTexture();
	void ConfigureTexture();
	bool GenerateUniforms();
	void UpdatePublicUniforms();

	void OnEditorMaterialConfiguration();
	void OnEditorTextureInformation();
	void OnEditorTextureConfiguration();
	void OnEditorShaderOptions();

	Shader* ShaderAlreadyLinked();

private:

	/* General */
	static uint checkeredPatternBufferId;
	static uint checkeredTextureCount;

	TextureConfigutaion textureConfiguration;
	TextureInfo textureInfo;

	/* Shader related */
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

	static std::vector<Shader*> loadedShaders;
	static std::map<Shader*, uint> loadedShaderCount;

	const VaoInfo* vaoInfo;
};

#endif // !__H_COMPONENT_MATERIAL__
