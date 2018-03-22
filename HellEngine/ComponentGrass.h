#ifndef __H_COMPONENT_GRASS__
#define __H_COMPONENT_GRASS__

#include "Billboard.h"
#include "Component.h"
#include "MeshInfo.h"
#include "TextureConfiguration.h"
#include "TextureInfo.h"

class Shader;

class ComponentGrass :
	public Component
{
public:
	ComponentGrass(GameObject* owner);
	virtual ~ComponentGrass() override;

	virtual void Update();

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
	void CreateQuadVAO();
	void UpdateBillboards();

	bool DrawElements();

	bool LoadVertexShader();
	bool LoadFragmentShader();
	bool LoadShaderData();
	bool LoadTexture();
	void ConfigureTexture();
	bool GenerateUniforms();
	void UpdatePublicUniforms();

	void OnEditorBillboardConfiguration();
	void OnEditorMaterialConfiguration();
	void OnEditorTextureInformation();
	void OnEditorTextureConfiguration();
	void OnEditorShaderOptions();

	Shader* ShaderAlreadyLinked();

private:
	uint textureID;
	std::vector<Billboard*> billboards;
	int billboardInstancesX = 1;
	int billboardInstancesZ = 1;
	float offsetX = 1;
	float offsetZ = 1;
	float3 position;
	float width = 1; 
	float height = 1;
	float randomPositionRange = 0;
	float randomScaleRange = 0;

	TextureConfiguration textureConfiguration;
	TextureInfo textureInfo;

	Shader* shader = nullptr;
	bool isValid = false;
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

	MeshInfo quadMeshInfo;
	float3 previousCameraPos;
	bool billboardsChanged;
};

#endif
