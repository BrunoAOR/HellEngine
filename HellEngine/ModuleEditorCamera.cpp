#include <math.h>
#include "MathGeoLib/src/Math/Quat.h"
#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"


ModuleEditorCamera::ModuleEditorCamera()
{}

ModuleEditorCamera::~ModuleEditorCamera()
{}

bool ModuleEditorCamera::Init()
{
	moveSpeed = 1;
	rotationSpeed = 90;
	zoomSpeed = 1;

	aspectRatio = 1;
	verticalFOVRad = 1;
	nearClippingPlane = 0.1f;
	farClippingPlane = 100.0f;
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 1.0f;
	verticalFOVRad = DegToRad(60);
	onWindowResize();
	frustum.SetViewPlaneDistances(nearClippingPlane, farClippingPlane);
	frustum.SetFrame(vec(0, 0, 3), vec(0, 0, -1), vec(0, 1, 0));
	
	return true;
}

UpdateStatus ModuleEditorCamera::Update()
{
	handleCameraMotion();
	handleCameraRotation();	

	return UpdateStatus::UPDATE_CONTINUE;
}

const float* ModuleEditorCamera::getPosition()
{
	return frustum.Pos().ptr();
}

/* Sets the position of the camera */
void ModuleEditorCamera::SetPosition(float x, float y, float z)
{
	frustum.SetPos(vec(x, y, z));
}

/* Sets the vertical FOV of the camera and adjust the horizontal FOV accordingly */
bool ModuleEditorCamera::SetFOV(float fovDeg)
{
	if (fovDeg <= 0 || fovDeg > 180)
		return false;

	verticalFOVRad = DegToRad(fovDeg);
	frustum.SetPerspective(getHorizontalFOVrad(), verticalFOVRad);
	return true;
}

float ModuleEditorCamera::getHorizontalFOV() const
{
	return RadToDeg(getHorizontalFOVrad());
}

float ModuleEditorCamera::getVerticalFOV() const
{
	return RadToDeg(verticalFOVRad);
}

float ModuleEditorCamera::getAspectRatio() const
{
	return aspectRatio;
}

/* Returns a float* to the first of 16 floats representing the view matrix */
float* ModuleEditorCamera::GetViewMatrix() const
{
	float4x4 viewMatrix = frustum.ViewMatrix();
	return viewMatrix.Transposed().v[0];
}

/* Returns a float* to the first of 16 floats representing the projection matrix */
float* ModuleEditorCamera::GetProjectionMatrix() const
{
	return frustum.ProjectionMatrix().Transposed().v[0];
}

/* Sets the distance for the near and far clipping planes */
bool ModuleEditorCamera::SetPlaneDistances(float near, float far)
{
	if (near < 0 || near > far)
		return false;
	
	frustum.SetViewPlaneDistances(near, far);
	return true;
}

float ModuleEditorCamera::getNearPlaneDistance() const
{
	return nearClippingPlane;
}

bool ModuleEditorCamera::SetNearPlaneDistance(float near)
{
	if (near < 0 || near > farClippingPlane)
		return false;

	frustum.SetViewPlaneDistances(near, farClippingPlane);
	return true;
}

float ModuleEditorCamera::getFarPlaneDistance() const
{
	return farClippingPlane;
}

bool ModuleEditorCamera::SetFarPlaneDistance(float far)
{
	if (nearClippingPlane < 0 || nearClippingPlane > far)
		return false;

	frustum.SetViewPlaneDistances(nearClippingPlane, far);
	return true;
}

/* Method to be called when the window is resized */
void ModuleEditorCamera::onWindowResize()
{
	int width = App->window->getWidth();
	int height = App->window->getHeight();

	aspectRatio = (float)width / height;
	frustum.SetPerspective(getHorizontalFOVrad(), verticalFOVRad);
}

const float* ModuleEditorCamera::GetFront() const
{
	return frustum.Front().ptr();
}

void ModuleEditorCamera::SetFront(float x, float y, float z)
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

const float* ModuleEditorCamera::GetUp() const
{
	return frustum.Up().ptr();
}

void ModuleEditorCamera::SetUp(float x, float y, float z)
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

void ModuleEditorCamera::handleCameraMotion()
{
	vec pos = frustum.Pos();
	/* Camera up */
	if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
		pos.y += moveSpeed * App->time->DeltaTime();
	/* Camera down */
	if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
		pos.y -= moveSpeed * App->time->DeltaTime();

	/* Camera forward */
	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
	{
		pos.x += frustum.Front().x * zoomSpeed * App->time->DeltaTime();
		pos.y += frustum.Front().y * zoomSpeed * App->time->DeltaTime();
		pos.z += frustum.Front().z * zoomSpeed * App->time->DeltaTime();
	}
	/* Camera backwards */
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
	{
		pos.x -= frustum.Front().x * zoomSpeed * App->time->DeltaTime();
		pos.y -= frustum.Front().y * zoomSpeed * App->time->DeltaTime();
		pos.z -= frustum.Front().z * zoomSpeed * App->time->DeltaTime();
	}

	/* Camera left */
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
	{
		pos.x -= frustum.WorldRight().x * moveSpeed * App->time->DeltaTime();
		pos.y -= frustum.WorldRight().y * moveSpeed * App->time->DeltaTime();
		pos.z -= frustum.WorldRight().z * moveSpeed * App->time->DeltaTime();
	}
	/* Camera right */
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
	{
		pos.x += frustum.WorldRight().x * moveSpeed * App->time->DeltaTime();
		pos.y += frustum.WorldRight().y * moveSpeed * App->time->DeltaTime();
		pos.z += frustum.WorldRight().z * moveSpeed * App->time->DeltaTime();
	}

	frustum.SetPos(pos);
}

void ModuleEditorCamera::handleCameraRotation()
{
	/* Camera rotate upwards */
	if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), DegToRad(rotationSpeed) * App->time->DeltaTime());
		frustum.SetFront(rotation.Transform(frustum.Front()));
		frustum.SetUp(rotation.Transform(frustum.Up()));
	}
	/* Camera rotate downwards */
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), -DegToRad(rotationSpeed) * App->time->DeltaTime());
		frustum.SetFront(rotation.Transform(frustum.Front()));
		frustum.SetUp(rotation.Transform(frustum.Up()));
	}

	/* Camera rotate leftwards */
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
	{
		Quat rotation = Quat::FromEulerXYZ(0, DegToRad(rotationSpeed) * App->time->DeltaTime(), 0);
		frustum.SetFront(rotation.Transform(frustum.Front()));
		frustum.SetUp(rotation.Transform(frustum.Up()));
	}
	/* Camera rotate rightwards */
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
	{
		Quat rotation = Quat::FromEulerXYZ(0, -DegToRad(rotationSpeed) * App->time->DeltaTime(), 0);
		frustum.SetFront(rotation.Transform(frustum.Front()));
		frustum.SetUp(rotation.Transform(frustum.Up()));
	}
}

float ModuleEditorCamera::getHorizontalFOVrad() const
{
	return 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
}
