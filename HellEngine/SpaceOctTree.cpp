#include <algorithm>
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "SpaceOctNode.h"
#include "SpaceOctTree.h"

SpaceOctTree::SpaceOctTree(): bucketSize(1), maxDepth(5)
{
}

SpaceOctTree::~SpaceOctTree()
{
	CleanUp();
}

int SpaceOctTree::Create(float3 minPoint, float3 maxPoint)
{
	CleanUp();
	type = OctTreeType::FIXED;
	if (minPoint.x < maxPoint.x && minPoint.y < maxPoint.y && minPoint.z < maxPoint.z)
	{
		minContainedPoint = minPoint;
		maxContainedPoint = maxPoint;
		node = new SpaceOctNode(minPoint, maxPoint, this, 1);
		return 0;
	}
	return 1;
}

int SpaceOctTree::Create(const std::vector<GameObject*>& gameObjects)
{
	CleanUp();
	type = OctTreeType::ADAPTIVE;
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

	node = new SpaceOctNode(minContainedPoint, maxContainedPoint, this, 1);

	for (GameObject* go : containedGameObjects)
	{
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		failCount += node->Insert(transform) ? 0 : 1;
	}
	return failCount;
}

SpaceOctTree::OctTreeType SpaceOctTree::GetType()
{
	return type;
}

int SpaceOctTree::Insert(GameObject* gameObject)
{
	if (!gameObject)
		return 1;

	int failCount = 1;

	/*
	A different vector must be created for ADAPTIVE QuadTree
	because the Create function calls CleanUp
	which will clear the containedGameObjects vector.
	*/
	std::vector<GameObject*> newGameObjectsVector(containedGameObjects);
	newGameObjectsVector.push_back(gameObject);

	switch (type)
	{
	case SpaceOctTree::OctTreeType::INVALID:
		failCount = 1;
		break;
	case SpaceOctTree::OctTreeType::FIXED:
		failCount = InsertFixed(gameObject);
		break;
	case SpaceOctTree::OctTreeType::ADAPTIVE:
		if (gameObject->GetComponent(ComponentType::TRANSFORM))
		{
			failCount = Create(newGameObjectsVector);
		}
		break;
	}
	LOGGER("SpaceOctTree now contains %i GameObjects.", containedGameObjects.size());
	return failCount;
}

int SpaceOctTree::Insert(std::vector<GameObject*> gameObjects)
{
	int failCount = 0;

	/*
	A different vector must be created for ADAPTIVE QuadTree
	because the Create function calls CleanUp
	which will clear the containedGameObjects vector.
	*/
	std::vector<GameObject*> newGameObjectsVector(containedGameObjects);
	newGameObjectsVector.insert(newGameObjectsVector.end(), gameObjects.begin(), gameObjects.end());

	switch (type)
	{
	case SpaceOctTree::OctTreeType::INVALID:
		failCount = gameObjects.size();
		break;
	case SpaceOctTree::OctTreeType::FIXED:
		for (GameObject* go : gameObjects)
			failCount += InsertFixed(go);
		break;
	case SpaceOctTree::OctTreeType::ADAPTIVE:
		failCount = Create(newGameObjectsVector);
		break;
	default:
		failCount = 1;
		break;
	}
	LOGGER("SpaceOctTree now contains %i GameObjects.", containedGameObjects.size());
	return failCount;

}

bool SpaceOctTree::Remove(GameObject* gameObject)
{
	if (type == OctTreeType::INVALID)
		return false;

	std::vector<GameObject*>::iterator it = std::find(containedGameObjects.begin(), containedGameObjects.end(), gameObject);
	if (it == containedGameObjects.end())
		return false;

	containedGameObjects.erase(it);

	bool result = false;
	/*
	If the gameObject WAS previously added to the SpaceOctTree,
	then it should contain a ComponentTransform
	*/
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	/*
	A different vector must be created for ADAPTIVE OCtTree
	because the Create function calls CleanUp
	which will clear the containedGameObjects vector.
	*/
	std::vector<GameObject*> newGameObjectsVector(containedGameObjects);

	switch (type)
	{
	case SpaceOctTree::OctTreeType::FIXED:
		/*
		The transform should be in the SpaceNode hierarchy
		*/
		result = node->Remove(transform);
		assert(result);
		break;
	case SpaceOctTree::OctTreeType::ADAPTIVE:
		/*
		Recreate the whole SpaceOctTree.
		*/
		result = Create(newGameObjectsVector);
		break;
	default:
		result = false;
		break;
	}
	LOGGER("SpaceQuadTree now contains %i GameObjects.", containedGameObjects.size());
	return result;
}

void SpaceOctTree::DrawTree()
{
	if (type != OctTreeType::INVALID && node)
		node->DrawNode();
}

void SpaceOctTree::CleanUp()
{
	type = OctTreeType::INVALID;
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

int SpaceOctTree::InsertFixed(GameObject* gameObject)
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

