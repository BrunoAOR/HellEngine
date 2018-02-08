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

	const float* GetPosition() const;

	const float3 GetPosition3() const;

	/* Sets the position of the camera */
	void SetPosition(float x, float y, float z);

	/* Sets the vertical FOV of the camera and adjust the horizontal FOV accordingly */
	bool SetFOV(float fov);

	float GetHorizontalFOV() const;

	float GetVerticalFOV() const;

	float GetAspectRatio() const;

	void SetAspectRatio(float ratio);

	void SetPerspective(float fovh, float fovv);

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

	const float3 GetFront3() const;

	void SetFront(float x, float y, float z);

	const float* GetUp() const;

	const float3 GetUp3() const;

	void SetUp(float x, float y, float z);

	const float3 GetRight3() const;

	const Color GetBackground() const;

	void SetBackground(float red, float green, float blue);

	bool ContainsAABB(const AABB& bb) const;

	bool FrustumCulling();

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

public:

	bool isActiveCamera;

private:

	float GetHorizontalFOVrad() const;
	void DrawFrustum() const;

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
