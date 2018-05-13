#include "Bullet/include/btBulletDynamicsCommon.h"
#include "openGL.h"
#include "Application.h"
#include "DebugDrawer.h"
#include "ModuleShaderManager.h"
#include "ShaderProgram.h"

DebugDrawer::DebugDrawer()
{
	shaderProgram = App->shaderManager->GetShaderProgram("assets/shaders/debugShader.vert", "assets/shaders/debugShader.frag");
}

DebugDrawer::~DebugDrawer()
{
	App->shaderManager->ReleaseShaderProgram(shaderProgram);
	shaderProgram = nullptr;
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{	
	
}

void DebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifetime, const btVector3 & color)
{
}

void DebugDrawer::reportErrorWarning(const char * warningString)
{
}

void DebugDrawer::draw3dText(const btVector3 & location, const char * textString)
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

