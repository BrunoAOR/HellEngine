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
	frustrum.SetPerspective(30, 30);
	frustrum.SetViewPlaneDistances(0.1f, 100.0f);
	frustrum.SetFrame(vec(0, 0, -5), vec(0, 0, 1), vec(0, 1, 0));
	
	return true;
}

UpdateStatus ModuleEditorCamera::Update()
{
	vec pos = frustrum.Pos();
	/* Camera up */
	if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
		pos.y -= moveSpeed * App->time->DeltaTime();
	/* Camera down */
	if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
		pos.y += moveSpeed * App->time->DeltaTime();
	
	/* Camera forward */
	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
	{
		pos.x += frustrum.Front().x * moveSpeed * App->time->DeltaTime();
		pos.y += frustrum.Front().y * moveSpeed * App->time->DeltaTime();
		pos.z += frustrum.Front().z * moveSpeed * App->time->DeltaTime();
	}
	/* Camera backwards */
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
	{
		pos.x -= frustrum.Front().x * moveSpeed * App->time->DeltaTime();
		pos.y -= frustrum.Front().y * moveSpeed * App->time->DeltaTime();
		pos.z -= frustrum.Front().z * moveSpeed * App->time->DeltaTime();
	}

	/* Camera left */
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
	{
		pos.x += frustrum.WorldRight().x * moveSpeed * App->time->DeltaTime();
		pos.y += frustrum.WorldRight().y * moveSpeed * App->time->DeltaTime();
		pos.z += frustrum.WorldRight().z * moveSpeed * App->time->DeltaTime();
	}
	/* Camera right */
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
	{
		pos.x -= frustrum.WorldRight().x * moveSpeed * App->time->DeltaTime();
		pos.y -= frustrum.WorldRight().y * moveSpeed * App->time->DeltaTime();
		pos.z -= frustrum.WorldRight().z * moveSpeed * App->time->DeltaTime();
	}
	
	frustrum.SetPos(pos);

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleEditorCamera::SetPosition(float x, float y, float z)
{
	frustrum.SetPos(vec(x, y, z));
}

float* ModuleEditorCamera::GetViewMatrix()
{
	float4x4 viewMatrix = frustrum.ViewMatrix();
	return viewMatrix.Transposed().v[0];
}

float* ModuleEditorCamera::GetProjectionMatrix()
{
	return frustrum.ProjectionMatrix().Transposed().v[0];
}
