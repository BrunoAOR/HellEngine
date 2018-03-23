#include <stack>
#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "Application.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModelInfo.h"
#include "ModuleDebugDraw.h"
#include "ModuleScene.h"
#include "SerializableObject.h"
#include "globals.h"
#include "openGL.h"

MeshInfo ComponentTransform::baseBoundingBoxMeshInfo;
std::vector<float3> ComponentTransform::baseBoundingBoxVertices;

ComponentTransform::ComponentTransform(GameObject* owner) : Component(owner)
{
	type = ComponentType::TRANSFORM;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	position = float3(0.0f, 0.0f, 0.0f);
	scale = float3(1.0f, 1.0f, 1.0f);
	rotation = Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f);
	
	if (baseBoundingBoxVertices.size() == 0)
	{
		InitializeBaseBB();
		CreateBBVAO();
	}

	UpdateBoundingBox();
	UpdateMatrices();
	//LOGGER("Component of type '%s'", GetString(type));
}

ComponentTransform::~ComponentTransform()
{
	App->scene->ChangeStaticStatus(this, false);
	//LOGGER("Deleting Component of type '%s'", GetString(type));
}

void ComponentTransform::Update()
{
	if (drawBoundingBox) {
		if (boundingBox.IsFinite())
		{
			GLfloat vA[3];
			GLfloat vB[3];
			GLfloat vC[3];
			GLfloat vD[3];
			GLfloat vE[3];
			GLfloat vF[3];
			GLfloat vG[3];
			GLfloat vH[3];

			/* Cube vertices */
			{
				vA[0] = boundingBox.CornerPoint(0).x;
				vA[1] = boundingBox.CornerPoint(0).y;
				vA[2] = boundingBox.CornerPoint(0).z;

				vB[0] = boundingBox.CornerPoint(4).x;
				vB[1] = boundingBox.CornerPoint(4).y;
				vB[2] = boundingBox.CornerPoint(4).z;

				vC[0] = boundingBox.CornerPoint(2).x;
				vC[1] = boundingBox.CornerPoint(2).y;
				vC[2] = boundingBox.CornerPoint(2).z;

				vD[0] = boundingBox.CornerPoint(6).x;
				vD[1] = boundingBox.CornerPoint(6).y;
				vD[2] = boundingBox.CornerPoint(6).z;

				vE[0] = boundingBox.CornerPoint(1).x;
				vE[1] = boundingBox.CornerPoint(1).y;
				vE[2] = boundingBox.CornerPoint(1).z;

				vF[0] = boundingBox.CornerPoint(5).x;
				vF[1] = boundingBox.CornerPoint(5).y;
				vF[2] = boundingBox.CornerPoint(5).z;

				vG[0] = boundingBox.CornerPoint(3).x;
				vG[1] = boundingBox.CornerPoint(3).y;
				vG[2] = boundingBox.CornerPoint(3).z;

				vH[0] = boundingBox.CornerPoint(7).x;
				vH[1] = boundingBox.CornerPoint(7).y;
				vH[2] = boundingBox.CornerPoint(7).z;
			}

			GLfloat cGreen[3];

			{
				cGreen[0] = 0.0f;
				cGreen[1] = 1.0f;
				cGreen[2] = 0.0f;
			}

			GLfloat uniqueVertices[24] = { SP_ARR_3(vA), SP_ARR_3(vB), SP_ARR_3(vC), SP_ARR_3(vD), SP_ARR_3(vE), SP_ARR_3(vF), SP_ARR_3(vG), SP_ARR_3(vH) };
			GLfloat uniqueColors[24] = { SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen) };

			glBindVertexArray(baseBoundingBoxMeshInfo.vao);
			glBindBuffer(GL_ARRAY_BUFFER, baseBoundingBoxMeshInfo.vbo);
			float* oldBBData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			for (int i = 0; i < 8 * 6; ++i)
			{
				if (i % 6 == 0 || i % 6 == 1 || i % 6 == 2)
					boundingBoxUniqueData[i] = uniqueVertices[(i / 6) * 3 + (i % 6)];
				else
					boundingBoxUniqueData[i] = uniqueColors[(i / 6) * 3 + ((i % 6) - 3)];

				oldBBData[i] = boundingBoxUniqueData[i];
			}

			glBindVertexArray(baseBoundingBoxMeshInfo.vao);
			glBindBuffer(GL_ARRAY_BUFFER, baseBoundingBoxMeshInfo.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 6, boundingBoxUniqueData, GL_DYNAMIC_DRAW);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindVertexArray(GL_NONE);
			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

			if (baseBoundingBoxMeshInfo.vao != 0) {
				float identity[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
				App->debugDraw->DrawElements(identity, baseBoundingBoxMeshInfo.vao, baseBoundingBoxMeshInfo.elementsCount);
			}
		}
	}
}

