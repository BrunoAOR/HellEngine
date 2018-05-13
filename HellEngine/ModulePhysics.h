#ifndef __H_MODULE_PHYSICS__
#define __H_MODULE_PHYSICS__

#include <list>
#include "Bullet/include/LinearMath/btIDebugDraw.h"
#include "globals.h"
#include "Module.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class DebugDrawer;
class ComponentRigidBody;

class btCollisionShape;
class btRigidBody;

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Init();
	bool CleanUp();
	UpdateStatus PreUpdate();
	UpdateStatus Update();

	void SetGravity(float3& gravity);
	float3 GetGravity();

	btRigidBody* AddSphereBody(const Sphere& sphere, ComponentRigidBody* component);
	btRigidBody* AddBoxBody(const OBB& box, ComponentRigidBody* component);
	btRigidBody* AddCapsuleBody(const Capsule& capsule, ComponentRigidBody* component);

	void DeleteBody(btRigidBody* body);

	void OnPlay();
	void OnStop();

	void DrawDebug();

private:
	btDefaultCollisionConfiguration* collisionConfig = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	DebugDrawer* debugDrawer = nullptr;

	bool gravityEnabled = true;

	std::list<btCollisionShape*> shapes;
	std::list<ComponentRigidBody*> bodies;
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() {}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
		btScalar distance, int lifetime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int getDebugMode() const;

public:
	DebugDrawModes mode;
};

#endif
