#include <assert.h>
#include "ComponentTransform.h"
#include "SpaceQuadNode.h"
#include "SpaceQuadTree.h"
#include "openGL.h"

SpaceQuadNode::SpaceQuadNode(float3 minPoint, float3 maxPoint, SpaceQuadTree* aQuadTree, unsigned int aDepth)
	:  quadTree(aQuadTree), depth(aDepth), childrenCount(4)
{
	assert(quadTree);
	aabb.SetNegativeInfinity();
	vec points[2] = { minPoint, maxPoint };
	aabb.Enclose(points, 2);
}


SpaceQuadNode::~SpaceQuadNode()
{
	for (int i = 0; i < childrenCount; ++i)
	{
		delete nodes[i];
		nodes[i] = nullptr;
	}
}

bool SpaceQuadNode::Insert(ComponentTransform* transform)
{
	if (isLeaf)
	{
		AABB goBoundingBox = transform->GetBoundingBox();
		if (goBoundingBox.MinX() < aabb.MaxX() && goBoundingBox.MinY() < aabb.MaxY() && goBoundingBox.MinZ() < aabb.MaxZ()
			&& goBoundingBox.MaxX() > aabb.MinX() && goBoundingBox.MaxY() > aabb.MinY() && goBoundingBox.MaxZ() > aabb.MinZ())
		{
			containedTransforms.push_back(transform);
			CheckBucketSize();
			return true;
		}
	}
	else
	{
		for (int i = 0; i < childrenCount; ++i)
		{
			if (nodes[i]->Insert(transform))
				return true;
		}
	}
	return false;
}

bool SpaceQuadNode::Remove(ComponentTransform* transform)
{
	if (isLeaf)
	{
		for (std::vector<ComponentTransform*>::iterator it = containedTransforms.begin(); it != containedTransforms.end(); ++it)
		{
			if ((*it) == transform)
			{
				containedTransforms.erase(it);
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < childrenCount; ++i)
		{
			if (nodes[i]->Remove(transform))
				return true;
		}
	}
	return false;
}

void SpaceQuadNode::DrawNode()
{
	/*
	Node's AABB drawing
	  H-----G
	 /|    /|
	D-----C |
	| |   | |
	| E---|-F
	|/    |/
	A-----B

	x to right, y up, z to front
	ABCD is the front face
	FEHG is the back face
	*/
	if (isLeaf)
	{
		float3 vA = aabb.CornerPoint(1);
		float3 vB = aabb.CornerPoint(5);
		float3 vC = aabb.CornerPoint(7);
		float3 vD = aabb.CornerPoint(3);
		float3 vE = aabb.CornerPoint(0);
		float3 vF = aabb.CornerPoint(4);
		float3 vG = aabb.CornerPoint(6);
		float3 vH = aabb.CornerPoint(2);
		float vertices[8 * 3] = { vA.x, vA.y, vA.z, vB.x, vB.y, vB.z, vC.x, vC.y, vC.z, vD.x, vD.y, vD.z, vE.x, vE.y, vE.z, vF.x, vF.y, vF.z, vG.x, vG.y, vG.z, vH.x, vH.y, vH.z, };
		GLubyte vertIndexes[] = {
			0, 1,	1, 2,	2, 3,	3, 0,	/* front face */
			4, 5,	5, 6,	6, 7,	7, 4,	/* back face */
			0, 4,	1, 5,	2, 6,	3, 7	/* front-back links */
		};
		GLubyte cornerIndexes[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

		float currentLineWidth = 0;
		glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
		glLineWidth(2.0f);
		float currentPointSize = 0;
		glGetFloatv(GL_POINT_SIZE, &currentPointSize);
		glPointSize(10.0f);
		float currentColor[4];
		glGetFloatv(GL_CURRENT_COLOR, currentColor);
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glDrawElements(GL_LINES, 12 * 2, GL_UNSIGNED_BYTE, vertIndexes);
		glDrawElements(GL_POINTS, 8, GL_UNSIGNED_BYTE, cornerIndexes);
		glDisableClientState(GL_VERTEX_ARRAY);

		glLineWidth(currentLineWidth);
		glPointSize(currentPointSize);
		glColor4fv(currentColor);
	}
	else
	{
		for (int i = 0; i < childrenCount; ++i)
			nodes[i]->DrawNode();
	}
}

void SpaceQuadNode::CheckBucketSize()
{
	if (containedTransforms.size() > quadTree->bucketSize && depth < quadTree->maxDepth)
	{
		isLeaf = false;
		/* Divide in X and Y */
		float3 minPoint = aabb.minPoint;
		float3 maxPoint = aabb.maxPoint;
		float minX = minPoint.x;
		float midX = minPoint.x + (maxPoint.x - minPoint.x) / 2;
		float maxX = maxPoint.x;
		float minY = minPoint.y;
		float maxY = maxPoint.y;
		float minZ = minPoint.z;
		float midZ = minPoint.z + (maxPoint.z - minPoint.z) / 2;
		float maxZ = maxPoint.z;
		/* Create child nodes */
		nodes[0] = new SpaceQuadNode(minPoint, float3(midX, maxY, midZ), quadTree, depth + 1);
		nodes[1] = new SpaceQuadNode(float3(midX, minY, minZ), float3(maxX, maxY, midZ), quadTree, depth + 1);
		nodes[2] = new SpaceQuadNode(float3(minX, minY, midZ), float3(midX, maxY, maxZ), quadTree, depth + 1);
		nodes[3] = new SpaceQuadNode(float3(midX, minY, midZ), maxPoint, quadTree, depth + 1);

		/* With the child nodes created, we insert the children into the child nodes */
		for (ComponentTransform* transform : containedTransforms)
		{
			bool inserted = false;
			for (int i = 0; i < childrenCount && !inserted; ++i)
			{
				if (nodes[i]->Insert(transform))
					inserted = true;
			}
			assert(inserted);
		}

		/* With the contained Transforms redistributed in the children, we clear containedTransforms since they are now in child nodes */
		containedTransforms.clear();
	}
}