bool ComponentTransform::Equals(ComponentTransform * t)
{
	return position.Equals(t->position) && scale.Equals(t->scale) && rotation.Equals(t->rotation);
}

bool ComponentTransform::GetIsStatic() const
{
	return isStatic;
}

void ComponentTransform::SetIsStatic(bool isStatic)
{
	std::stack<GameObject*> stack;
	stack.push(gameObject);

	GameObject* go;
	while (!stack.empty())
	{
		go = stack.top();
		stack.pop();

		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		if (transform)
		{
			transform->isStatic = isStatic;
			App->scene->ChangeStaticStatus(transform, isStatic);
		}

		/* If setting a transform to static, all the parent hierarchy is also made static*/
		if (isStatic)
		{
			GameObject* parent = go->GetParent();
			if (parent)
				stack.push(parent);
		}
		/* If setting a transform to NON-static, all the child hierarchy is also made NON-static*/
		else
		{
			for (GameObject* child : go->GetChildren())
				stack.push(child);
		}
	}
}

float3 ComponentTransform::GetPosition() const
{
	return position;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

float3 ComponentTransform::GetRotationRad() const
{
	return rotation.ToEulerXYZ();
}

float3 ComponentTransform::GetRotationDeg() const
{
	float3 rotationRad = GetRotationRad();

	return float3(RadToDeg(rotationRad.x), RadToDeg(rotationRad.y), RadToDeg(rotationRad.z));
}

Quat ComponentTransform::GetRotationQuat() const
{
	return rotation;
}

AABB ComponentTransform::GetBoundingBox() const
{
	return boundingBox;
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	if (!isStatic)
	{
		position.x = x;
		position.y = y;
		position.z = z;
		UpdateMatrices();
	}
}

void ComponentTransform::SetScale(float x, float y, float z)
{
	if (!isStatic)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
		UpdateMatrices();
	}
}

void ComponentTransform::SetRotation(Quat newRotation)
{
	if (!isStatic)
	{
		rotation = newRotation;
		UpdateMatrices();
	}
}

void ComponentTransform::SetRotationRad(float x, float y, float z)
{
	if (!isStatic)
	{
		rotation = Quat::FromEulerXYZ(x, y, z);
		UpdateMatrices();
	}
}

void ComponentTransform::SetRotationDeg(float x, float y, float z)
{
	if (!isStatic)
	{
		SetRotationRad(DegToRad(x), DegToRad(y), DegToRad(z));
	}
}

void ComponentTransform::UpdateBoundingBox(ComponentMesh* mesh)
{
	if (mesh)
		EncloseBoundingBox(mesh);

	boundingBox = baseBoundingBox;
	boundingBox.TransformBB(GetModelMatrix4x4().Transposed());
}


