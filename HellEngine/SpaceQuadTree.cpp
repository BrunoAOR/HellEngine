#include <algorithm>
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
	CleanUp();
}

int SpaceQuadTree::Create(float3 minPoint, float3 maxPoint)
{
	CleanUp();
	type = QuadTreeType::FIXED;
	if (minPoint.x < maxPoint.x && minPoint.y < maxPoint.y && minPoint.z < maxPoint.z)
	{
		minContainedPoint = minPoint;
		maxContainedPoint = maxPoint;
		node = new SpaceNode(minPoint, maxPoint, bucketSize);
		return 0;
	}
	return 1;
}

int SpaceQuadTree::Create(const std::vector<GameObject*>& gameObjects)
{
	CleanUp();
	type = QuadTreeType::ADAPTIVE;
	int failCount = 0;
	
	for (GameObject* go : gameObjects)
	{
		if (go)
		{
			if (ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM))
			{
				/* Add gameObject to internal buffer (used if the Tree needs to be recreated upon inserting a new GameObject with Insert method) */
				containedGameObjects.push_back(go);

				/* Determine the minPoint and maxPoint for the node to be created */
				AABB goBoundingBox = transform->GetBoundingBox();
				float3 goMinPoint = goBoundingBox.minPoint;
				float3 goMaxPoint = goBoundingBox.maxPoint;

				minContainedPoint.x = minContainedPoint.x <= goMinPoint.x ? minContainedPoint.x : goMinPoint.x;
				minContainedPoint.y = minContainedPoint.y <= goMinPoint.y ? minContainedPoint.y : goMinPoint.y;
				minContainedPoint.z = minContainedPoint.z <= goMinPoint.z ? minContainedPoint.z : goMinPoint.z;

				maxContainedPoint.x = maxContainedPoint.x >= goMaxPoint.x ? maxContainedPoint.x : goMaxPoint.x;
				maxContainedPoint.y = maxContainedPoint.y >= goMaxPoint.y ? maxContainedPoint.y : goMaxPoint.y;
				maxContainedPoint.z = maxContainedPoint.z >= goMaxPoint.z ? maxContainedPoint.z : goMaxPoint.z;
			}
			else
				++failCount;
		}
	}
	
	node = new SpaceNode(minContainedPoint, maxContainedPoint, bucketSize);
	
	for (GameObject* go : containedGameObjects)
	{
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		failCount += node->Insert(transform) ? 0 : 1;
	}
	return failCount;
}

SpaceQuadTree::QuadTreeType SpaceQuadTree::GetType()
{
	return type;
}

int SpaceQuadTree::Insert(GameObject* gameObject)
{
	if (!gameObject)
		return 1;

	switch (type)
	{
	case SpaceQuadTree::QuadTreeType::INVALID:
		if (gameObject->GetComponent(ComponentType::TRANSFORM))
			return Create(std::vector<GameObject*>{gameObject});
		break;

	case SpaceQuadTree::QuadTreeType::FIXED:
		return InsertFixed(gameObject);
	
	case SpaceQuadTree::QuadTreeType::ADAPTIVE:
		if (gameObject->GetComponent(ComponentType::TRANSFORM))
		{
			containedGameObjects.push_back(gameObject);
			return Create(containedGameObjects);
		}
		break;
	}
	return 1;
}

int SpaceQuadTree::Insert(std::vector<GameObject*> gameObjects)
{
	int failCount = 0;

	switch (type)
	{
	case SpaceQuadTree::QuadTreeType::INVALID:
		return Create(gameObjects);
	
	case SpaceQuadTree::QuadTreeType::FIXED:
		for (GameObject* go : gameObjects)
			failCount += InsertFixed(go);
		return failCount;
	
	case SpaceQuadTree::QuadTreeType::ADAPTIVE:
		containedGameObjects.insert(containedGameObjects.end(), gameObjects.begin(), gameObjects.end());
		return Create(containedGameObjects);

	default:
		return 1;
	}
}

bool SpaceQuadTree::Remove(GameObject* gameObject)
{
	if (type == QuadTreeType::INVALID)
		return false;

	std::vector<GameObject*>::iterator it = std::find(containedGameObjects.begin(), containedGameObjects.end(), gameObject);
	if (it == containedGameObjects.end())
		return false;

	containedGameObjects.erase(it);
	
	bool result = false;
	/*
	If the gameObject WAS previously added to the SpaceQuadTree,
	then it should contain a ComponentTransform
	*/
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	std::vector<GameObject*> newGameObjectsVector(containedGameObjects);

	switch (type)
	{
	case SpaceQuadTree::QuadTreeType::FIXED:
		/*
		The transform should be in the SpaceNode hierarchy
		*/
		result = node->Remove(transform);
		assert(result);
		break;
	case SpaceQuadTree::QuadTreeType::ADAPTIVE:
		/*
		Recreate the whole SpaceQuadTree.
		A different vector must be created
		because the Create function calls CleanUp
		which will clear the containedGameObjects vector.
		*/
		result = Create(newGameObjectsVector);
		break;
	default:
		result = false;
		break;
	}
	return result;
}

void SpaceQuadTree::DrawTree()
{
	if (node)
		node->DrawNode();
}

void SpaceQuadTree::CleanUp()
{
	type = QuadTreeType::INVALID;
	containedGameObjects.clear();
	if (node != nullptr)
	{
		delete node;
		node = nullptr;
	}
	minContainedPoint.x = FLT_MAX;
	minContainedPoint.y = FLT_MAX;
	minContainedPoint.z = FLT_MAX;
	maxContainedPoint.x = FLT_MIN;
	maxContainedPoint.y = FLT_MIN;
	maxContainedPoint.z = FLT_MIN;
}

int SpaceQuadTree::InsertFixed(GameObject* gameObject)
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (!transform)
		return 1;

	containedGameObjects.push_back(gameObject);
	AABB goBoundingBox = transform->GetBoundingBox();
	float3 goMinPoint = goBoundingBox.minPoint;
	float3 goMaxPoint = goBoundingBox.maxPoint;

	if (goMinPoint.x > maxContainedPoint.x && goMinPoint.y > maxContainedPoint.y && goMinPoint.z > maxContainedPoint.z
		|| goMaxPoint.x < minContainedPoint.x && goMaxPoint.y < minContainedPoint.y && goMaxPoint.z < minContainedPoint.z)
		return 1;

	if (node->Insert(transform))
		return 0;
	else
		return 1;
}
