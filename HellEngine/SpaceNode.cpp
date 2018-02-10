#include "SpaceNode.h"

SpaceNode::SpaceNode(float3 minPoint, float3 maxPoint): childCount(4)
{
	aabb.SetNegativeInfinity();
	vec points[2] = { minPoint, maxPoint };
	aabb.Enclose(points, 2);
}


SpaceNode::~SpaceNode()
{
	for (int i = 0; i < childCount; ++i)
	{
		delete nodes[i];
		nodes[i] = nullptr;
	}
}
