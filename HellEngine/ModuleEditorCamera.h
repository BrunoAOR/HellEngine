#ifndef __H_MODULE_EDITOR_CAMERA__
#define __H_MODULE_EDITOR_CAMERA__

#include "MathGeoLib/src/Geometry/Frustum.h"
#include "Color.h"
#include "ComponentCamera.h"
#include "Module.h"

class ModuleEditorCamera :
	public Module
{
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	UpdateStatus Update();
	
	/* Method to be called when the window is resized */
	void OnWindowResize();

	/* Method to be called when yaw rotation is required */
	void RotateYaw(const int direction);

	/* Method to be called when pitch rotation is required */
	void RotatePitch(const int direction);

	void DragCameraHorizontalAxis(int direction, vec &frustumPos, float speed);

	void DragCameraVerticalAxis(int direction, vec &frustumPos, float speed);

public:
	ComponentCamera* camera;
	float moveSpeed;
	float rotationSpeed;
	float zoomSpeed;

	bool currentlyMovingCamera = false;
	bool currentlyZoomingCamera = false;

private:

	void HandleCameraMotion();
	void HandleCameraRotation();
};

#endif // !__H_MODULE_EDITOR_CAMERA__
