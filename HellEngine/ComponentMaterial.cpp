#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentType.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "openGL.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIAL;
}


ComponentMaterial::~ComponentMaterial()
{
}

/* Recieves the vertex shader file path and tries to compile it */
bool ComponentMaterial::SetVertexShaderPath(const std::string& sourcePath)
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
bool ComponentMaterial::SetFragmentShaderPath(const std::string& sourcePath)
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
bool ComponentMaterial::SetTexturePath(const std::string& sourcePath)
{
	glDeleteTextures(1, &textureBufferId);

	textureBufferId = App->renderer->LoadImageWithDevIL(sourcePath.c_str());

	if (textureBufferId == 0)
		return false;

	texturePath = sourcePath;

	return textureBufferId != 0;
}

bool ComponentMaterial::SetShaderDataPath(const std::string & sourcePath)
{
	std::string shaderRawData;
	if (!LoadTextFile(sourcePath, shaderRawData))
		return false;

	shaderDataPath = sourcePath;
	shaderData = shaderRawData;
	return true;
}

/* Returns whether the Material has a loaded texture and a linked shader */
bool ComponentMaterial::IsValid()
{
	return textureBufferId != 0 && shader->IsValid();
}

/* Attemps to link the shader, if a valid texture has been set */
bool ComponentMaterial::Apply()
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
bool ComponentMaterial::Reapply()
{
	if (SetVertexShaderPath(vertexShaderPath))
		if (SetFragmentShaderPath(fragmentShaderPath))
			if (SetTexturePath(texturePath))
				if (shaderDataPath == "" || SetShaderDataPath(shaderDataPath))
					return Apply();

	return false;
}

void ComponentMaterial::OnEditor()
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	ImGui::Text(" options:");
	ImGui::Text("");

	if (ImGui::Button("Reapply"))
		Reapply();

	if (uniforms.size() == 0)
	{
		ImGui::Text("This Material has no configuration options.");
	}

	for (Uniform& uniform : uniforms)
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
bool ComponentMaterial::DrawArray(float* modelMatrix, uint drawDataBufferId, uint vertexCount)
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

	UpdateUniforms();

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

bool ComponentMaterial::GenerateUniforms()
{
	uniforms.clear();
	if (shaderDataPath == "")
		return true;

	std::string rest(shaderData);
	std::string pair;
	std::string type;
	std::string name;

	while (rest.length() > 0)
	{
		int cut = rest.find('\n');
		pair = rest.substr(0, cut);
		rest = rest.substr(cut + 1, rest.length());

		int cut2 = pair.find(' ');
		type = pair.substr(0, cut2);
		name = pair.substr(cut2 + 1, pair.length());

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

		uniforms.push_back(uniform);
	}


	return true;
}

void ComponentMaterial::UpdateUniforms()
{
	for (const Uniform& uniform : uniforms)
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

