#ifndef __H_COMPONENT_RIGID_BODY__
#define __H_COMPONENT_RIGID_BODY__

#include "Bullet/include/LinearMath/btMotionState.h"
#include "MathGeoLib/src/Geometry/AABB.h"
#include "MathGeoLib/src/Geometry/Capsule.h"
#include "MathGeoLib/src/Geometry/OBB.h"
#include "MathGeoLib/src/Geometry/Sphere.h"
#include "Component.h"

class btRigidBody;

class ComponentRigidBody : public Component, public btMotionState
{
public:	
	enum class BodyType
	{
		BOX,
		SPHERE,
		CAPSULE
	};

public:
	ComponentRigidBody(GameObject* owner);
	~ComponentRigidBody();

	void GetBoundingBox(AABB& box) const;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

	void SetBodyType(BodyType new_type);
	BodyType GetBodyType() const;

	void OnPlay();
	void OnDebugDraw(bool selected) const;

	/* Methods from btMotionState */
	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;

private:
	void CreateBody();

public:
	BodyType bodyType = BodyType::SPHERE;

	Sphere sphere;
	OBB box;
	Capsule capsule;

	btRigidBody* body = nullptr;
	float mass = 1.0f;
	float restitution = 1.0f;
	float3 linearFactor = float3::one;
	float3 angularFactor = float3::one;
};

#endif
