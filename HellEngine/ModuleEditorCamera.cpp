#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"


ModuleEditorCamera::ModuleEditorCamera()
{}

ModuleEditorCamera::~ModuleEditorCamera()
{}

bool ModuleEditorCamera::Init()
{
	frustum.SetPerspective(DegToRad(30), DegToRad(30));
	frustum.SetViewPlaneDistances(0.1f, 100.0f);
	frustum.SetFrame(vec(0, 0, 0), vec(0, 0, 1), vec(0, 1, 0));
	
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

void ModuleEditorCamera::SetPosition(float x, float y, float z)
{
	frustum.SetPos(vec(x, y, z));
}

float* ModuleEditorCamera::GetViewMatrix()
{
	float4x4 viewMatrix = frustum.ViewMatrix();
	return viewMatrix.Transposed().v[0];
}

float* ModuleEditorCamera::GetProjectionMatrix()
{
	return frustum.ProjectionMatrix().Transposed().v[0];
}
