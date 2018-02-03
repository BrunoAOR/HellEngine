#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "globals.h"
#include "openGL.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIAL;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	shader = new Shader();
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentMaterial::~ComponentMaterial()
{
	delete shader;
	shader = nullptr;
	LOGGER("Deleting Component of type '%s'", GetString(type));
}

void ComponentMaterial::Update()
{
	if (!isActive)
		return;

	if (mesh == nullptr)
	{
		std::vector<Component*> meshes = gameObject->GetComponents(ComponentType::MESH);
		if (meshes.size() == 0)
		{
			return;
		}
		mesh = (ComponentMesh*)meshes[0];
	}

	if (transform == nullptr)
	{
		std::vector<Component*> transforms = gameObject->GetComponents(ComponentType::TRANSFORM);
		if (transforms.size() == 0)
		{
			return;
		}
		transform = (ComponentTransform*)transforms[0];
	}

	if (!IsValid())
	{
		return;
	}

	float* modelMatrix = transform->GetModelMatrix();

	/*float modelMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 4, 0, 1
	};*/

	ComponentMesh::VaoInfo vaoInfo = mesh->getActiveVao();
	if (vaoInfo.vao == 0)
	{
		return;
	}

	DrawElements(modelMatrix, vaoInfo.vao, vaoInfo.elementsCount, vaoInfo.indexesType);
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
	return transform && mesh && textureBufferId != 0 && shader->IsValid();
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
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		ImGui::Text("Material GUI goes here");

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
}

/* Draws a certain model using the Material's shader and texture */
bool ComponentMaterial::DrawArray(float* modelMatrix, uint vao, uint vertexCount)
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

bool ComponentMaterial::DrawElements(float * modelMatrix, uint vao, uint vertexCount, int indexesType)
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

bool ComponentMaterial::GenerateUniforms()
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

void ComponentMaterial::UpdatePublicUniforms()
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

