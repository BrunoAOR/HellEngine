#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include <vector>
#include "MathGeoLib/src/Math/float3.h"
#include "Component.h"
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
	void SetCustomModel(const ModelInfo& modelInfo);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

public:

	bool activeModelInfoChanged = true;

private:

	void CreateCubeVAO();
	void CreateSphereVAO(uint rings, uint sections);

	void UpdateBoundingBox();

private:

	static uint meshesCount;
	static std::vector<ModelInfo> defaultModelInfos;
	
	int activeModelInfo = -1;

};

#endif // !__H_COMPONENT_MESH__
