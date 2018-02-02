#ifndef __H_COMPONENT_TRANSFORM__
#define __H_COMPONENT_TRANSFORM__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Component.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform();
	~ComponentTransform();

	float3 GetPosition();
	float3 GetScale();
	float3 GetRotation();

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	float* GetModelMatrix();

	virtual void OnEditor() override;

private:
	float3 position;
	float3 scale;
	Quat rotation;

	float modelMatrix[16];
};

#endif
