#ifndef __H_SPACE_OCT_TREE__
#define __H_SPACE_OCT_TREE__

#include <vector>
#include "MathGeoLib/src/Math/float3.h"
#include "SpaceOctNode.h"
class GameObject;

class SpaceOctTree
{

public:

	SpaceOctTree();
	~SpaceOctTree();

	enum class OctTreeType { INVALID, FIXED, ADAPTIVE };


	/* Creates a fixed OctTree with the dimensions specified */
	int Create(float3 minPoint, float3 maxPoint);

	/* Creates an adaptive OctTree for the specified GameObjects in the gameObjects vector */
	int Create(const std::vector<GameObject*>& gameObjects);

	/* Returns the type of the OctTree */
	OctTreeType GetType();

	/*
	Adds the provided GameObject to the OctTree.
	The operation can be unsuccessful if the QuadTree is of type FIXED
	and the GameObject falls outside of the predefined bounds, or if the GameObject contains no Transform
	*/
	int Insert(GameObject* gameObject);

	/*
	Adds the provided GameObjects to the OctTree.
	Returns the number of GameObjects that could NOT be added to the QuadTree (if it is of type FIXED),
	because they fall outside of the predefined bounds or if the GameObjects contain no Transform.
	*/
	int Insert(std::vector<GameObject*> gameObjects);

	/* Removes the provided GameObject if it is found inside the SpaceOctTree */
	bool Remove(GameObject* gameObject);

	/* Tests collision of the provided primitive and gathers the intersected GameObjects in the provided intersectedGameObjects vector */
	template<typename T>
	void Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);

	void DrawTree();

	void CleanUp();

public:

	const unsigned int bucketSize;
	const unsigned int maxDepth;

private:

	int InsertFixed(GameObject* gameObject);

private:

	SpaceOctNode * node = nullptr;
	OctTreeType type;

	std::vector<GameObject*> containedGameObjects;
	float3 minContainedPoint;
	float3 maxContainedPoint;
};

template<typename T>
inline void SpaceOctTree::Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	node->CollectIntersections(intersectedGameObjects, primitive);
}


#endif

