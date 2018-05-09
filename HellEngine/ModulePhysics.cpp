#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

#include "ModulePhysics.h"

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadPhase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig);
	world->setGravity(btVector3(0.0f, -10.0f, 0.0f));

	return true;
}

bool ModulePhysics::CleanUp()
{
	return false;
}

UpdateStatus ModulePhysics::PreUpdate()
{
	return UpdateStatus();
}

UpdateStatus ModulePhysics::Update()
{
	return UpdateStatus();
}
