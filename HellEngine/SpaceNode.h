#ifndef __H_SPACE_NODE__
#define __H_SPACE_NODE__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Geometry/AABB.h"

class SpaceNode
{
public:
	SpaceNode(float3 minPoint, float3 maxPoint);
	~SpaceNode();

private:
	AABB aabb;
	bool isLeaf = true;
	const int childCount;
	SpaceNode* nodes[4] = { nullptr, nullptr, nullptr, nullptr };
};

#endif // !__H_SPACE_NODE__
