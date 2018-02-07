#include "ImGui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "globals.h"

ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner)
{
	type = ComponentType::CAMERA;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	Init();
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentCamera::~ComponentCamera()
{
	LOGGER("Deleting Component of type '%s'", GetString(type));
}

bool ComponentCamera::Init()
{
	aspectRatio = 1;
	verticalFOVRad = 1;
	nearClippingPlane = 0.1f;
	farClippingPlane = 100.0f;
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 1.0f;
	verticalFOVRad = DegToRad(60);
	frustum.SetViewPlaneDistances(nearClippingPlane, farClippingPlane);
	float3 position = vec(0, 0, 0);
	std::vector<Component*> transforms = gameObject->GetComponents(ComponentType::TRANSFORM);
	if (transforms.size() > 0) {
		ComponentTransform* transform = (ComponentTransform*)transforms[0];
		if (transform != nullptr) {
			position = transform->GetPosition();
		}
	}
	frustum.SetFrame(position, vec(0, 0, 1), vec(0, 1, 0));
	frustumCulling = true;

	return true;
}

void ComponentCamera::Update()
{
	std::vector<Component*> transforms = gameObject->GetComponents(ComponentType::TRANSFORM);
	if (transforms.size() > 0) {
		ComponentTransform* transform = (ComponentTransform*)transforms[0];
		if (transform != nullptr) {
			float3 position = transform->GetPosition();
			SetPosition(position.x, position.y, position.z);
		}
	}
}

const float * ComponentCamera::GetPosition()
{
	return frustum.Pos().ptr();
}

void ComponentCamera::SetPosition(float x, float y, float z)
{
	frustum.SetPos(vec(x, y, z));
}

bool ComponentCamera::SetFOV(float fov)
{
	if (fov <= 0 || fov > 180)
		return false;

	verticalFOVRad = DegToRad(fov);
	frustum.SetPerspective(GetHorizontalFOVrad(), verticalFOVRad);
	return true;
}

float ComponentCamera::GetHorizontalFOV() const
{
	return RadToDeg(GetHorizontalFOVrad());
}

float ComponentCamera::GetVerticalFOV() const
{
	return RadToDeg(verticalFOVRad);
}

float ComponentCamera::GetAspectRatio() const
{
	return aspectRatio;
}

float * ComponentCamera::GetViewMatrix()
{
	viewMatrix = frustum.ViewMatrix();
	viewMatrix.Transpose();
	return viewMatrix.v[0];
}

float * ComponentCamera::GetProjectionMatrix()
{
	projectionMatrix = frustum.ProjectionMatrix().Transposed();
	return projectionMatrix.v[0];
}

bool ComponentCamera::SetPlaneDistances(float near, float far)
{
	if (near < 0 || near > far)
		return false;

	frustum.SetViewPlaneDistances(near, far);
	return true;
}

float ComponentCamera::GetNearPlaneDistance() const
{
	return nearClippingPlane;
}

bool ComponentCamera::SetNearPlaneDistance(float near)
{
	if (near < 0 || near > farClippingPlane)
		return false;

	frustum.SetViewPlaneDistances(near, farClippingPlane);
	return true;
}

float ComponentCamera::GetFarPlaneDistance() const
{
	return farClippingPlane;
}

bool ComponentCamera::SetFarPlaneDistance(float far)
{
	if (nearClippingPlane < 0 || nearClippingPlane > far)
		return false;

	frustum.SetViewPlaneDistances(nearClippingPlane, far);
	return true;
}

const float * ComponentCamera::GetFront() const
{
	return frustum.Front().ptr();
}

void ComponentCamera::SetFront(float x, float y, float z)
{
	if (x == 0 && y == 0 && z == 0)
		return;

	vec front(x, y, z);
	front.Normalize();
	Quat rot = Quat::RotateFromTo(frustum.Front(), front);
	vec up = frustum.Up();
	rot.Transform(up);
	frustum.SetFront(front);
	frustum.SetUp(up);
}

const float * ComponentCamera::GetUp() const
{
	return frustum.Up().ptr();
}

void ComponentCamera::SetUp(float x, float y, float z)
{
	if (x == 0 && y == 0 && z == 0)
		return;

	vec up(x, y, z);
	up.Normalize();
	Quat rot = Quat::RotateFromTo(frustum.Up(), up);
	vec front = frustum.Front();
	rot.Transform(front);
	frustum.SetUp(up);
	frustum.SetFront(front);
}

void ComponentCamera::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		/* Decide if necessary to show in a component */

		/*
		const float* editorFront = GetFront();
		float front[3] = { editorFront[0], editorFront[1], editorFront[2] };
		ImGui::InputFloat3("Front", front, 2);

		const float* editorUp = GetUp();
		float up[3] = { editorUp[0], editorUp[1], editorUp[2] };
		ImGui::InputFloat3("Up", up, 3);

		const float* editorPos = GetPosition();
		float position[3] = { editorPos[0], editorPos[1], editorPos[2] };
		if (ImGui::InputFloat3("Position", position))
			SetPosition(position[0], position[1], position[2]);
		*/

		ImGui::Checkbox("Frustum Culling", &frustumCulling);
		if (ImGui::SliderFloat("Near Plane", &nearClippingPlane, 0.01f, 30.0f))
			SetNearPlaneDistance(nearClippingPlane);
		if (ImGui::SliderFloat("Far Plane", &farClippingPlane, 50.0f, 2000.0f))
			SetFarPlaneDistance(farClippingPlane);

		float fovh = GetHorizontalFOV();
		ImGui::SliderFloat("Fov H", &fovh, 0.1f, 180.0f);
		float fovv = GetVerticalFOV();
		if (ImGui::SliderFloat("Fov V", &fovv, 0.1f, 180.0f))
			SetFOV(fovv);
		aspectRatio = GetAspectRatio();
		ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 5.0f);

		static float backgroundColor[3] = { 0.0f, 0.0f, 0.0f };
		if (ImGui::ColorEdit3("Background Color", backgroundColor))
		{
			background.r = backgroundColor[0];
			background.g = backgroundColor[1];
			background.b = backgroundColor[2];
		}
	}
}

int ComponentCamera::MaxCountInGameObject()
{
	return 1;
}

float ComponentCamera::GetHorizontalFOVrad() const
{
	return 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
}