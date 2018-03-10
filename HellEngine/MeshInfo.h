#ifndef __H_MESH_INFO__
#define __H_MESH_INFO__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "globals.h"

struct BoneWeight
{
	uint vertexIndex = 0;
	float weight = 0.0f;
};

struct Bone
{
	const char* name = nullptr;
	BoneWeight* weights = nullptr;
	uint numWeights = 0;
	float4x4 inverseBindMatrix;
};

struct MeshInfo
{
	const char* name = nullptr;
	uint vao = 0;
	uint vbo = 0;
	uint ebo = 0;
	uint elementsCount = 0;
	std::vector<float3> vertices;
	std::vector<uint> indices;
	std::map<const char*, Bone*> bones;
};

#endif // !__H_MESH_INFO__
