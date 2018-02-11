#include <assert.h>
#include "ComponentTransform.h"
#include "SpaceNode.h"

SpaceNode::SpaceNode(float3 minPoint, float3 maxPoint, unsigned int aBucketSize):  bucketSize(aBucketSize), childrenCount(4)
{
	assert(bucketSize > 0);
	aabb.SetNegativeInfinity();
	vec points[2] = { minPoint, maxPoint };
	aabb.Enclose(points, 2);
}


SpaceNode::~SpaceNode()
{
	for (int i = 0; i < childrenCount; ++i)
	{
		delete nodes[i];
		nodes[i] = nullptr;
	}
}

bool SpaceNode::Insert(ComponentTransform* transform)
{
	if (isLeaf)
	{
		AABB goBoundingBox = transform->GetBoundingBox();
		if (goBoundingBox.MinX() < aabb.MaxX() && goBoundingBox.MinY() < aabb.MaxY() && goBoundingBox.MinZ() < aabb.MaxZ()
			&& goBoundingBox.MaxX() > aabb.MinX() && goBoundingBox.MaxY() > aabb.MinY() && goBoundingBox.MaxZ() > aabb.MinZ())
		{
			containedTransforms.push_back(transform);
			CheckBucketSize();
			return true;
		}
	}
	else
	{
		for (int i = 0; i < childrenCount; ++i)
		{
			if (nodes[i]->Insert(transform))
				return true;
		}
	}
	return false;
}

void SpaceNode::CheckBucketSize()
{
}
