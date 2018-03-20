#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include <vector>
#include "MathGeoLib/src/Math/float3.h"
#include "Component.h"
#include "ModelInfo.h"
#include "MeshInfo.h"
#include "globals.h"

typedef float GLfloat;

class ComponentMesh :
	public Component
{
public:

	ComponentMesh(GameObject* owner);
	virtual ~ComponentMesh() override;

	void Update() override;

	const ModelInfo* GetActiveModelInfo() const;
	bool SetActiveModelInfo(int index);
	void SetCustomModel(const ModelInfo& modelInfo);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

private:

	void CreateCubeMeshInfo();
	void CreateSphereMeshInfo(uint rings, uint sections);

	void UpdateBoundingBox();
	void ApplyVertexSkinning(const MeshInfo* meshInfo);

private:

	static uint meshesCount;
	static std::vector<ModelInfo> defaultModelInfos;
	ModelInfo customModelInfo;
	
	int activeModelInfo = -1;

};

#endif // !__H_COMPONENT_MESH__
