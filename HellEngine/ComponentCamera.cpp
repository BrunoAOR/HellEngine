#include <algorithm>
#include "ImGui/imgui.h"
#include "MathGeoLib/src/Geometry/Plane.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "globals.h"
#include "openGL.h"

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
	farClippingPlane = 1000.0f;
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 1.0f;
	verticalFOVRad = DegToRad(60);
	
	frustum.SetPerspective(GetHorizontalFOVrad(), verticalFOVRad);
	frustum.SetViewPlaneDistances(nearClippingPlane, farClippingPlane);
	float3 position = vec(0, 3, 0);

	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform != nullptr) {
		position = transform->GetPosition();
	}
	frustum.SetFrame(position, vec(0, 0, 1), vec(0, 1, 0));
	frustumCulling = true;

	return true;
}

void ComponentCamera::Update()
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	
	if (transform != nullptr) {
		float3 position = transform->GetPosition();
		SetPosition(position.x, position.y, position.z);
		
		float3 rot3 = transform->GetRotationRad();
		Quat rot = Quat::FromEulerXYZ(rot3.x, rot3.y, rot3.z);
		float3 newFront = rot.Transform(vec(0, 0, 1));
		float3 newUp = rot.Transform(vec(0, 1, 0));
		SetFront(newFront.x, newFront.y, newFront.z);
		SetUp(newUp.x, newUp.y, newUp.z);
		
		if (DEBUG_MODE)
			DrawFrustum();

	}

	App->scene->QuadTreeFrustumCulling(insideFrustum, frustum);
}

const float * ComponentCamera::GetPosition() const
{
	return frustum.Pos().ptr();
}

const float3 ComponentCamera::GetPosition3() const
{
	return frustum.Pos();
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

void ComponentCamera::SetAspectRatio(float ratio)
{
	aspectRatio = ratio;
	SetPerspective(GetHorizontalFOVrad(), verticalFOVRad);
}

void ComponentCamera::SetPerspective(float fovh, float fovv)
{
	frustum.SetPerspective(fovh, fovv);
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

const float3 ComponentCamera::GetFront3() const {
	return frustum.Front();
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

const float3 ComponentCamera::GetUp3() const
{
	return frustum.Up();
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

const float3 ComponentCamera::GetRight3() const
{
	return frustum.WorldRight();
}

const Color ComponentCamera::GetBackground() const
{
	return background;
}

void ComponentCamera::SetBackground(float red, float green, float blue)
{
	background.r = red;
	background.g = green;
	background.b = blue;
}

const Frustum & ComponentCamera::GetFrustum() const
{
	return frustum;
}

bool ComponentCamera::FrustumCulling()
{
	return frustumCulling;
}

void ComponentCamera::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;
				
		if (ImGui::Checkbox("Active Camera", &isActiveCamera))
		{
			if (isActiveCamera)
				App->scene->SetActiveGameCamera(this);
			else
				App->scene->SetActiveGameCamera(nullptr);
		}
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

		float editorAspectRatio = GetAspectRatio();
		if (ImGui::SliderFloat("Aspect Ratio", &editorAspectRatio, 0.1f, 5.0f))
			SetAspectRatio(editorAspectRatio);

		static float backgroundColor[3] = { 0.0f, 0.0f, 0.0f };
		if (ImGui::ColorEdit3("Background Color", backgroundColor))
		{
			background.r = backgroundColor[0];
			background.g = backgroundColor[1];
			background.b = backgroundColor[2];
		}
	}
}

void ComponentCamera::SetAsActiveCamera()
{
	isActiveCamera = true;
	App->scene->SetActiveGameCamera(this);
}

int ComponentCamera::MaxCountInGameObject()
{
	return 1;
}

bool ComponentCamera::IsInsideFrustum(GameObject * go)
{
	return	std::find(insideFrustum.begin(), insideFrustum.end(), go) != insideFrustum.end();
}

float ComponentCamera::GetHorizontalFOVrad() const
{
	return 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
}

void ComponentCamera::DrawFrustum() const
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform != nullptr)
	{

		float xTan = tanf(GetHorizontalFOVrad() / 2);
		float nearXOffset = nearClippingPlane * xTan;
		float farXOffset = farClippingPlane * xTan;

		float yTan = tanf(verticalFOVRad / 2);
		float nearYOffset = nearClippingPlane * yTan;
		float farYOffset = farClippingPlane * yTan;

		float nearA[3] = { -nearXOffset, -nearYOffset, nearClippingPlane };
		float nearB[3] = { -nearXOffset, nearYOffset, nearClippingPlane };
		float nearC[3] = { nearXOffset, nearYOffset, nearClippingPlane };
		float nearD[3] = { nearXOffset, -nearYOffset, nearClippingPlane };
		float farA[3] = { -farXOffset, -farYOffset, farClippingPlane };
		float farB[3] = { -farXOffset, farYOffset, farClippingPlane };
		float farC[3] = { farXOffset, farYOffset, farClippingPlane };
		float farD[3] = { farXOffset, -farYOffset, farClippingPlane };

		float vertices[8 * 3] = { SP_ARR_3(nearA), SP_ARR_3(nearB), SP_ARR_3(nearC), SP_ARR_3(nearD), SP_ARR_3(farA), SP_ARR_3(farB), SP_ARR_3(farC), SP_ARR_3(farD) };
		GLubyte indices[12 * 2] = {
			0, 1,	1, 2,	2, 3,	3, 0,	/* Near plane */
			4, 5,	5, 6,	6, 7,	7, 4,	/*  Far plane */
			0, 4,	1, 5,	2, 6,	3, 7	/* Near to far links */
		};
		
		int currentMatrixMode = 0;
		glGetIntegerv(GL_MATRIX_MODE, &currentMatrixMode);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadMatrixf((transform->GetModelMatrix4x4() * App->editorCamera->camera->viewMatrix).ptr());

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glDrawElements(GL_LINES, 12 * 2, GL_UNSIGNED_BYTE, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		
		glPopMatrix();
		glMatrixMode(currentMatrixMode);
	}
}
