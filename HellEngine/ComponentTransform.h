#ifndef __H_COMPONENT_TRANSFORM__
#define __H_COMPONENT_TRANSFORM__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Component.h"


class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform() override;

	float3 GetPosition();
	float3 GetScale();
	float3 GetRotation();

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotationRad(float x, float y, float z);
	void SetRotationDeg(float x, float y, float z);

	float* GetModelMatrix();

	void SetParent(ComponentTransform* newParent);

	virtual void OnEditor() override;

private:

	float4x4& GetModelMatrix4x4();
	float4x4& UpdateLocalModelMattix();

private:

	float3 position;
	float3 scale;
	Quat rotation;

	float4x4 localModelMatrix;
	float4x4 worldModelMatrix;

};

#endif
