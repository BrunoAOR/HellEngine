#include <stack>
#include <assert.h>
#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleDebugDraw.h"
#include "SpaceQuadNode.h"
#include "SpaceQuadTree.h"
#include "openGL.h"

SpaceQuadNode::SpaceQuadNode(float3 minPoint, float3 maxPoint, SpaceQuadTree* aQuadTree, unsigned int aDepth)
	: quadTree(aQuadTree), depth(aDepth), childrenCount(4)
{
	assert(quadTree);
	aabb.SetNegativeInfinity();
	vec points[2] = { minPoint, maxPoint };
	aabb.Enclose(points, 2);
	CreateVAO();
}


SpaceQuadNode::~SpaceQuadNode()
{
	glDeleteVertexArrays(1, &quadVao.vao);
	quadVao.vao = 0;
	glDeleteBuffers(1, &quadVao.vbo);
	quadVao.vbo = 0;
	glDeleteBuffers(1, &quadVao.ebo);
	quadVao.ebo = 0;
	quadVao.vertices.clear();
	quadVao.indices.clear();
	quadVao.elementsCount = 0;

	for (int i = 0; i < childrenCount; ++i)
	{
		delete childNodes[i];
		childNodes[i] = nullptr;
	}
}

bool SpaceQuadNode::CanContain(ComponentTransform * transform)
{
	AABB goBoundingBox = transform->GetBoundingBox();
	return aabb.Intersects(goBoundingBox);
}

bool SpaceQuadNode::CanContain(AABB goAABB)
{
	return aabb.Intersects(goAABB);
}

bool SpaceQuadNode::Insert(ComponentTransform* transform)
{
	AABB goBoundingBox = transform->GetBoundingBox();

	std::stack<SpaceQuadNode*> stack;
	SpaceQuadNode* root = this;
	stack.push(root);

	while (!stack.empty()) {
		root = stack.top();
		stack.pop();

		if (root->aabb.Intersects(goBoundingBox))
		{
			if (root->isLeaf)
			{
				root->containedTransforms.push_back(transform);
				root->CheckBucketSize();
				return true;
			}
			else
			{
				for (int i = 0; i < root->childrenCount; i++)
				{
					if (root->childNodes[i]->CanContain(goBoundingBox))
						stack.push(root->childNodes[i]);
				}
				assert(stack.size() > 0);
				if (stack.size() > 1) {
					root->containedTransforms.push_back(transform);
					for (unsigned int i = 0; i < stack.size(); i++)
						stack.pop();
				}
			}
		}
	}

	return false;
}

bool SpaceQuadNode::Remove(ComponentTransform* transform)
{
	std::stack<SpaceQuadNode*> stack;

	SpaceQuadNode* root = this;

	do {
		while (root != nullptr) {
			for (unsigned int i = 4; i > 1; i--) {
				if (root->childNodes[i - 1] != nullptr)
					stack.push(root->childNodes[i - 1]);
			}

			stack.push(root);
			root = root->childNodes[0];
		}

		root = stack.top();
		stack.pop();

		bool rightChild = false;
		for (unsigned int i = 4; i > 1; i--) {
			if (root->childNodes[i - 1] == stack.top()) {
				SpaceQuadNode* aux = stack.top();
				stack.pop();
				stack.push(root);
				root = aux;
				rightChild = true;
			}
		}

		if (rightChild) {
			if (root->isLeaf)
			{
				for (std::vector<ComponentTransform*>::iterator it = root->containedTransforms.begin(); it != root->containedTransforms.end(); ++it)
				{
					if ((*it) == transform)
					{
						root->containedTransforms.erase(it);
						return true;
					}
				}
			}
		}

	} while (!stack.empty());

	/* Pending further test
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
			if (childNodes[i]->Remove(transform))
				return true;
		}
	} */

	return false;
}

