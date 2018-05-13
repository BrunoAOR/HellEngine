#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleShaderManager.h"
#include "ShaderProgram.h"
#include "openGL.h"

ModuleDebugDraw::ModuleDebugDraw()
{
}

ModuleDebugDraw::~ModuleDebugDraw()
{
}

bool ModuleDebugDraw::Init()
{
	shaderProgram = App->shaderManager->GetShaderProgram("assets/shaders/debugShader.vert", "assets/shaders/debugShader.frag");
	return shaderProgram != nullptr;
}

bool ModuleDebugDraw::CleanUp()
{
	App->shaderManager->ReleaseShaderProgram(shaderProgram);
	shaderProgram = nullptr;
	
	return true;
}

bool ModuleDebugDraw::DrawElements(const float* modelMatrix, uint vao, uint vertexCount)
{
	shaderProgram->Activate();
	shaderProgram->UpdateModelMatrixUniform(modelMatrix);

	glBindVertexArray(vao);
	glDrawElements(GL_LINES, vertexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(GL_NONE);
	
	shaderProgram->Deactivate();
	return true;
}