void ComponentTransform::EncloseBoundingBox(ComponentMesh* mesh)
{
	if (mesh->GetActiveModelInfo() != nullptr) {
		uint size = mesh->GetActiveModelInfo()->meshInfosIndexes.size();

		if (size > 0) {
			baseBoundingBox.SetNegativeInfinity();
			float3 minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			float3 maxPoint(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
			for (uint i = 0; i < size; i++) {
				std::vector<float3> vertices = App->scene->meshes.at(mesh->GetActiveModelInfo()->meshInfosIndexes.at(i))->vertices;

				for (uint j = 0; j < vertices.size(); j++) {
					minPoint.ModifyToMin(vertices.at(j));
					maxPoint.ModifyToMax(vertices.at(j));
				}
			}
			baseBoundingBox.Enclose(minPoint, maxPoint);
		}
	}
}

void ComponentTransform::RecalculateLocalMatrix(ComponentTransform* newParent)
{
	/* Calculate the new local model matrix */
	float4x4 newParentWorldMatrix;
	if (newParent != nullptr)
		newParentWorldMatrix = newParent->GetModelMatrix4x4();
	else
		newParentWorldMatrix = float4x4::identity;

	float4x4 inverseNewParentWorldMatrix = newParentWorldMatrix;
	inverseNewParentWorldMatrix.Inverse();

	float4x4 currentWorldMatrix = GetModelMatrix4x4();
	float4x4 newLocalModelMatrix = currentWorldMatrix * inverseNewParentWorldMatrix;

	DecomposeMatrix(newLocalModelMatrix, position, rotation, scale);
	float3 rotEuler = rotation.ToEulerXYZ();
	rotationDeg[0] = RadToDeg(rotEuler.x);
	rotationDeg[1] = RadToDeg(rotEuler.y);
	rotationDeg[2] = RadToDeg(rotEuler.z);

	UpdateMatrices(&newParentWorldMatrix);
	//UpdateBoundingBox();
}

void ComponentTransform::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		float positionFP[3] = { position.x, position.y, position.z };
		float scaleFP[3] = { scale.x, scale.y, scale.z };

		if (ImGui::DragFloat3("Position", positionFP, 0.03f))
		{
			if (!isStatic)
			{
				SetPosition(positionFP[0], positionFP[1], positionFP[2]);
				UpdateBoundingBox();
			}
		}
		if (ImGui::DragFloat3("Rotation", rotationDeg, 0.3f))
		{
			if (!isStatic)
			{
				SetRotationDeg(rotationDeg[0], rotationDeg[1], rotationDeg[2]);
				UpdateBoundingBox();
			}
		}
		if (ImGui::DragFloat3("Scale", scaleFP, 0.1f, 0.1f, 1000.0f))
		{
			if (!isStatic)
			{
				if (scaleFP[0] >= 0 && scaleFP[1] >= 0 && scaleFP[2] > 0)
				{
					SetScale(scaleFP[0], scaleFP[1], scaleFP[2]);
					UpdateBoundingBox();
				}
			}
		}

		if (DEBUG_MODE)
			ImGui::Checkbox("Draw Bounding Box", &drawBoundingBox);
		else if (drawBoundingBox)
			drawBoundingBox = false;

		if (ImGui::Checkbox("Static", &isStatic))
		{
			SetIsStatic(isStatic);
		}
	}
}

int ComponentTransform::MaxCountInGameObject()
{
	return 1;
}

const float4x4& ComponentTransform::GetModelMatrix4x4() const
{
	return worldModelMatrix;
}

const float* ComponentTransform::GetModelMatrix() const
{
	return worldModelMatrix.ptr();
}

void ComponentTransform::UpdateMatrices(const float4x4* newParentWorldMatrix)
{
	UpdateLocalModelMatrix();
	UpdateWorldModelMatrix(newParentWorldMatrix);
}

void ComponentTransform::UpdateLocalModelMatrix()
{
	float4x4 scaleMatrix = float4x4::Scale(scale.x, scale.y, scale.z);
	float4x4 rotationMatrix = float4x4::QuatToRotation(rotation);
	float4x4 translationMatrix = float4x4::TranslationToRotation(position.x, position.y, position.z);
	translationMatrix.LeftMultiply(rotationMatrix);
	translationMatrix.LeftMultiply(scaleMatrix);
	memcpy_s(localModelMatrix.ptr(), sizeof(float) * 16, translationMatrix.Transposed().ptr(), sizeof(float) * 16);	
	
	UpdateWorldModelMatrix();
}

