#include <assert.h>
#include "ShaderProgram.h"
#include "openGL.h"

ShaderProgram::ShaderProgram(unsigned int programId, unsigned int modelMatrixLocation, unsigned int viewMatrixLocation, unsigned int projectionMatrixLocation)
	: programId(programId)
	, modelMatrixLocation(modelMatrixLocation)
	, viewMatrixLocation(viewMatrixLocation)
	, projectionMatrixLocation(projectionMatrixLocation)
{
	assert(programId && modelMatrixLocation != -1 && viewMatrixLocation != -1 && projectionMatrixLocation != -1);
}

ShaderProgram::~ShaderProgram()
{
}

unsigned int ShaderProgram::GetProgramId() const
{
	return programId;
}

void ShaderProgram::Activate() const
{
	glUseProgram(programId);
}

void ShaderProgram::Deactivate() const
{
	glUseProgram(GL_NONE);
}

void ShaderProgram::UpdateMatrixUniforms(const float* model, const float* view, const float* projection) const
{
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, model);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, view);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projection);
}
