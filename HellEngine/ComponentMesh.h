#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include <vector>
#include "Component.h"
#include "globals.h"
#include "VAOInfo.h"
#include "MathGeoLib\src\Math\float3.h"

typedef float GLfloat;

class ComponentMesh :
	public Component
{
public:

	ComponentMesh(GameObject* owner);
	virtual ~ComponentMesh() override;

	VaoInfo GetActiveVao() const;
	bool SetActiveVao(uint index);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

	void CreateCubeVAO();
	void CreateSphereVAO(uint rings, uint sections);

	void UpdateBoundingBox();

private:

	static std::vector<VaoInfo> vaoInfos;
	int activeVao = -1;

};

#endif // !__H_COMPONENT_MESH__
