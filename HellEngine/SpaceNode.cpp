#include <assert.h>
#include "ComponentTransform.h"
#include "SpaceNode.h"
#include "openGL.h"

SpaceNode::SpaceNode(float3 minPoint, float3 maxPoint, unsigned int aBucketSize):  bucketSize(aBucketSize), childrenCount(4)
{
	assert(bucketSize > 0);
	aabb.SetNegativeInfinity();
	vec points[2] = { minPoint, maxPoint };
	aabb.Enclose(points, 2);
}


SpaceNode::~SpaceNode()
{
	for (int i = 0; i < childrenCount; ++i)
	{
		delete nodes[i];
		nodes[i] = nullptr;
	}
}

bool SpaceNode::Insert(ComponentTransform* transform)
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

void SpaceNode::DrawNode()
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

void SpaceNode::CheckBucketSize()
{
}
