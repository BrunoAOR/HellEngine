#include <math.h>
#include "MathGeoLib/src/Math/Quat.h"
#include "SDL/include/SDL_mouse.h"
#include "Application.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "globals.h"


ModuleEditorCamera::ModuleEditorCamera()
{}

ModuleEditorCamera::~ModuleEditorCamera()
{}

bool ModuleEditorCamera::Init()
{
	std::vector<Component*> cameras = App->scene->root->GetComponents(ComponentType::CAMERA);
	if (cameras.size() > 0) {
		camera = (ComponentCamera*)cameras[0];
		if (camera == nullptr) {
			LOGGER("Root GameObject in scene can't be found!");
			return false;
		}
	}

	OnWindowResize();

	moveSpeed = 10;
	rotationSpeed = 15;
	zoomSpeed = 100;
	
	return true;
}

UpdateStatus ModuleEditorCamera::Update()
{
	HandleCameraMotion();
	HandleCameraRotation();	

	return UpdateStatus::UPDATE_CONTINUE;
}


/* Method to be called when the window is resized */
void ModuleEditorCamera::OnWindowResize()
{
	int width = App->window->getWidth();
	int height = App->window->getHeight();

	float aspectRatio = (float)width / height;
	camera->SetAspectRatio(aspectRatio);
	camera->SetPerspective(DegToRad(camera->GetHorizontalFOV()), DegToRad(camera->GetVerticalFOV()));
}


void ModuleEditorCamera::HandleCameraMotion()
{
	vec pos = camera->GetPosition3();
	int moveFactor = App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT ? 3 : 1;

	/* Handling Keyboard (Arrows) */

	/* Camera forward or backwards  */
	/* Camera forward */
	if ((App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		&& !currentlyZoomingCamera)
	{
		/* Set up a vector that looks forward as the frustum, but that lies on the XZ-plane */
		vec forwardInPlane = camera->GetFront3();
		/* Handle looking straight down case */
		if (forwardInPlane.y == -1)
		{
			forwardInPlane = camera->GetUp3();
		}
		/* Handle looking straight up case */
		else if (forwardInPlane.y == 1)
		{
			forwardInPlane = -camera->GetUp3();
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
		DragCameraHorizontalAxis(1, pos, moveFactor * moveSpeed);
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
			pos.y -= moveFactor * moveSpeed * App->time->DeltaTime();
		/* Camera down */
		if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
			pos.y += moveFactor * moveSpeed * App->time->DeltaTime();

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

	camera->SetPosition(pos.x, pos.y, pos.z);
}

void ModuleEditorCamera::HandleCameraRotation()
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
	float3 rot = rotation.Transform(camera->GetFront3());
	camera->SetFront(rot.x, rot.y, rot.z);
	rot = rotation.Transform(camera->GetUp3());
	camera->SetUp(rot.x, rot.y, rot.z);
}

void ModuleEditorCamera::RotatePitch(int mouseMotionY)
{
	/* mouseMotionY is <0 when moving up and >0 when moving down */
	Quat rotation = Quat::RotateAxisAngle(camera->GetRight3(), -DegToRad(rotationSpeed * mouseMotionY) * App->time->DeltaTime());
	vec newUp = rotation.Transform(camera->GetUp3());
	if (newUp.y >= 0)
	{
		float3 rot = rotation.Transform(camera->GetFront3());
		camera->SetFront(rot.x, rot.y, rot.z);
		camera->SetUp(newUp.x, newUp.y, newUp.z);
	}
}

void ModuleEditorCamera::DragCameraHorizontalAxis(int direction, vec& frustumPos, float speed)
{
	frustumPos.x -= camera->GetRight3().x * speed * App->time->DeltaTime() * direction;
	frustumPos.y -= camera->GetRight3().y * speed * App->time->DeltaTime() * direction;
	frustumPos.z -= camera->GetRight3().z * speed * App->time->DeltaTime() * direction;
}

void ModuleEditorCamera::DragCameraVerticalAxis(int direction, vec& frustumPos, float speed)
{
	frustumPos.x += camera->GetFront3().x * speed * App->time->DeltaTime() * direction;
	frustumPos.y += camera->GetFront3().y * speed * App->time->DeltaTime() * direction;
	frustumPos.z += camera->GetFront3().z * speed * App->time->DeltaTime() * direction;
}
