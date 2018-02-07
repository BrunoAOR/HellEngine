#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include <vector>
#include "Component.h"
#include "globals.h"
#include "MathGeoLib\src\Math\float3.h"

typedef float GLfloat;

class ComponentMesh :
	public Component
{
public:
	struct VaoInfo
	{
		const char* name = "";
		uint vao = 0;
		uint elementsCount = 0;
		int indexesType = 0;
	};

	ComponentMesh(GameObject* owner);
	virtual ~ComponentMesh() override;

	VaoInfo GetActiveVao() const;
	bool SetActiveVao(uint index);

	void BuildVectorCubeVertexes(GLfloat arrayCubePoints[], int vertexesNumber);
	std::vector<float3> *GetBuildVectorCubePoints();

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

	void CreateCubeVAO();
	void CreateSphereVAO(uint rings, uint sections);

private:

	std::vector<VaoInfo> vaoInfos;
	std::vector<float3> vecCubeVertexes;
	int activeVao = -1;

};

#endif // !__H_COMPONENT_MESH__
