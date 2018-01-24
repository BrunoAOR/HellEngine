#ifndef __H_MODULE_EDITOR_CAMERA__
#define __H_MODULE_EDITOR_CAMERA__

#include "MathGeoLib/src/Geometry/Frustum.h"
#include "Module.h"

class ModuleEditorCamera :
	public Module
{
public:

	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	UpdateStatus Update();

	/* Sets the position of the camera */
	void SetPosition(float x, float y, float z);

	/* Sets the vertical FOV of the camera and adjust the horizontal FOV accordingly */
	bool SetFOV(float fov);

	/* Returns a float* to the first of 16 floats representing the view matrix */
	float* GetViewMatrix();
	
	/* Returns a float* to the first of 16 floats representing the projection matrix */
	float* GetProjectionMatrix();

	/* Sets the distance for the near and far clipping planes */
	bool SetPlaneDistances(float near, float far);

	/* Method to be called when the window is resized */
	void onWindowResize();

private:

	float getHorizontalFOV(float vertFOV) const;

private:

	Frustum frustum;
	const float moveSpeed = 1;
	float aspectRatio = 1;
	float verticalFOVRad = 1;
};

#endif // !__H_MODULE_EDITOR_CAMERA__
