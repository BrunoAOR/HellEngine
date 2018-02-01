#include "Application.h"
#include "Material.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "openGL.h"


Material::Material()
{
	shader = new Shader();
}


Material::~Material()
{
	delete shader;
	shader = nullptr;

	glDeleteTextures(1, &textureBufferId);
	textureBufferId = 0;
}

/* Recieves the vertex shader file path and tries to compile it */
bool Material::SetVertexShaderPath(const std::string& sourcePath)
{
	std::string vertexString;
	if (!LoadTextFile(sourcePath, vertexString))
		return false;

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;
	
	vertexShaderPath = sourcePath;

	return true;
}

/* Recieves the fragment shader file path and tries to compile it */
bool Material::SetFragmentShaderPath(const std::string& sourcePath)
{
	std::string fragmentString;
	if (!LoadTextFile(sourcePath, fragmentString))
		return false;

	if (!shader->CompileFragmentShader(fragmentString.c_str()))
		return false;
	
	fragmentShaderPath = sourcePath;

	return true;
}

/* Recieves the texture file path and tries to load it using Devil */
bool Material::SetTexturePath(const std::string& sourcePath)
{
	glDeleteTextures(1, &textureBufferId);

	textureBufferId = App->renderer->LoadImageWithDevIL(sourcePath.c_str());

	if (textureBufferId == 0)
		return false;
	
	texturePath = sourcePath;

	return textureBufferId != 0;
}

/* Returns whether the Material has a loaded texture and a linked shader */
bool Material::IsValid()
{
	return textureBufferId != 0 && shader->IsValid();
}

/* Attemps to link the shader, if a valid texture has been set */
bool Material::Apply()
{
	if (textureBufferId == 0)
		return false;

	if (!shader->LinkShaderProgram())
		return false;

	return true;
}

/* Reloads the vertex shader, fragment shader and texture, recompiles the shaders and relinks the shader program */
bool Material::Reapply()
{
	if (SetVertexShaderPath(vertexShaderPath))
		if (SetFragmentShaderPath(fragmentShaderPath))
			if (SetTexturePath(texturePath))
				return Apply();
	
	return false;
}

/* Draws a certain model using the Material's shader and texture */
bool Material::DrawArray(float* modelMatrix, uint drawDataBufferId, uint vertexCount)
{
	if (!IsValid())
		return false;

	shader->Activate();

	GLint modelLoc = glGetUniformLocation(shader->GetProgramId(), "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);

	GLint viewLoc = glGetUniformLocation(shader->GetProgramId(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->editorCamera->GetViewMatrix());

	GLint projLoc = glGetUniformLocation(shader->GetProgramId(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, App->editorCamera->GetProjectionMatrix());

	glBindTexture(GL_TEXTURE_2D, textureBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, drawDataBufferId);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	shader->Deactivate();
	return true;
}
