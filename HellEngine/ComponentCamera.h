#ifndef __H_COMPONENT_CAMERA__
#define __H_COMPONENT_CAMERA__

#include "MathGeoLib/src/Geometry/Frustum.h"
#include "Color.h"
#include "Component.h"


class ComponentCamera :
	public Component
{
public:
	ComponentCamera(GameObject* owner);
	virtual ~ComponentCamera() override;

	bool Init();
	void Update();

	const float* GetPosition();

	/* Sets the position of the camera */
	void SetPosition(float x, float y, float z);

	/* Sets the vertical FOV of the camera and adjust the horizontal FOV accordingly */
	bool SetFOV(float fov);

	float GetHorizontalFOV() const;

	float GetVerticalFOV() const;

	float GetAspectRatio() const;

	/* Returns a float* to the first of 16 floats representing the view matrix */
	float* GetViewMatrix();

	/* Returns a float* to the first of 16 floats representing the projection matrix */
	float* GetProjectionMatrix();

	/* Sets the distance for the near and far clipping planes */
	bool SetPlaneDistances(float near, float far);

	float GetNearPlaneDistance() const;

	bool SetNearPlaneDistance(float near);

	float GetFarPlaneDistance() const;

	bool SetFarPlaneDistance(float far);

	const float* GetFront() const;

	void SetFront(float x, float y, float z);

	const float* GetUp() const;

	void SetUp(float x, float y, float z);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

	float GetHorizontalFOVrad() const;

private:

	Color background;
	Frustum frustum;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float aspectRatio;
	float verticalFOVRad;
	float nearClippingPlane;
	float farClippingPlane;
	bool frustumCulling;
};

#endif
