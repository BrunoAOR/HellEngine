#include <math.h>
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
	verticalFOVRad = DegToRad(60);
	onWindowResize();
	frustum.SetViewPlaneDistances(0.1f, 100.0f);
	frustum.SetFrame(vec(0, 0, 3), vec(0, 0, -1), vec(0, 1, 0));
	
	return true;
}

UpdateStatus ModuleEditorCamera::Update()
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
		pos.x += frustum.Front().x * moveSpeed * App->time->DeltaTime();
		pos.y += frustum.Front().y * moveSpeed * App->time->DeltaTime();
		pos.z += frustum.Front().z * moveSpeed * App->time->DeltaTime();
	}
	/* Camera backwards */
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
	{
		pos.x -= frustum.Front().x * moveSpeed * App->time->DeltaTime();
		pos.y -= frustum.Front().y * moveSpeed * App->time->DeltaTime();
		pos.z -= frustum.Front().z * moveSpeed * App->time->DeltaTime();
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

	return UpdateStatus::UPDATE_CONTINUE;
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
	frustum.SetPerspective(getHorizontalFOV(verticalFOVRad), verticalFOVRad);
	return true;
}

/* Returns a float* to the first of 16 floats representing the view matrix */
float* ModuleEditorCamera::GetViewMatrix()
{
	float4x4 viewMatrix = frustum.ViewMatrix();
	return viewMatrix.Transposed().v[0];
}

/* Returns a float* to the first of 16 floats representing the projection matrix */
float* ModuleEditorCamera::GetProjectionMatrix()
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

/* Method to be called when the window is resized */
void ModuleEditorCamera::onWindowResize()
{
	int width = App->window->getWidth();
	int height = App->window->getHeight();

	aspectRatio = (float)width / height;
	float h = getHorizontalFOV(verticalFOVRad);
	frustum.SetPerspective(getHorizontalFOV(verticalFOVRad), verticalFOVRad);
}

float ModuleEditorCamera::getHorizontalFOV(float vertFOV) const
{
	return 2 * atan(tan(vertFOV / 2) * aspectRatio);
}
