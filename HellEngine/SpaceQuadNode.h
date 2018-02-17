#ifndef __H_SPACE_QUAD_NODE__
#define __H_SPACE_QUAD_NODE__

#include <stack>
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
	SpaceQuadTree * quadTree = nullptr;
	const unsigned int depth;
	AABB aabb;
	bool isLeaf = true;
	const int childrenCount = 0;
	SpaceQuadNode* childNodes[4] = { nullptr, nullptr, nullptr, nullptr };
	std::vector<ComponentTransform*> containedTransforms;
	VaoInfo quadVao;
};

template<typename T>
inline void SpaceQuadNode::CollectIntersections(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	int checksPerformed = 0;

	std::stack<SpaceQuadNode*> stack;

	SpaceQuadNode* node = this;

	stack.push(node);

	BROFILER_CATEGORY("QuadNode::CollectIntersections", Profiler::Color::Green);
	while (!stack.empty()) {
		node = stack.top();
		stack.pop();

		++checksPerformed;
		if (primitive.Intersects(node->aabb))
		{
			for (uint i = 0; i < node->containedTransforms.size(); i++) {
				++checksPerformed;
				if (primitive.Intersects(node->containedTransforms.at(i)->GetBoundingBox()))
					intersectedGameObjects.push_back(node->containedTransforms.at(i)->gameObject);
			}

			for (int i = 0; i < node->childrenCount; ++i)
				if (node->childNodes[i] != nullptr)
					stack.push(node->childNodes[i]);

		}

		node->quadTree->lastChecksPerformed = checksPerformed;
	}
}

#endif // !__H_SPACE_QUAD_NODE__
