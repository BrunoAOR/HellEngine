#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "Shader.h"
#include "openGL.h"

ModuleDebugDraw::ModuleDebugDraw()
{
}

ModuleDebugDraw::~ModuleDebugDraw()
{
}

/* Applies the default material configuration */
void ModuleDebugDraw::SetDefaultMaterialConfiguration()
{
	memcpy_s(vertexShaderPath, 256, "assets/shaders/debugShader.vert", 256);
	memcpy_s(fragmentShaderPath, 256, "assets/shaders/debugShader.frag", 256);
	shaderDataPath[0] = '\0';
	shaderData = "";
	texturePath[0] = '\0';
}

bool ModuleDebugDraw::Init()
{
	bool success = true;
	shader = new Shader();	
	SetDefaultMaterialConfiguration();

	success &= LoadVertexShader();
	success &= LoadFragmentShader();
	success &= shader->LinkShaderProgram();
	success &= GenerateUniforms();

	return success;
}

bool ModuleDebugDraw::CleanUp()
{
	delete shader;
	shader = nullptr;
	return true;
}


bool ModuleDebugDraw::DrawElements(float * modelMatrix, uint vao, uint vertexCount)
{
	shader->Activate();

	glUniformMatrix4fv(privateUniforms["model_matrix"], 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(privateUniforms["view"], 1, GL_FALSE, App->editorCamera->camera->GetViewMatrix());
	glUniformMatrix4fv(privateUniforms["projection"], 1, GL_FALSE, App->editorCamera->camera->GetProjectionMatrix());
	UpdatePublicUniforms();
	
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, vertexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(GL_NONE);
	
	shader->Deactivate();
	return true;
}

bool ModuleDebugDraw::GenerateUniforms()
{
	/* Gets called AFTER successfully linking the shader program */
	privateUniforms.clear();
	privateUniforms["model_matrix"] = glGetUniformLocation(shader->GetProgramId(), "model_matrix");
	privateUniforms["view"] = glGetUniformLocation(shader->GetProgramId(), "view");
	privateUniforms["projection"] = glGetUniformLocation(shader->GetProgramId(), "projection");

	publicUniforms.clear();
	if (IsEmptyString(shaderDataPath))
		return true;

	std::string rest(shaderData);
	std::string pair;
	std::string type;
	std::string name;

	while (rest.length() > 0)
	{
		int pairSplitIndex = rest.find('\n');
		pair = rest.substr(0, pairSplitIndex);
		rest = rest.substr(pairSplitIndex + 1, rest.length());

		int wordSplitIndex = pair.find(' ');
		type = pair.substr(0, wordSplitIndex);
		name = pair.substr(wordSplitIndex + 1, pair.length());

		Uniform uniform;
		uniform.name = name;
		if (type == "float")
		{
			uniform.type = Uniform::UniformType::FLOAT;
			uniform.size = 1;
		}
		else if (type == "color4")
		{
			uniform.type = Uniform::UniformType::COLOR4;
			uniform.size = 4;
		}

		uniform.values[0] = 0.0f;
		uniform.values[1] = 0.0f;
		uniform.values[2] = 0.0f;
		uniform.values[3] = 1.0f;

		uniform.location = glGetUniformLocation(shader->GetProgramId(), uniform.name.c_str());
		if (uniform.location == -1)
			return false;

		publicUniforms.push_back(uniform);
	}

	return true;
}

void ModuleDebugDraw::UpdatePublicUniforms()
{
	for (const Uniform& uniform : publicUniforms)
	{
		switch (uniform.type)
		{
		case Uniform::UniformType::FLOAT:
			glUniform1fv(uniform.location, 1, uniform.values);
			break;
		case Uniform::UniformType::COLOR4:
			glUniform4fv(uniform.location, 1, uniform.values);
			break;
		default:
			break;
		}
	}
}

bool ModuleDebugDraw::LoadVertexShader()
{
	std::string vertexString;
	if (!LoadTextFile(vertexShaderPath, vertexString))
		return false;

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;

	return true;
}

bool ModuleDebugDraw::LoadFragmentShader()
{
	std::string fragmentString;
	if (!LoadTextFile(fragmentShaderPath, fragmentString))
		return false;

	if (!shader->CompileFragmentShader(fragmentString.c_str()))
		return false;

	return true;
}

bool ModuleDebugDraw::LoadShaderData()
{
	shaderData == "";
	if (IsEmptyString(shaderDataPath))
		return true;

	std::string shaderRawData;
	if (!LoadTextFile(shaderDataPath, shaderRawData))
		return false;

	shaderData = shaderRawData;
	return true;
}