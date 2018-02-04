#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "globals.h"


ComponentTransform::ComponentTransform(GameObject* owner) : Component(owner)
{
	type = ComponentType::TRANSFORM;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	position = float3(0.0f, 0.0f, 0.0f);
	scale = float3(1.0f, 1.0f, 1.0f);
	rotation = Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f);
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentTransform::~ComponentTransform()
{
	LOGGER("Deleting Component of type '%s'", GetString(type));
}

float3 ComponentTransform::GetPosition()
{
	return position;
}

float3 ComponentTransform::GetScale()
{
	return scale;
}

float3 ComponentTransform::GetRotation()
{
	return rotation.ToEulerXYZ();
	
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void ComponentTransform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void ComponentTransform::SetRotationRad(float x, float y, float z)
{
	rotation = Quat::FromEulerXYZ(x, y, z);
}

void ComponentTransform::SetRotationDeg(float x, float y, float z)
{
	SetRotationRad(DegToRad(x), DegToRad(y), DegToRad(z));
}

float* ComponentTransform::GetModelMatrix()
{
	return GetModelMatrix4x4().ptr();
}

void ComponentTransform::SetParent(ComponentTransform* newParent)
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

	/* Transpose to get the matrix in a shape when the bottom line is 0 0 0 1 */
	newLocalModelMatrix.Transpose();
	
	/* Extract the Translation, Scale and Rotation */
	/*
	Matrix shape
	
	a  b  c  d
	e  f  g  h
	i  j  k  l
	0  0  0  1
	*/

	/* Extract translation */
	/* x from d, y from h, z from l*/
	position.x = newLocalModelMatrix[0][3];
	position.y = newLocalModelMatrix[1][3];
	position.z = newLocalModelMatrix[2][3];
	/* Zero out extracted positions */
	newLocalModelMatrix[0][3] = 0;
	newLocalModelMatrix[1][3] = 0;
	newLocalModelMatrix[2][3] = 0;

	/* Extract scale */
	/* x from aei.Length, y from bfj.Length, z from cgh.Length */
	scale.x = (float3(newLocalModelMatrix[0][0], newLocalModelMatrix[1][0], newLocalModelMatrix[2][0])).Length();
	scale.y = (float3(newLocalModelMatrix[0][1], newLocalModelMatrix[1][1], newLocalModelMatrix[2][1])).Length();
	scale.z = (float3(newLocalModelMatrix[0][2], newLocalModelMatrix[1][2], newLocalModelMatrix[2][2])).Length();

	/* Extract rotation */
	/* Divide each column by the corresponding scale value */
	newLocalModelMatrix[0][0] /= scale.x;
	newLocalModelMatrix[1][0] /= scale.x;
	newLocalModelMatrix[2][0] /= scale.x;
	newLocalModelMatrix[0][1] /= scale.y;
	newLocalModelMatrix[1][1] /= scale.y;
	newLocalModelMatrix[2][1] /= scale.y;
	newLocalModelMatrix[0][2] /= scale.z;
	newLocalModelMatrix[1][2] /= scale.z;
	newLocalModelMatrix[2][2] /= scale.z;
	/* Then turn into a Quat */
	rotation = Quat(newLocalModelMatrix);
}

void ComponentTransform::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		float positionFP[3] = { position.x, position.y, position.z };
		float rotationFP[3] = { RadToDeg(GetRotation()[0]), RadToDeg(GetRotation()[1]), RadToDeg(GetRotation()[2]) };
		float scaleFP[3] = { scale.x, scale.y, scale.z };
		

		ImGui::Text("Transfomr GUI goes here");
		if (ImGui::InputFloat3("Position", positionFP))
			SetPosition(positionFP[0], positionFP[1], positionFP[2]);
		if (ImGui::InputFloat3("Rotation", rotationFP))
			SetRotationDeg(rotationFP[0], rotationFP[1], rotationFP[2]);
		if (ImGui::InputFloat3("Scale", scaleFP))
			if (scaleFP[0] >= 0 && scaleFP[1] >= 0 && scaleFP[2] >0)
				SetScale(scaleFP[0], scaleFP[1], scaleFP[2]);
	}
}

float4x4& ComponentTransform::GetModelMatrix4x4()
{
	UpdateLocalModelMattix();

	worldModelMatrix = localModelMatrix;

	GameObject* parent = gameObject->GetParent();
	while (parent)
	{
		std::vector<Component*> parentTransforms = parent->GetComponents(ComponentType::TRANSFORM);
		if (parentTransforms.size() != 0)
		{
			ComponentTransform* parentTransform = (ComponentTransform*)parentTransforms[0];
			worldModelMatrix = worldModelMatrix * parentTransform->GetModelMatrix4x4();
		}

		parent = parent->GetParent();
	}
	
	return worldModelMatrix;
}

float4x4& ComponentTransform::UpdateLocalModelMattix()
{
	float4x4 scaleMatrix = float4x4::Scale(scale.x, scale.y, scale.z);
	float4x4 rotationMatrix = float4x4::FromQuat(rotation);
	float4x4 translationMatrix = float4x4::Translate(position.x, position.y, position.z);
	memcpy_s(localModelMatrix.ptr(), sizeof(float) * 16, (translationMatrix * rotationMatrix * scaleMatrix).Transposed().ptr(), sizeof(float) * 16);
	return localModelMatrix;
}
