#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "globals.h"


ComponentTransform::ComponentTransform(GameObject* owner) : Component(owner)
{
	type = ComponentType::TRANSFORM;
	position = float3(0.0f, 0.0f, 0.0f);
	scale = float3(1.0f, 1.0f, 1.0f);
	rotation = Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f);
	LOGGER("Component of type '%s'", GetEditorTitle(type));
}

ComponentTransform::~ComponentTransform()
{
	LOGGER("Deleting Component of type '%s'", GetEditorTitle(type));
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

void ComponentTransform::OnEditor()
{
	ImGui::Text("Transfomr GUI goes here");
}

float4x4& ComponentTransform::GetModelMatrix4x4()
{
	float4x4 scaleMatrix = float4x4::Scale(scale.x, scale.y, scale.z);
	float4x4 rotationMatrix = float4x4::FromQuat(rotation);
	float4x4 translationMatrix = float4x4::Translate(position.x, position.y, position.z);
	memcpy_s(localModelMatrix.ptr(), sizeof(float) * 16, (translationMatrix * rotationMatrix * scaleMatrix).Transposed().ptr(), sizeof(float) * 16);

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
