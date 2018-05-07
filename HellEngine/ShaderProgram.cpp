#include <assert.h>
#include "ShaderProgram.h"
#include "globals.h"
#include "openGL.h"

ShaderProgram::ShaderProgram(unsigned int programId, ShaderOptions options) : programId(programId), shaderOptions(options)
{
	assert(programId);
	modelMatrixLocation = glGetUniformLocation(programId, "model_matrix");
	viewMatrixLocation = glGetUniformLocation(programId, "view_matrix");
	projectionMatrixLocation = glGetUniformLocation(programId, "projection_matrix");
	assert(modelMatrixLocation != -1 && viewMatrixLocation != -1 && projectionMatrixLocation != -1);

	normalMatrixLocation = glGetUniformLocation(programId, "normal_matrix");
	lightPositionLocation = glGetUniformLocation(programId, "light_position");
	cameraPositionLocation = glGetUniformLocation(programId, "camera_position");

	bonesPaletteLocation = glGetUniformLocation(programId, "bones_palette");
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

void ShaderProgram::UpdateMatrixUniforms(const float* modelMatrix, const float* viewMatrix, const float* projectionMatrix) const
{
	if (modelMatrix)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, modelMatrix);
	if (viewMatrix)
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, viewMatrix);
	if (projectionMatrix)
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix);
}

void ShaderProgram::UpdateLightingUniforms(const float* normalMatrix, const float* lightPosition, const float* cameraPosition) const
{
	if (normalMatrix)
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, normalMatrix);
	if (lightPosition)
		glUniform3fv(lightPositionLocation, 1, lightPosition);
	if (cameraPosition)
		glUniform3fv(cameraPositionLocation, 1, cameraPosition);
}

void ShaderProgram::UpdateBonesUniform(const float* bonesPalette) const
{
	glUniformMatrix4fv(bonesPaletteLocation, MAX_BONES, GL_FALSE, bonesPalette);
}

ShaderOptions ShaderProgram::GetShaderOptions() const
{
	return shaderOptions;
}
