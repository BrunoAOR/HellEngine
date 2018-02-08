#ifndef __H_COMPONENT_TRANSFORM__
#define __H_COMPONENT_TRANSFORM__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Component.h"
#include "ComponentMesh.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform() override;

	virtual void Update();

	float3 GetPosition();
	float3 GetScale();
	float3 GetRotation();

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotationRad(float x, float y, float z);
	void SetRotationDeg(float x, float y, float z);

	void UpdateCubeBoundingBox();

	float* GetModelMatrix();
	float4x4& GetModelMatrix4x4();

	void SetParent(ComponentTransform* newParent);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

	enum class RotationAxisModified { MOD_ALL, MOD_X, MOD_Y, MOD_Z };

	float4x4& UpdateLocalModelMatrix();

	RotationAxisModified SetRotationDegFormGUI(float x, float y, float z);

private:

	float3 position;
	float3 scale;
	Quat rotation;
	float rotationDeg[3];
	AABB boundingBox;

	bool activeBoundingBox = false;

	RotationAxisModified rotationMod = RotationAxisModified::MOD_ALL;

	float4x4 localModelMatrix;
	float4x4 worldModelMatrix;

};

#endif
