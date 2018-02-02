#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
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

void ComponentTransform::SetRotation(float x, float y, float z)
{
	rotation = Quat::FromEulerXYZ(x, y, z);
}

float * ComponentTransform::GetModelMatrix()
{
	float4x4 scaleMatrix = float4x4::Scale(scale.x, scale.y, scale.z);
	float4x4 rotationMatrix = float4x4::FromQuat(rotation);
	float4x4 translationMatrix = float4x4::Translate(position.x, position.y, position.z);
	memcpy_s(modelMatrix, sizeof(float) * 16, (translationMatrix * rotationMatrix * scaleMatrix).Transposed().ptr(), sizeof(float) * 16);
	return modelMatrix;
}

void ComponentTransform::OnEditor()
{
	ImGui::Text("Transfomr GUI goes here");
}
