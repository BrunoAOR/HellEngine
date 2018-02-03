#include "ImGui/imgui.h"
#include "Application.h"
#include "Material.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "openGL.h"


Material::Material(const std::string& aName)
{
	name = aName;
	shader = new Shader();
}


Material::~Material()
{
	delete shader;
	shader = nullptr;

	glDeleteTextures(1, &textureBufferId);
	textureBufferId = 0;
}

const std::string& Material::getName() const
{
	return name;
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

bool Material::SetShaderDataPath(const std::string & sourcePath)
{
	std::string shaderRawData;
	if (!LoadTextFile(sourcePath, shaderRawData))
		return false;

	shaderDataPath = sourcePath;
	shaderData = shaderRawData;
	return true;
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

	if (!GenerateUniforms())
		return false;

	return true;
}

/* Reloads the vertex shader, fragment shader and texture, recompiles the shaders and relinks the shader program */
bool Material::Reapply()
{
	if (SetVertexShaderPath(vertexShaderPath))
		if (SetFragmentShaderPath(fragmentShaderPath))
			if (SetTexturePath(texturePath))
				if (shaderDataPath == "" || SetShaderDataPath(shaderDataPath))
					return Apply();
	
	return false;
}

void Material::ShowGUIMenu()
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	ImGui::Text(" options:");
	ImGui::Text("");

	if (ImGui::Button("Reapply"))
		Reapply();

	if (publicUniforms.size() == 0)
	{
		ImGui::Text("This Material has no configuration options.");
	}
	
	for (Uniform& uniform : publicUniforms)
	{
		switch (uniform.type)
		{
		case Uniform::UniformType::FLOAT:
			ImGui::DragFloat(uniform.name.c_str(), uniform.values, 0.01f, 0.0f, 1.0f, "%.2f");
			break;
		case Uniform::UniformType::COLOR4:
			ImGui::ColorEdit4(uniform.name.c_str(), uniform.values);
			break;
		default:
			break;
		}
	}
}

/* Draws a certain model using the Material's shader and texture */
bool Material::DrawArray(float* modelMatrix, uint vao, uint vertexCount)
{
	if (!IsValid())
		return false;

	shader->Activate();

	glUniformMatrix4fv(privateUniforms["model_matrix"], 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(privateUniforms["view"], 1, GL_FALSE, App->editorCamera->GetViewMatrix());
	glUniformMatrix4fv(privateUniforms["projection"], 1, GL_FALSE, App->editorCamera->GetProjectionMatrix());
	UpdatePublicUniforms();

	glBindTexture(GL_TEXTURE_2D, textureBufferId);
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);

	shader->Deactivate();
	return true;
}

bool Material::DrawElements(float * modelMatrix, uint vao, uint vertexCount, int indexesType)
{
	if (!IsValid())
		return false;

	shader->Activate();

	glUniformMatrix4fv(privateUniforms["model_matrix"], 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(privateUniforms["view"], 1, GL_FALSE, App->editorCamera->GetViewMatrix());
	glUniformMatrix4fv(privateUniforms["projection"], 1, GL_FALSE, App->editorCamera->GetProjectionMatrix());
	UpdatePublicUniforms();

	glBindTexture(GL_TEXTURE_2D, textureBufferId);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, vertexCount, indexesType, nullptr);
	glBindVertexArray(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);

	shader->Deactivate();
	return true;
}

bool Material::GenerateUniforms()
{
	/* Gets called AFTER successfully linking the shader program */
	privateUniforms.clear();
	privateUniforms["model_matrix"] = glGetUniformLocation(shader->GetProgramId(), "model_matrix");
	privateUniforms["view"] = glGetUniformLocation(shader->GetProgramId(), "view");
	privateUniforms["projection"] = glGetUniformLocation(shader->GetProgramId(), "projection");

	publicUniforms.clear();
	if (shaderDataPath == "")
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

void Material::UpdatePublicUniforms()
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
