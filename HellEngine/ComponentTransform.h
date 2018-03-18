#ifndef __H_COMPONENT_TRANSFORM__
#define __H_COMPONENT_TRANSFORM__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "MathGeoLib/src/Geometry/AABB.h"
#include "Component.h"
#include "ComponentMesh.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform() override;

	virtual void Update();

	bool Equals(ComponentTransform* t);

	float3 GetPosition() const;
	float3 GetScale() const;
	float3 GetRotationRad() const;
	float3 GetRotationDeg() const;
	Quat GetRotationQuat() const;

	AABB GetBoundingBox() const;

	bool GetIsStatic() const;
	void SetIsStatic(bool isStatic);

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(Quat newRotation);
	void SetRotationRad(float x, float y, float z);
	void SetRotationDeg(float x, float y, float z);

	void UpdateBoundingBox(ComponentMesh* mesh = nullptr);
	void EncloseBoundingBox(ComponentMesh* mesh);

	const float4x4& GetModelMatrix4x4() const;
	const float* GetModelMatrix() const;

	void RecalculateLocalMatrix(ComponentTransform* newParent);

	virtual void OnEditor() override;

	virtual void Save(nlohmann::json& json) const override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

    /* Aplies the transform from the Guizmo */
    void ApplyWorldTransformationMatrix(const float4x4& worldTransformationMatrix, bool excludePosition = false, bool excludeRotation = false, bool excludeScale = false);

private:

	void UpdateMatrices(const float4x4* newParentWorldMatrix = nullptr);
	void UpdateLocalModelMatrix();
	void UpdateWorldModelMatrix(const float4x4* newParentWorldMatrix = nullptr);
	void InitializeBaseBB();
	void CreateBBVAO();

private:

	float3 position;
	float3 scale;
	Quat rotation;
	float rotationDeg[3];
	AABB boundingBox;

	bool drawBoundingBox = false;
	bool isStatic = false;

	float4x4 localModelMatrix;
	float4x4 worldModelMatrix;

	float boundingBoxUniqueData[8 * 6];

	static std::vector<float3> baseBoundingBox;
	static MeshInfo baseBoundingBoxMeshInfo;

};

#endif // !__H_COMPONENT_TRANSFORM__