void ComponentTransform::UpdateWorldModelMatrix(const float4x4* newParentWorldMatrix)
{
	std::stack<ComponentTransform*> stack;
	
	ComponentTransform* transform = this;
	stack.push(transform);

	std::vector<GameObject*> children;

	while (!stack.empty()) 
	{
		transform = stack.top();
		stack.pop();
				 
		transform->worldModelMatrix = transform->localModelMatrix;

		/* Calculate world Matrix */
		if (newParentWorldMatrix == nullptr)
		{
			GameObject* parent = transform->gameObject->GetParent();
			if (parent)
			{
				ComponentTransform* parentTransform = (ComponentTransform*)parent->GetComponent(ComponentType::TRANSFORM);
				if (parentTransform)
					transform->worldModelMatrix.LeftMultiply(parentTransform->GetModelMatrix4x4());
			}
		}
		else
		{
			transform->worldModelMatrix.LeftMultiply(*newParentWorldMatrix);
			newParentWorldMatrix = nullptr;
		}

		transform->UpdateBoundingBox();

		/* Inform children of the change so they can update their worldMatrix */

		children = transform->gameObject->GetChildren();
		for (GameObject* child : children)
		{
			ComponentTransform* childTransform = (ComponentTransform*)child->GetComponent(ComponentType::TRANSFORM);
			if (childTransform)
				stack.push(childTransform);

		}
	}

	
}

void ComponentTransform::InitializeBaseBB()
{
	const float s = 0.5f;

	baseBoundingBoxVertices.push_back(float3(-s, -s, s));
	baseBoundingBoxVertices.push_back(float3(s, -s, s));
	baseBoundingBoxVertices.push_back(float3(-s, s, s));
	baseBoundingBoxVertices.push_back(float3(s, s, s));
	baseBoundingBoxVertices.push_back(float3(-s, -s, -s));
	baseBoundingBoxVertices.push_back(float3(s, -s, -s));
	baseBoundingBoxVertices.push_back(float3(-s, s, -s));
	baseBoundingBoxVertices.push_back(float3(s, s, -s));
}

