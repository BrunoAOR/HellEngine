#ifndef __H_MODULE_EDITOR_CAMERA__
#define __H_MODULE_EDITOR_CAMERA__

#include "MathGeoLib/src/Geometry/Frustum.h"
#include "Color.h"
#include "Module.h"

class ModuleEditorCamera :
	public Module
{
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	UpdateStatus Update();

	const float* getPosition();

	/* Sets the position of the camera */
	void SetPosition(float x, float y, float z);

	/* Sets the vertical FOV of the camera and adjust the horizontal FOV accordingly */
	bool SetFOV(float fov);

	float getHorizontalFOV() const;

	float getVerticalFOV() const;

	float getAspectRatio() const;

	/* Returns a float* to the first of 16 floats representing the view matrix */
	float* GetViewMatrix();
	
	/* Returns a float* to the first of 16 floats representing the projection matrix */
	float* GetProjectionMatrix();

	/* Sets the distance for the near and far clipping planes */
	bool SetPlaneDistances(float near, float far);

	float getNearPlaneDistance() const;

	bool SetNearPlaneDistance(float near);

	float getFarPlaneDistance() const;

	bool SetFarPlaneDistance(float far);
	
	/* Method to be called when the window is resized */
	void onWindowResize();

	/* Method to be called when yaw rotation is required */
	void RotateYaw(const int direction);

	/* Method to be called when pitch rotation is required */
	void RotatePitch(const int direction);

	void DragCameraHorizontalAxis(int direction, vec &frustumPos, float speed);

	void DragCameraVerticalAxis(int direction, vec &frustumPos, float speed);

	const float* GetFront() const;

	void SetFront(float x, float y, float z);

	const float* GetUp() const;

	void SetUp(float x, float y, float z);

public:

	Color background;
	float moveSpeed;
	float rotationSpeed;
	float zoomSpeed;

	bool currentlyMovingCamera = false;
	bool currentlyZoomingCamera = false;

private:

	void handleCameraMotion();
	void handleCameraRotation();
	float getHorizontalFOVrad() const;

private:

	Frustum frustum;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float aspectRatio;
	float verticalFOVRad;
	float nearClippingPlane;
	float farClippingPlane;
};

#endif // !__H_MODULE_EDITOR_CAMERA__
