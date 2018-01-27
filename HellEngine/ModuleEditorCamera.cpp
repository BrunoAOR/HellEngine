#include <math.h>
#include "MathGeoLib/src/Math/Quat.h"
#include "SDL/include/SDL_mouse.h"
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
	moveSpeed = 10;
	rotationSpeed = 15;
	zoomSpeed = 100;

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
float* ModuleEditorCamera::GetViewMatrix()
{
	viewMatrix = frustum.ViewMatrix();
	viewMatrix.Transpose();
	return viewMatrix.v[0];
}

/* Returns a float* to the first of 16 floats representing the projection matrix */
float* ModuleEditorCamera::GetProjectionMatrix()
{
	projectionMatrix = frustum.ProjectionMatrix().Transposed();
	return projectionMatrix.v[0];
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
	int moveFactor = App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT ? 3 : 1;

	/* Handling Keyboard (Arrows) */

	/* Camera forward or backwards  */
	/* Camera forward */
	if ((App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		&& !currentlyZoomingCamera)
	{
		/* Set up a vector that looks forward as the frustum, but that lies on the XZ-plane */
		vec forwardInPlane = frustum.Front();
		/* Handle looking straight down case */
		if (forwardInPlane.y == -1)
		{
			forwardInPlane = frustum.Up();
		}
		/* Handle looking straight up case */
		else if (forwardInPlane.y == 1)
		{
			forwardInPlane = -frustum.Up();
		}
		/* Handle generic case, where the y-coordinate gets turned to zero */
		else
		{
			forwardInPlane.y = 0;
			forwardInPlane.Normalize();
		}
		currentlyMovingCamera = true;

		/* Camera forwards */
		if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
		{
			pos.x += forwardInPlane.x * moveFactor * moveSpeed * App->time->DeltaTime();
			pos.z += forwardInPlane.z * moveFactor * moveSpeed * App->time->DeltaTime();
		}
		/* Camera backwards */
		else
		{
			pos.x -= forwardInPlane.x * moveFactor * moveSpeed * App->time->DeltaTime();
			pos.z -= forwardInPlane.z * moveFactor * moveSpeed * App->time->DeltaTime();
		}
	}
	
	/* Camera left */
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT && !currentlyZoomingCamera)
	{
		DragCameraHorizontalAxis(1, pos, moveFactor *moveSpeed);
		currentlyMovingCamera = true;
	}
	/* Camera right */
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT && !currentlyZoomingCamera)
	{
		DragCameraHorizontalAxis(-1, pos, moveFactor * moveSpeed);
		currentlyMovingCamera = true;
	}


	if (App->input->GetKey(SDL_SCANCODE_UP) != KeyState::KEY_REPEAT
		&& App->input->GetKey(SDL_SCANCODE_DOWN) != KeyState::KEY_REPEAT
		&& App->input->GetKey(SDL_SCANCODE_LEFT) != KeyState::KEY_REPEAT
		&& App->input->GetKey(SDL_SCANCODE_RIGHT) != KeyState::KEY_REPEAT)
	{
		currentlyMovingCamera = false;
	}

	/* Handling mouse*/

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT
		&& !currentlyZoomingCamera)
	{
		/* Camera up */
		if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
			pos.y += moveFactor * moveSpeed * App->time->DeltaTime();
		/* Camera down */
		if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
			pos.y -= moveFactor * moveSpeed * App->time->DeltaTime();

		/* Camera forward */
		if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
		{
			DragCameraVerticalAxis(1, pos, moveFactor * moveSpeed);
		}
		/* Camera backwards */
		if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
		{
			DragCameraVerticalAxis(-1, pos, moveFactor * moveSpeed);
		}

		/* Camera left */
		if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
		{
			DragCameraHorizontalAxis(1, pos, moveFactor * moveSpeed);
		}
		/* Camera right */
		if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
		{
			DragCameraHorizontalAxis(-1, pos, moveFactor * moveSpeed);
		}
	}

	/*Zoom with camera */
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT
		&& App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT
		&& !currentlyMovingCamera)
	{
		currentlyZoomingCamera = true;
		if (App->input->GetMouseMotion().x > 0 || App->input->GetMouseMotion().y > 0) {
			DragCameraVerticalAxis(1, pos, zoomSpeed);
		}
		if (App->input->GetMouseMotion().x < 0 || App->input->GetMouseMotion().y < 0) {
			DragCameraVerticalAxis(-1, pos, zoomSpeed);
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_UP)
	{
		currentlyZoomingCamera = false;
	}

	/*Zoom with mouse wheel */
	if (App->input->GetMouseWheel().y != 0) // scroll up
	{
		DragCameraVerticalAxis(App->input->GetMouseWheel().y, pos, 5 * zoomSpeed);
	}	

	frustum.SetPos(pos);
}

void ModuleEditorCamera::handleCameraRotation()
{
	/*Handling Mouse*/
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		int xMotion = App->input->GetMouseMotion().x;
		int yMotion = App->input->GetMouseMotion().y;

		/* Camera rotate upwards and downwards */
		if (yMotion != 0
			&& !currentlyMovingCamera
			&& !currentlyZoomingCamera)
		{
			RotatePitch(yMotion);
		}
		
		/* Camera rotate leftwards and rightwards */
		if (xMotion != 0
			&& !currentlyMovingCamera
			&& !currentlyZoomingCamera)
		{
			RotateYaw(xMotion);
		}
	}
}

void ModuleEditorCamera::RotateYaw(int mouseMotionX)
{
	/* mouseMotionX is <0 when moving to the left and >0 when moving to the right */
	Quat rotation = Quat::FromEulerXYZ(0, -DegToRad(rotationSpeed * mouseMotionX) * App->time->DeltaTime(), 0);
	frustum.SetFront(rotation.Transform(frustum.Front()));
	frustum.SetUp(rotation.Transform(frustum.Up()));
}

void ModuleEditorCamera::RotatePitch(int mouseMotionY)
{
	/* mouseMotionY is <0 when moving up and >0 when moving down */
	Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), -DegToRad(rotationSpeed * mouseMotionY) * App->time->DeltaTime());
	vec newUp = rotation.Transform(frustum.Up());
	if (newUp.y >= 0)
	{
		frustum.SetFront(rotation.Transform(frustum.Front()));
		frustum.SetUp(newUp);
	}
}

void ModuleEditorCamera::DragCameraHorizontalAxis(int direction, vec& frustumPos, float speed)
{
	frustumPos.x -= frustum.WorldRight().x * speed * App->time->DeltaTime() * direction;
	frustumPos.y -= frustum.WorldRight().y * speed * App->time->DeltaTime() * direction;
	frustumPos.z -= frustum.WorldRight().z * speed * App->time->DeltaTime() * direction;
}

void ModuleEditorCamera::DragCameraVerticalAxis(int direction, vec& frustumPos, float speed)
{
	frustumPos.x += frustum.Front().x * speed * App->time->DeltaTime() * direction;
	frustumPos.y += frustum.Front().y * speed * App->time->DeltaTime() * direction;
	frustumPos.z += frustum.Front().z * speed * App->time->DeltaTime() * direction;
}

float ModuleEditorCamera::getHorizontalFOVrad() const
{
	return 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
}