void ComponentTransform::CreateBBVAO()
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

	GLfloat cGreen[3];

	int numCubeUniqueVertexes = 8;

	/* Cube vertices */
	{
		vA[0] = baseBoundingBoxVertices.at(0).x;
		vA[1] = baseBoundingBoxVertices.at(0).y;
		vA[2] = baseBoundingBoxVertices.at(0).z;

		vB[0] = baseBoundingBoxVertices.at(1).x;
		vB[1] = baseBoundingBoxVertices.at(1).y;
		vB[2] = baseBoundingBoxVertices.at(1).z;

		vC[0] = baseBoundingBoxVertices.at(2).x;
		vC[1] = baseBoundingBoxVertices.at(2).y;
		vC[2] = baseBoundingBoxVertices.at(2).z;

		vD[0] = baseBoundingBoxVertices.at(3).x;
		vD[1] = baseBoundingBoxVertices.at(3).y;
		vD[2] = baseBoundingBoxVertices.at(3).z;

		vE[0] = baseBoundingBoxVertices.at(4).x;
		vE[1] = baseBoundingBoxVertices.at(4).y;
		vE[2] = baseBoundingBoxVertices.at(4).z;

		vF[0] = baseBoundingBoxVertices.at(5).x;
		vF[1] = baseBoundingBoxVertices.at(5).y;
		vF[2] = baseBoundingBoxVertices.at(5).z;

		vG[0] = baseBoundingBoxVertices.at(6).x;
		vG[1] = baseBoundingBoxVertices.at(6).y;
		vG[2] = baseBoundingBoxVertices.at(6).z;

		vH[0] = baseBoundingBoxVertices.at(7).x;
		vH[1] = baseBoundingBoxVertices.at(7).y;
		vH[2] = baseBoundingBoxVertices.at(7).z;
	}

	{
		cGreen[0] = 0.0f;
		cGreen[1] = 1.0f;
		cGreen[2] = 0.0f;
	}

	const uint allVertCount = 24;
	const uint uniqueVertCount = 8;
	GLfloat uniqueVertices[allVertCount] = { SP_ARR_3(vA), SP_ARR_3(vB), SP_ARR_3(vC), SP_ARR_3(vD), SP_ARR_3(vE), SP_ARR_3(vF), SP_ARR_3(vG), SP_ARR_3(vH) };
	GLfloat uniqueColors[allVertCount] = { SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen), SP_ARR_3(cGreen) };
	GLuint vertIndexes[] = {
		0, 1,	0, 2,	2, 3,	3, 1,	/* front face */
		4, 5,	4, 6,	6, 7,	7, 5,	/* back face */
		0, 4,	1, 5,	2, 6,	3, 7	/* front-back links */

	};
	GLuint cornerIndexes[] = { 0, 1, 2, 3, 4, 5, 6, 7 }; /* Will use later */

	for (int i = 0; i < uniqueVertCount * 6; ++i)
	{
		if (i % 6 == 0 || i % 6 == 1 || i % 6 == 2)
		{
			boundingBoxUniqueData[i] = uniqueVertices[(i / 6) * 3 + (i % 6)];
		}
		else
		{
			boundingBoxUniqueData[i] = uniqueColors[(i / 6) * 3 + ((i % 6) - 3)];
		}
	}

	baseBoundingBoxMeshInfo.name = "BaseBoundingBox";
	baseBoundingBoxMeshInfo.elementsCount = allVertCount;

	glGenVertexArrays(1, &baseBoundingBoxMeshInfo.vao);
	glGenBuffers(1, &baseBoundingBoxMeshInfo.vbo);
	glGenBuffers(1, &baseBoundingBoxMeshInfo.ebo);

	glBindVertexArray(baseBoundingBoxMeshInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, baseBoundingBoxMeshInfo.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 6, boundingBoxUniqueData, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baseBoundingBoxMeshInfo.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * baseBoundingBoxMeshInfo.elementsCount, vertIndexes, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void ComponentTransform::ApplyWorldTransformationMatrix(const float4x4& worldTransformationMatrix, bool excludePosition, bool excludeRotation, bool excludeScale)
{
	static float3 pos;
	static Quat rot;
	static float3 sca;

	if (!isStatic)
	{
		float4x4 inverseNewParentWorldMatrix;
		GameObject* parent = gameObject->GetParent();
		if (parent)
		{
			ComponentTransform* transform = (ComponentTransform*)gameObject->GetParent()->GetComponent(ComponentType::TRANSFORM);
			if (transform)
				inverseNewParentWorldMatrix = transform->GetModelMatrix4x4();
		}
		else
			inverseNewParentWorldMatrix = float4x4::identity;

		inverseNewParentWorldMatrix.Inverse();

		float4x4 newLocalModelMatrix = worldTransformationMatrix * inverseNewParentWorldMatrix;

		float3& outputPosition = excludePosition ? pos : position;
		Quat& outputRotation = excludeRotation ? rot : rotation;
		float3& outputScale = excludeScale ? sca : scale;

		DecomposeMatrix(newLocalModelMatrix, outputPosition, outputRotation, outputScale);
		float3 rotEuler = rotation.ToEulerXYZ();
		rotationDeg[0] = RadToDeg(rotEuler.x);
		rotationDeg[1] = RadToDeg(rotEuler.y);
		rotationDeg[2] = RadToDeg(rotEuler.z);

		UpdateBoundingBox();
		UpdateMatrices();
	}
}

void ComponentTransform::Save(SerializableObject& obj) const
{
	Component::Save(obj);
	float3 worldPos;
	float3 worldScale;
	Quat worldRot;
	DecomposeMatrix(GetModelMatrix4x4(), worldPos, worldRot, worldScale);

	obj.AddFloat3("Position", worldPos);
	obj.AddFloat3("Scale", worldScale);
	obj.AddFloat4("Rotation", float4(worldRot.x, worldRot.y, worldRot.z, worldRot.w));
}

void ComponentTransform::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	position = obj.GetFloat3("Position");
	scale = obj.GetFloat3("Scale");
	float4 rot = obj.GetFloat4("Rotation");
	rotation = Quat(rot.x, rot.y, rot.z, rot.w);
	UpdateLocalModelMatrix();
}
