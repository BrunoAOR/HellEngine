#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include <vector>
#include "MathGeoLib\src\Math\float3.h"
#include "Component.h"
#include "Model.h"
#include "VAOInfo.h"
#include "globals.h"

struct ModelInfo;
typedef float GLfloat;

class ComponentMesh :
	public Component
{
public:

	ComponentMesh(GameObject* owner);
	virtual ~ComponentMesh() override;

	const ModelInfo* GetActiveModelInfo() const;
	bool SetActiveModelInfo(int index);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

public:

	bool activeVaoChanged = true;

private:

	void CreateCubeVAO();
	void CreateSphereVAO(uint rings, uint sections);

	void UpdateBoundingBox();
	bool LoadModel();
	void UnloadModel();

private:

	static uint meshesCount;
	static std::vector<ModelInfo> defaultModelInfos;
	
	int activeVao = -1;
	Model model;
	char modelPath[256] = "";

};

#endif // !__H_COMPONENT_MESH__
