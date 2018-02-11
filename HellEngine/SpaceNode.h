#ifndef __H_SPACE_NODE__
#define __H_SPACE_NODE__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Geometry/AABB.h"
class ComponentTransform;

class SpaceNode
{
public:
	SpaceNode(float3 minPoint, float3 maxPoint, unsigned int bucketSize);
	~SpaceNode();

	bool Insert(ComponentTransform* transform);

private:

	void CheckBucketSize();

private:
	const unsigned int bucketSize;
	AABB aabb;
	bool isLeaf = true;
	const int childrenCount;
	SpaceNode* nodes[4] = { nullptr, nullptr, nullptr, nullptr };
	std::vector<ComponentTransform*> containedTransforms;
};

#endif // !__H_SPACE_NODE__
