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
	frustrum.SetFrame(vec(0, 0, 0), vec(0, 0, 1), vec(0, 1, 0));
	
	return true;
}
