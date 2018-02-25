#ifndef __H_VAO_INFO__
#define __H_VAO_INFO__

#include "MathGeoLib/src/Math/float3.h"
#include "globals.h"

struct VaoInfo
{
	const char* name = "";
	uint vao = 0;
	uint vbo = 0;
	uint ebo = 0;
	uint elementsCount = 0;
	std::vector<float3> vertices;
	std::vector<uint> indices;
};

#endif
