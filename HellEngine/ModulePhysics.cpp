#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

#include "Bullet/include/btBulletDynamicsCommon.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "Application.h"
#include "ComponentRigidBody.h"
#include "ModulePhysics.h"
#include "ModuleTime.h"

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
	delete collisionConfig;
	collisionConfig = nullptr;
	delete dispatcher;
	dispatcher = nullptr;
	delete broadPhase;
	broadPhase = nullptr;
	delete solver;
	solver = nullptr;
	delete world;
	world = nullptr;
}

bool ModulePhysics::Init()
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadPhase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	debugDrawer = new DebugDrawer();

	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig);
	world->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	world->setDebugDrawer(debugDrawer);

	return true;
}

bool ModulePhysics::CleanUp()
{
	return false;
}

UpdateStatus ModulePhysics::PreUpdate()
{
	world->stepSimulation((btScalar)App->time->DeltaTimeMS(), 15);

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModulePhysics::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModulePhysics::SetGravity(float3& gravity)
{
	world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

float3 ModulePhysics::GetGravity()
{
	return float3(world->getGravity().x(), world->getGravity().y(), world->getGravity().z());
}

btRigidBody* ModulePhysics::AddSphereBody(const Sphere& sphere, ComponentRigidBody* component)
{
	float mass = component->mass;

	btCollisionShape* colShape = new btSphereShape(sphere.r);
	shapes.push_back(colShape);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, component, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	bodies.push_back(component);

	return body;
}

btRigidBody* ModulePhysics::AddBoxBody(const OBB& box, ComponentRigidBody* component)
{
	float mass = component->mass;

	btCollisionShape* colShape = new btBoxShape((btVector3(box.r.x, box.r.y, box.r.z)));

	shapes.push_back(colShape);

	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, component, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	bodies.push_back(component);

	return body;
}

btRigidBody* ModulePhysics::AddCapsuleBody(const Capsule& capsule, ComponentRigidBody* component)
{
	float mass = component->mass;

	btCollisionShape* colShape = new btCapsuleShape(capsule.r, capsule.LineLength());
	shapes.push_back(colShape);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, component, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	bodies.push_back(component);

	return body;
}

void ModulePhysics::DeleteBody(btRigidBody* body)
{
	if (body != nullptr)
		world->removeCollisionObject(body);
}

void ModulePhysics::OnPlay()
{
	for (std::list<ComponentRigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
		(*it)->OnPlay();
}

void ModulePhysics::OnStop()
{
	for (std::list<ComponentRigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
		DeleteBody((*it)->body);
}

void ModulePhysics::DrawDebug()
{
	if (DEBUG_MODE)
		world->debugDrawWorld();
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifetime, const btVector3& color)
{
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOGGER("Bullet error warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int DebugDrawer::getDebugMode() const
{
	return mode;
}
