#ifndef __H_SPACE_NODE__
#define __H_SPACE_NODE__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Geometry/AABB.h"
class ComponentTransform;
class SpaceQuadTree;

class SpaceNode
{
public:
	SpaceNode(float3 minPoint, float3 maxPoint, SpaceQuadTree* quadTree, unsigned int depth);
	~SpaceNode();

	bool Insert(ComponentTransform* transform);
	bool Remove(ComponentTransform* transform);
	void DrawNode();
	template<typename T>
	void CollectIntersections(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);
private:

	void CheckBucketSize();

private:
	SpaceQuadTree* quadTree = nullptr;
	const unsigned int depth;
	AABB aabb;
	bool isLeaf = true;
	const int childrenCount;
	SpaceNode* nodes[4] = { nullptr, nullptr, nullptr, nullptr };
	std::vector<ComponentTransform*> containedTransforms;
};

template<typename T>
inline void SpaceNode::CollectIntersections(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	static int checksPerformed = 0;
	if (depth == 1)
		checksPerformed = 0;

	++checksPerformed;
	if (primitive.Intersects(aabb))
	{
		if (isLeaf)
		{
			for (std::vector<ComponentTransform*>::iterator it = this->containedTransforms.begin(); it != this->containedTransforms.end(); ++it)
			{
				++checksPerformed;
				if (primitive.Intersects((*it)->GetBoundingBox()))
					intersectedGameObjects.push_back((*it)->gameObject);
			}
		}
		else
		{
			for (int i = 0; i < childrenCount; ++i)
				if (nodes[i] != nullptr)
					nodes[i]->CollectIntersections(intersectedGameObjects, primitive);
		}
	}

	if (depth == 1)
		LOGGER("QuadTree checks: %i", checksPerformed);;
}

#endif // !__H_SPACE_NODE__
