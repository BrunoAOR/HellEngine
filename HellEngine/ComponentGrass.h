#ifndef __H_COMPONENT_GRASS__
#define __H_COMPONENT_GRASS__

#include "Billboard.h"
#include "Component.h"
#include "MeshInfo.h"
#include "TextureConfiguration.h"
#include "TextureInfo.h"
class ShaderProgram;

class ComponentGrass :
	public Component
{
public:
	ComponentGrass(GameObject* owner);
	virtual ~ComponentGrass() override;

	virtual void Update();

	/* Draws the Inspector window section for this component */
	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:	
	void CreateQuadVAO();
	void UpdateBillboards();
	bool LoadTexture();

	bool DrawElements();

	void OnEditorBillboardConfiguration();

private:
	uint textureID = 0;
	const ShaderProgram* shaderProgram = nullptr;
	bool isValid = false;

	char texturePath[256] = "";

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

	MeshInfo quadMeshInfo;
	float3 previousCameraPos;
	bool billboardsChanged;
};

#endif
