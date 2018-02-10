#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "SpaceNode.h"
#include "SpaceQuadTree.h"

SpaceQuadTree::SpaceQuadTree(): bucketSize(1)
{
}


SpaceQuadTree::~SpaceQuadTree()
{
	delete node;
	node = nullptr;
}

void SpaceQuadTree::Create(float3 minPoint, float3 maxPoint)
{
	node = new SpaceNode(minPoint, maxPoint);
}

void SpaceQuadTree::Create(const std::vector<GameObject*>& gameObjects)
{
	float3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);
	float3 maxPoint(FLT_MIN, FLT_MIN, FLT_MIN);
	for (GameObject* go : gameObjects)
	{
		if (ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM))
		{
			/* Add gameObject to internal buffer (used if the Tree needs to be recreated upon inserting a new GameObject with Insert method) */
			containedGameObjects.push_back(go);
			
			/* Determine the minPoint and maxPoint for the node to be created */
			AABB goBoundingBox = transform->GetBoundingBox();
			float3 goMinPoint = goBoundingBox.CornerPoint(0); /* The point with the smallest x, y & z coordinate */
			float3 goMaxPoint = goBoundingBox.CornerPoint(7); /* The point with the largest x, y & z coordinate */
			
			minPoint.x = minPoint.x <= goMinPoint.x ? minPoint.x : goMinPoint.x;
			minPoint.y = minPoint.y <= goMinPoint.y ? minPoint.y : goMinPoint.y;
			minPoint.z = minPoint.z <= goMinPoint.z ? minPoint.z : goMinPoint.z;

			maxPoint.x = maxPoint.x >= goMaxPoint.x ? maxPoint.x : goMaxPoint.x;
			maxPoint.y = maxPoint.y >= goMaxPoint.y ? maxPoint.y : goMaxPoint.y;
			maxPoint.z = maxPoint.z >= goMaxPoint.z ? maxPoint.z : goMaxPoint.z;
		}
	}

	node = new SpaceNode(minPoint, maxPoint);
		
}

SpaceQuadTree::QuadTreeType SpaceQuadTree::GetType()
{
	return type;
}

int SpaceQuadTree::Insert(GameObject * gameObject)
{
	return 0;
}

int SpaceQuadTree::Insert(std::vector<GameObject*> gameObjects)
{
	return 0;
}

bool SpaceQuadTree::Remove(GameObject * gameObject)
{
	return false;
}
