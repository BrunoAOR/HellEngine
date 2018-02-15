#ifndef __H_SPACE_QUAD_NODE__
#define __H_SPACE_QUAD_NODE__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Geometry/AABB.h"
class ComponentTransform;
class SpaceQuadTree;

class SpaceQuadNode
{
public:
	SpaceQuadNode(float3 minPoint, float3 maxPoint, SpaceQuadTree* quadTree, unsigned int depth);
	~SpaceQuadNode();

	bool CanContain(ComponentTransform* transform);
	bool CanContain(AABB goAABB);
	bool Insert(ComponentTransform* transform);
	bool Remove(ComponentTransform* transform);
	void DrawNode();
	template<typename T>
	void CollectIntersections(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);
private:

	void CheckBucketSize();
	void CreateVAO();

private:
	SpaceQuadTree* quadTree = nullptr;
	const unsigned int depth;
	AABB aabb;
	bool isLeaf = true;
	const int childrenCount;
	SpaceQuadNode* childNodes[4] = { nullptr, nullptr, nullptr, nullptr };
	std::vector<ComponentTransform*> containedTransforms;
	VaoInfo quadVao;
};

template<typename T>
inline void SpaceQuadNode::CollectIntersections(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	static int checksPerformed = 0;
	if (depth == 1)
		checksPerformed = 0;

	++checksPerformed;
	if (primitive.Intersects(aabb))
	{
		for (std::vector<ComponentTransform*>::iterator it = this->containedTransforms.begin(); it != this->containedTransforms.end(); ++it)
		{
			++checksPerformed;
			if (primitive.Intersects((*it)->GetBoundingBox()))
				intersectedGameObjects.push_back((*it)->gameObject);
		}

		if (!isLeaf)
		{
			for (int i = 0; i < childrenCount; ++i)
				if (childNodes[i] != nullptr)
					childNodes[i]->CollectIntersections(intersectedGameObjects, primitive);
		}
	}

	if (depth == 1)
		quadTree->lastChecksPerformed = checksPerformed;
}

#endif // !__H_SPACE_QUAD_NODE__
