#ifndef __H_MODULE_PHYSICS__
#define __H_MODULE_PHYSICS__

#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Module.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Init();
	bool CleanUp();
	UpdateStatus PreUpdate();
	UpdateStatus Update();

private:
	btDefaultCollisionConfiguration* collisionConfig = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
};

#endif
