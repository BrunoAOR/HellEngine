#include <list>
#include "MathGeoLib/src/Geometry/Triangle.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "physicsFunctions.h"

struct GameObjectDistanceInfo {
	GameObject* gameObject = nullptr;
	ComponentTransform* transform = nullptr;
	float nearDistance = 0;
	float farDistance = 0;
};

struct TriangleDistanceInfo {
	GameObject* gameObject = nullptr;
	Triangle triangle;
	float hitDistance = std::numeric_limits<float>::max();
};

GameObject* CalculateCollisionsWithStaticGameObjects(const LineSegment& lineSegment)
{
	/* First, get all potential collision GameObjects from the scene's SpaceQuadTree */
	const SpaceQuadTree& quadTree = App->scene->GetQuadTree();
	std::vector<GameObject*> potentialCollisionGOs;
	quadTree.Intersects(potentialCollisionGOs, lineSegment);

	return CalculateCollisionsWithGameObjects(potentialCollisionGOs, lineSegment);
}

GameObject* CalculateCollisionsWithGameObjects(const std::vector<GameObject*>& gameObjects, const LineSegment& lineSegment)
{
	/* Pair the GameObjects with information about the distance of their AABB's collision with the LineSegment */
	std::list<GameObjectDistanceInfo> distanceSortedGOs;
	for (GameObject* go : gameObjects)
	{
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		assert(transform);
		GameObjectDistanceInfo goDistanceInfo;
		goDistanceInfo.gameObject = go;
		goDistanceInfo.transform = transform;
		lineSegment.Intersects(transform->GetBoundingBox(), goDistanceInfo.nearDistance, goDistanceInfo.farDistance);

		distanceSortedGOs.push_back(goDistanceInfo);
	}

	/* Sort them by distance based on the collision point closest to the start of the lineSegment */
	distanceSortedGOs.sort([](GameObjectDistanceInfo a, GameObjectDistanceInfo b) -> bool {return a.nearDistance < b.nearDistance; });

	/* Now iterate through the sorted GameObjects and calculate collision against every triangle in every GameObject's mesh (if available)
	Discard backface-triangles if backface culling is in use
	*/
	bool backfaceCulling = App->renderer->isBackFaceCullActive();
	bool goHit = false;
	TriangleDistanceInfo closestHitGameObject;
	float maxQueryDistance = lineSegment.Length();
	for (std::list<GameObjectDistanceInfo>::iterator it = distanceSortedGOs.begin(); it != distanceSortedGOs.end() && (*it).nearDistance <= maxQueryDistance; ++it)
	{
		GameObjectDistanceInfo goDistanceInfo = *it;
		ComponentMesh* mesh = (ComponentMesh*)goDistanceInfo.gameObject->GetComponent(ComponentType::MESH);
		if (mesh)
		{
			/* Transform the lineSegment to localSpace for this GameObject */
			LineSegment localLineSegment(lineSegment);
			/* Note: MathGeoLib handles matrices transposed in relation to OpenGL (and the GetModelMatrix4x4 method) */
			float4x4 inverseModelMatrix = (*it).transform->GetModelMatrix4x4();
			inverseModelMatrix.Transpose();
			inverseModelMatrix.Inverse();
			localLineSegment.Transform(inverseModelMatrix);

			/* Get vertices and indices */
			const VaoInfo& vaoInfo = mesh->GetActiveVao();
			const std::vector<float3>& vertices = vaoInfo.vertices;
			const std::vector<uint>& indices = vaoInfo.indices;
			assert(indices.size() % 3 == 0);

			/* Iterate through triangles looking for the closest hit */
			bool triangleHit = false;
			for (unsigned int idx = 0; idx < indices.size(); idx += 3)
			{
				Triangle triangle(vertices[indices[idx]], vertices[indices[idx + 1]], vertices[indices[idx + 2]]);
				
				/* Discard triangle if back face culling is active and the triangle is back facing */
				if (!backfaceCulling || Dot(triangle.NormalCCW(), lineSegment.b - lineSegment.a) < 0)
				{
					float hitDistance = 0;
					if (localLineSegment.Intersects(triangle, &hitDistance, nullptr) && hitDistance < closestHitGameObject.hitDistance)
					{
						triangleHit = true;
						closestHitGameObject.triangle = triangle;
						closestHitGameObject.hitDistance = hitDistance;
					}
				}
			}

			/* If a hit was found, reset the maxQueryDistance to the farHitDistance of the current GameObject's AABB
			This is required because a mesh inside an AABB contained within another AABB could be colliding with the LineSegment
			closer to the LineSegment origin than the mesh in the containing AABB does.
			*/
			if (triangleHit)
			{
				goHit = true;
				closestHitGameObject.gameObject = goDistanceInfo.gameObject;
				maxQueryDistance = goDistanceInfo.farDistance;
			}
		}
	}

	return closestHitGameObject.gameObject;
}