void SpaceQuadNode::DrawNode()
{
	std::stack<SpaceQuadNode*> stack;

	SpaceQuadNode* root = this;

	stack.push(root);

	while (!stack.empty()) {
		root = stack.top();
		stack.pop();

		if (root->isLeaf) {
			if (root->quadVao.vao != 0)
			{
				float identity[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
				App->debugDraw->DrawElements(identity, root->quadVao.vao, root->quadVao.elementsCount);
			}
		}
		else {
			for (unsigned int i = 4; i > 0; i--)
				stack.push(root->childNodes[i - 1]);

		}
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
		childNodes[0] = new SpaceQuadNode(minPoint, float3(midX, maxY, midZ), quadTree, depth + 1);
		childNodes[1] = new SpaceQuadNode(float3(midX, minY, minZ), float3(maxX, maxY, midZ), quadTree, depth + 1);
		childNodes[2] = new SpaceQuadNode(float3(minX, minY, midZ), float3(midX, maxY, maxZ), quadTree, depth + 1);
		childNodes[3] = new SpaceQuadNode(float3(midX, minY, midZ), maxPoint, quadTree, depth + 1);

		/* With the child nodes created, we insert the children into the child nodes, whenever they are not shared.
		When they are shared, they will be returned to the containedTransforms and kept by this node */
		std::vector<ComponentTransform*> transformsToRedistribute(containedTransforms);
		containedTransforms.clear();

		for (ComponentTransform* transform : transformsToRedistribute)
		{
			std::vector<SpaceQuadNode*> candidates;
			for (int i = 0; i < childrenCount; ++i)
			{
				if (childNodes[i]->CanContain(transform))
					candidates.push_back(childNodes[i]);
			}
			assert(candidates.size() > 0);
			if (candidates.size() == 1)
			{
				candidates[0]->Insert(transform);
			}
			else
			{
				containedTransforms.push_back(transform);
			}
		}

		/* With the contained Transforms redistributed in the children,
		we only keep transforms that fall between two or more child nodes in containedTransforms */
	}
}

void SpaceQuadNode::CreateVAO()
{
	/*
	CUBE drawing
	  G-----H
	 /|    /|
	C-----D |
	| |   | |
	| E---|-F
	|/    |/
	A-----B

	ABDC is the front face
	FEGH is the back face
	*/
	GLfloat vA[3];
	GLfloat vB[3];
	GLfloat vC[3];
	GLfloat vD[3];
	GLfloat vE[3];
	GLfloat vF[3];
	GLfloat vG[3];
	GLfloat vH[3];

	GLfloat cYellow[3];

	int numCubeUniqueVertexes = 8;

	/* Cube vertices */
	{
		vA[0] = aabb.CornerPoint(1).x;
		vA[1] = aabb.CornerPoint(1).y;
		vA[2] = aabb.CornerPoint(1).z;

		vB[0] = aabb.CornerPoint(5).x;
		vB[1] = aabb.CornerPoint(5).y;
		vB[2] = aabb.CornerPoint(5).z;

		vC[0] = aabb.CornerPoint(7).x;
		vC[1] = aabb.CornerPoint(7).y;
		vC[2] = aabb.CornerPoint(7).z;

		vD[0] = aabb.CornerPoint(3).x;
		vD[1] = aabb.CornerPoint(3).y;
		vD[2] = aabb.CornerPoint(3).z;

		vE[0] = aabb.CornerPoint(0).x;
		vE[1] = aabb.CornerPoint(0).y;
		vE[2] = aabb.CornerPoint(0).z;

		vF[0] = aabb.CornerPoint(4).x;
		vF[1] = aabb.CornerPoint(4).y;
		vF[2] = aabb.CornerPoint(4).z;

		vG[0] = aabb.CornerPoint(6).x;
		vG[1] = aabb.CornerPoint(6).y;
		vG[2] = aabb.CornerPoint(6).z;

		vH[0] = aabb.CornerPoint(2).x;
		vH[1] = aabb.CornerPoint(2).y;
		vH[2] = aabb.CornerPoint(2).z;
	}

	{
		cYellow[0] = 1.0f;
		cYellow[1] = 1.0f;
		cYellow[2] = 0.0f;
	}

	const uint allVertCount = 24;
	const uint uniqueVertCount = 8;
	GLfloat uniqueVertices[allVertCount] = { SP_ARR_3(vA), SP_ARR_3(vB), SP_ARR_3(vC), SP_ARR_3(vD), SP_ARR_3(vE), SP_ARR_3(vF), SP_ARR_3(vG), SP_ARR_3(vH) };
	GLfloat uniqueColors[allVertCount] = { SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow), SP_ARR_3(cYellow) };
	GLuint vertIndexes[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	/* front face */
		4, 5,	5, 6,	6, 7,	7, 4,	/* back face */
		0, 4,	1, 5,	2, 6,	3, 7	/* front-back links */

	};
	GLuint cornerIndexes[] = { 0, 1, 2, 3, 4, 5, 6, 7 }; /* Will use later */

	float allUniqueData[uniqueVertCount * 6];

	for (int i = 0; i < uniqueVertCount * 6; ++i)
	{
		if (i % 6 == 0 || i % 6 == 1 || i % 6 == 2)
		{
			allUniqueData[i] = uniqueVertices[(i / 6) * 3 + (i % 6)];
		}
		else
		{
			allUniqueData[i] = uniqueColors[(i / 6) * 3 + ((i % 6) - 3)];
		}
	}

	quadVao.name = "QuadNode";
	quadVao.elementsCount = allVertCount;

	glGenVertexArrays(1, &quadVao.vao);
	glGenBuffers(1, &quadVao.vbo);
	glGenBuffers(1, &quadVao.ebo);

	glBindVertexArray(quadVao.vao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVao.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 6, allUniqueData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVao.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * quadVao.elementsCount, vertIndexes, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}
