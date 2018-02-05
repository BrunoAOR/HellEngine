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
	checkeredPatternBufferId = CreateCheckeredTexture();
	shader = new Shader();
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentMaterial::~ComponentMaterial()
{
	delete shader;
	shader = nullptr;

	glDeleteTextures(1, &checkeredPatternBufferId);
	checkeredPatternBufferId = 0;
	glDeleteTextures(1, &textureBufferId);
	textureBufferId = 0;

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

	if (!isValid || !mesh || !transform)
	{
		return;
	}

	float* modelMatrix = transform->GetModelMatrix();

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
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(vertexShaderPath, 256, sourcePath.c_str(), sourcePath.length());
	vertexShaderPath[sourcePath.length() + 1] = '\0';

	return true;
}

/* Recieves the fragment shader file path and tries to compile it */
bool ComponentMaterial::SetFragmentShaderPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(fragmentShaderPath, 256, sourcePath.c_str(), sourcePath.length());
	fragmentShaderPath[sourcePath.length() + 1] = '\0';

	return true;
}

/* Recieves the texture file path and tries to load it using Devil */
bool ComponentMaterial::SetTexturePath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(texturePath, 256, sourcePath.c_str(), sourcePath.length());
	texturePath[sourcePath.length() + 1] = '\0';

	return true;
}

bool ComponentMaterial::SetShaderDataPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(shaderDataPath, 256, sourcePath.c_str(), sourcePath.length());
	shaderDataPath[sourcePath.length() + 1] = '\0';

	return true;
}

/* Returns whether the Material has a loaded texture and a linked shader with valid data */
bool ComponentMaterial::IsValid()
{
	return isValid;
}

/* Attemps to apply all of the material setup */
bool ComponentMaterial::Apply()
{
	isValid = (LoadVertexShader()
		&& LoadFragmentShader()
		&& LoadShaderData()
		&& LoadTexture()
		&& shader->LinkShaderProgram()
		&& GenerateUniforms());

	return isValid;
}

void ComponentMaterial::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		ImGui::Checkbox("Active", &isActive);
		ImGui::Text("Setup:");
		if (ImGui::Button("Use defaults"))
		{
			memcpy_s(vertexShaderPath, 256, "assets/shaders/defaultShader.vert", 256);
			memcpy_s(fragmentShaderPath, 256, "assets/shaders/defaultShader.frag", 256);
			shaderDataPath[0] = '\0';
			shaderData = "";
			texturePath[0] = '\0';
		}
		ImGui::InputText("Vertex shader", vertexShaderPath, 256);
		ImGui::InputText("Fragment shader", fragmentShaderPath, 256);
		ImGui::InputText("Shader data", shaderDataPath, 256);
		ImGui::InputText("Texture path", texturePath, 256);
		if (ImGui::Button("Apply"))
			Apply();

		ImGui::NewLine();

		if (!isValid)
		{
			ImGui::Text("Invalid Material setup detected!");
		}
		else
		{
			ImGui::Text("Options:");
			if (publicUniforms.size() == 0)
			{
				ImGui::TextWrapped("The shader data has no configuration options.");
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

uint ComponentMaterial::CreateCheckeredTexture()
{
	static const int checkeredTextureSize = 64;
	GLubyte checkImage[checkeredTextureSize][checkeredTextureSize][4];
	for (int i = 0; i < checkeredTextureSize; i++) {
		for (int j = 0; j < checkeredTextureSize; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	GLuint textureId;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkeredTextureSize, checkeredTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return textureId;
}

bool ComponentMaterial::GenerateUniforms()
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

bool ComponentMaterial::LoadVertexShader()
{
	std::string vertexString;
	if (!LoadTextFile(vertexShaderPath, vertexString))
		return false;

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;

	return true;
}

bool ComponentMaterial::LoadFragmentShader()
{
	std::string fragmentString;
	if (!LoadTextFile(fragmentShaderPath, fragmentString))
		return false;

	if (!shader->CompileFragmentShader(fragmentString.c_str()))
		return false;

	return true;
}

bool ComponentMaterial::LoadShaderData()
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

bool ComponentMaterial::LoadTexture()
{
	if (textureBufferId != checkeredPatternBufferId)
		glDeleteTextures(1, &textureBufferId);

	if (IsEmptyString(texturePath))
		textureBufferId = checkeredPatternBufferId;
	else
		textureBufferId = App->renderer->LoadImageWithDevIL(texturePath);

	return textureBufferId != 0;
}

