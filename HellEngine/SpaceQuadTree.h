#ifndef __H_SPACE_QUAD_TREE__
#define __H_SPACE_QUAD_TREE__

#include <vector>
#include "MathGeoLib/src/Math/float3.h"
#include "SpaceNode.h"
class GameObject;

class SpaceQuadTree
{
public:
	
	enum class QuadTreeType { INVALID, FIXED, ADAPTIVE };
	
	SpaceQuadTree();
	~SpaceQuadTree();

	/* Creates a fixed QuadTree with the dimensions specified */
	int Create(float3 minPoint, float3 maxPoint);

	/* Creates an adaptive QuadTree for the specified GameObjects in the gameObjects vector */
	int Create(const std::vector<GameObject*>& gameObjects);

	/* Returns the type of the QuadTree */
	QuadTreeType GetType();

	/*
	Adds the provided GameObject to the QuadTree.
	The operation can be unsuccessful if the QuadTree is of type FIXED
	and the GameObject falls outside of the predefined bounds, or if the GameObject contains no Transform
	*/
	int Insert(GameObject* gameObject);

	/*
	Adds the provided GameObjects to the QuadTree.
	Returns the number of GameObjects that could NOT be added to the QuadTree (if it is of type FIXED),
	because they fall outside of the predefined bounds or if the GameObjects contain no Transform.
	*/
	int Insert(std::vector<GameObject*> gameObjects);

	/* Removes the provided GameObject if it is found inside the SpaceQuadTree */
	bool Remove(GameObject* gameObject);

	/* Tests collision of the provided primitive and gathers the intersected GameObjects in the provided intersectedGameObjects vector */
	template<typename T>
	void Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);

	void DrawTree();

	void CleanUp();

private:

	int InsertFixed(GameObject* gameObject);

private:

	const unsigned int bucketSize;
	SpaceNode* node = nullptr;
	QuadTreeType type;

	std::vector<GameObject*> containedGameObjects;
	float3 minContainedPoint;
	float3 maxContainedPoint;
};


template<typename T>
inline void SpaceQuadTree::Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	node->CollectIntersections(intersectedGameObjects, primitive);
}

#endif // !__H_SPACE_QUAD_TREE__
