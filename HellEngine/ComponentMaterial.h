#ifndef __H_COMPONENT_MATERIAL__
#define __H_COMPONENT_MATERIAL__

#include <map>
#include <string>
#include <vector>
#include "Component.h"
#include "TextureConfiguration.h"
#include "TextureInfo.h"
#include "globals.h"

class ShaderProgram;
enum class ShaderOptions : unsigned int;

class ComponentMaterial :
	public Component
{
public:

	ComponentMaterial(GameObject* owner);
	virtual ~ComponentMaterial() override;
	
	/* Recieves the vertex shader file path and tries to compile it */
	bool SetVertexShaderPath(const std::string& sourcePath);

	/* Recieves the fragment shader file path and tries to compile it */
	bool SetFragmentShaderPath(const std::string& sourcePath);

	/* Stores the diffuse texture file path */
	bool SetTexturePath(const std::string& sourcePath);

	/* Stores the normal texture file path */
	bool SetNormalPath(const std::string& sourcePath);

	/* Stores the specular texture file path */
	bool SetSpecularPath(const std::string& sourcePath);

	/* Recieves the shader data file path and checks its validity */
	bool SetShaderDataPath(const std::string& sourcePath);

	/* Returns whether the Material has a loaded texture and a linked shader with valid data */
	bool IsValid() const;

	/* Attemps to apply all of the material setup */
	bool Apply();
	
	/* Applies the default material configuration */
	void SetDefaultMaterialConfiguration();

	/* Draws the Inspector window section for this component */
	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

	void UpdatePublicUniforms();

public:
	
	/* Mesh related */
	int modelInfoVaoIndex = -1;

	/* Shader related */
	const ShaderProgram* shaderProgram;
	uint diffuseBufferId = 0;
	uint normalBufferId = 0;
	uint specularBufferId = 0;

private:

	/* Attemps to apply all of the material setup */
	bool Apply(ShaderOptions shaderOptions);

	uint CreateCheckeredTexture();
	uint CreateDefaultNormalMap();
	uint CreateDefaultSpecularMap();

	bool LoadShaderData();
	bool LoadTexture(uint& bufferId, const char* path, uint fallbackDefaultId);
	void ConfigureTexture(uint bufferId);
	bool GenerateUniforms();

	void OnEditorMaterialConfiguration();
	void OnEditorTextureInformation();
	void OnEditorTextureConfiguration();
	void OnEditorShaderOptions();

private:

	/* General */
	static uint materialsCount;
	static uint checkeredPatternBufferId;
	static uint defaultNormalMapBufferId;
	static uint defaultSpecularMapBufferId;

	TextureConfiguration textureConfiguration;
	TextureInfo textureInfo;

	/* Shader related */

	bool isValid = false;
	char vertexShaderPath[256] = "";
	char fragmentShaderPath[256] = "";
	char diffusePath[256] = "";
	char normalPath[256] = "";
	char specularPath[256] = "";
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
};

#endif // !__H_COMPONENT_MATERIAL__
