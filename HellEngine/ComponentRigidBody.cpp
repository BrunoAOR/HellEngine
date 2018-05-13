#include "Bullet/include/btBulletDynamicsCommon.h"
#include "globals.h"
#include "Application.h"
#include "Component.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "SerializableObject.h"

ComponentRigidBody::ComponentRigidBody(GameObject* owner) : Component(owner)
{
	type = ComponentType::RIGID_BODY;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);

	sphere.r = 1.0f;
	sphere.pos = float3::zero;

	box.r = float3::one;
	box.pos = float3::zero;
	box.axis[0] = float3::unitX;
	box.axis[1] = float3::unitY;
	box.axis[2] = float3::unitZ;

	capsule.r = 1.0f;
	capsule.l = LineSegment(float3::zero, float3::one);
}

ComponentRigidBody::~ComponentRigidBody()
{
	if (body != nullptr && App->physics3d != nullptr)
		App->physics3d->DeleteBody(body);
}

void ComponentRigidBody::GetBoundingBox(AABB& box) const
{
	switch (bodyType)
	{
	case BodyType::SPHERE:
		box.Enclose(sphere);
		break;
	case BodyType::BOX:
		box.Enclose(box);
		break;
	case BodyType::CAPSULE:
		box.Enclose(capsule);
	}
}

void ComponentRigidBody::Save(SerializableObject& obj) const
{
	Component::Save(obj);
	obj.AddFloat("Mass", mass);
	switch (bodyType)
	{
	case BodyType::SPHERE:
		obj.AddString("BodyType", "Sphere");
		break;
	case BodyType::BOX:
		obj.AddString("BodyType", "Box");
		break;
	case BodyType::CAPSULE:
		obj.AddString("BodyType", "Capsule");
	}
}

void ComponentRigidBody::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	mass = obj.GetFloat("Mass");
	std::string s = obj.GetString("BodyType");

	if (s.compare("Sphere") == 0)
		bodyType = BodyType::SPHERE;
	else if (s.compare("Box") == 0)
		bodyType = BodyType::BOX;
	else if (s.compare("Capsule") == 0)
		bodyType = BodyType::CAPSULE;
	else
		LOGGER("Error when loading ComponentRigidBody: Unknown body type");
}

void ComponentRigidBody::SetBodyType(BodyType newType)
{
	bodyType = newType;
}

ComponentRigidBody::BodyType ComponentRigidBody::GetBodyType() const
{
	return bodyType;
}

void ComponentRigidBody::OnPlay()
{
	CreateBody();
}

void ComponentRigidBody::OnDebugDraw(bool selected) const
{
}

void ComponentRigidBody::getWorldTransform(btTransform& worldTransformation) const
{
	ComponentTransform* transform = (ComponentTransform*) gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform)
	{
		float3 position;
		Quat rotation;
		float3 scale;
		float4x4 worldMatrix = transform->GetModelMatrix4x4();
		DecomposeMatrix(worldMatrix, position, rotation, scale);

		worldTransformation.setOrigin(btVector3(position.x, position.y, position.z));
		worldTransformation.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	}

}

void ComponentRigidBody::setWorldTransform(const btTransform& worldTransformation)
{
	btQuaternion rotation = worldTransformation.getRotation();
	btVector3 position = worldTransformation.getOrigin();

	float4x4 newGlobal(Quat(rotation.x(), rotation.y(), rotation.z(), rotation.w()), float3(position.x(), position.y(), position.z()));

	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform)
	{
		float4x4 currentLocal(transform->GetRotationQuat(), transform->GetPosition());
		float4x4 newLocal = newGlobal * currentLocal;

		float3 translation, scale;
		Quat rot;

		DecomposeMatrix(newLocal, translation, rot, scale);
		transform->SetPosition(translation.x, translation.y, translation.z);
		transform->SetRotation(rot);
		transform->SetScale(scale.x, scale.y, scale.z);
	}
}

void ComponentRigidBody::CreateBody()
{
	if (body != nullptr)
		App->physics3d->DeleteBody(body);

	switch (bodyType)
	{
	case BodyType::SPHERE:
		body = App->physics3d->AddSphereBody(sphere, this);
		break;
	case BodyType::BOX:
		body = App->physics3d->AddBoxBody(box, this);
		break;
	case BodyType::CAPSULE:
		body = App->physics3d->AddCapsuleBody(capsule, this);
		break;
	}

	if (body != nullptr)
	{
		body->setRestitution(restitution);
		body->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
		body->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
	}
}