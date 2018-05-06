#ifndef __H_MESH_INFO__
#define __H_MESH_INFO__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "globals.h"

class ComponentTransform;

struct BoneWeight
{
	uint vertexIndex = 0;
	float weight = 0.0f;
};

//struct BoneWeightGroup
//{
//	std::vector<uint> vertexIndices;
//	Bone* bone = nullptr;
//	float weight = 0.0f;
//};

struct Bone
{
	const char* name = nullptr;
	BoneWeight* weights = nullptr;
	uint numWeights = 0;
	float4x4 inverseBindMatrix;
};

struct VerticesGroup
{
	//std::vector<uint> verticesId;
	std::vector<Bone*> bones;
	std::vector<float> weights;
};

struct MeshInfo
{
	const char* name = nullptr;
	uint vao = 0;
	uint vbo = 0;
	uint ebo = 0;
	uint elementsCount = 0;
	uint vertexDataOffset = 0;
	std::vector<float3> vertices;
	std::vector<uint> indices;
	std::vector<Bone*> bones;
	std::map<std::vector<uint>, VerticesGroup> verticesGroups;
};

#endif // !__H_MESH_INFO__
