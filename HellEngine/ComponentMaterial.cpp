#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleShaderManager.h"
#include "ModuleTextureManager.h"
#include "SerializableObject.h"
#include "ShaderProgram.h"
#include "globals.h"
#include "openGL.h"

uint ComponentMaterial::materialsCount = 0;
uint ComponentMaterial::checkeredPatternBufferId = 0;
uint ComponentMaterial::defaultNormalMapBufferId = 0;
uint ComponentMaterial::defaultSpecularMapBufferId = 0;

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIAL;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	if (materialsCount == 0) {
		checkeredPatternBufferId = CreateCheckeredTexture();
		defaultNormalMapBufferId = CreateDefaultNormalMap();
		defaultSpecularMapBufferId = CreateDefaultSpecularMap();
	}
	materialsCount++;

	//LOGGER("Component of type '%s'", GetString(type));
}

ComponentMaterial::~ComponentMaterial()
{
	if (shaderProgram)
	{
		App->shaderManager->ReleaseShaderProgram(shaderProgram);
		shaderProgram = nullptr;
	}

	if (diffuseBufferId != checkeredPatternBufferId)
		App->textureManager->ReleaseTexture(diffuseBufferId);
	diffuseBufferId = 0;

	if (normalBufferId != defaultNormalMapBufferId)
		App->textureManager->ReleaseTexture(normalBufferId);
	normalBufferId = 0;

	if (specularBufferId != defaultSpecularMapBufferId)
		App->textureManager->ReleaseTexture(specularBufferId);
	specularBufferId = 0;

	materialsCount--;

	if (materialsCount == 0) {
		glDeleteTextures(1, &checkeredPatternBufferId);
		checkeredPatternBufferId = 0;
		glDeleteTextures(1, &defaultNormalMapBufferId);
		defaultNormalMapBufferId = 0;
		glDeleteTextures(1, &defaultSpecularMapBufferId);
		defaultSpecularMapBufferId = 0;
	}

	//LOGGER("Deleting Component of type '%s'", GetString(type));
}

/* Recieves the vertex shader file path and tries to compile it */
bool ComponentMaterial::SetVertexShaderPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(vertexShaderPath, 256, sourcePath.c_str(), sourcePath.length());
	vertexShaderPath[sourcePath.length()] = '\0';

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
	fragmentShaderPath[sourcePath.length()] = '\0';

	return true;
}

/* Recieves the texture file path and tries to load it using Devil */
bool ComponentMaterial::SetTexturePath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(diffusePath, 256, sourcePath.c_str(), sourcePath.length());
	diffusePath[sourcePath.length()] = '\0';

	return true;
}

bool ComponentMaterial::SetNormalPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(normalPath, 256, sourcePath.c_str(), sourcePath.length());
	normalPath[sourcePath.length()] = '\0';

	return true;
}

bool ComponentMaterial::SetSpecularPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(specularPath, 256, sourcePath.c_str(), sourcePath.length());
	specularPath[sourcePath.length()] = '\0';

	return true;
}

bool ComponentMaterial::SetShaderDataPath(const std::string& sourcePath)
{
	if (sourcePath.length() > 255)
	{
		return false;
	}

	memcpy_s(shaderDataPath, 256, sourcePath.c_str(), sourcePath.length());
	shaderDataPath[sourcePath.length()] = '\0';

	return true;
}

/* Returns whether the Material has a loaded texture and a linked shader with valid data */
bool ComponentMaterial::IsValid() const
{
	return isValid;
}

/* Attemps to apply all of the material setup */
bool ComponentMaterial::Apply()
{
	return Apply(ShaderOptions::DEFAULT);
}

bool ComponentMaterial::Apply(ShaderOptions shaderOptions)
{
	/*
	The new shaderProgramId is request before releasing the previous one in case we are requesting for the same program.
	If this were the case, and this ComponentMaterial were the only user of the program,
	releasing the program first would cause it to get destroyed and then rebuilt when requesting it anew.
	*/
	const ShaderProgram* oldShaderProgram = shaderProgram;
	shaderProgram = App->shaderManager->GetShaderProgram(vertexShaderPath, fragmentShaderPath, shaderOptions);
	App->shaderManager->ReleaseShaderProgram(oldShaderProgram);
	oldShaderProgram = nullptr;

	isValid = shaderProgram
		&& LoadTexture(diffuseBufferId, diffusePath, checkeredPatternBufferId)
		&& LoadTexture(normalBufferId, normalPath, defaultNormalMapBufferId)
		&& LoadTexture(specularBufferId, specularPath, defaultSpecularMapBufferId)
		&& LoadShaderData()
		&& GenerateUniforms();

	return isValid;
}

/* Applies the default material configuration */
void ComponentMaterial::SetDefaultMaterialConfiguration()
{
	memcpy_s(vertexShaderPath, 256, "assets/shaders/uberShader.vert", 256);
	memcpy_s(fragmentShaderPath, 256, "assets/shaders/uberShader.frag", 256);
	shaderDataPath[0] = '\0';
	shaderData = "";
	diffusePath[0] = '\0';
	normalPath[0] = '\0';
	specularPath[0] = '\0';
}

void ComponentMaterial::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);

		OnEditorMaterialConfiguration();

		if (!isValid)
		{
			ImGui::Text("Invalid Material setup detected!");
		}
		else
		{
			OnEditorTextureInformation();
			OnEditorTextureConfiguration();
			OnEditorShaderOptions();
		}
	}
}

int ComponentMaterial::MaxCountInGameObject()
{
	return -1;
}

void ComponentMaterial::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	obj.AddInt("ModelInfoVaoIndex", modelInfoVaoIndex);
	obj.AddString("VertexShaderPath", vertexShaderPath);
	obj.AddString("FragmentShaderPath", fragmentShaderPath);
	obj.AddString("DiffuseTexturePath", diffusePath);
	obj.AddString("NormalTexturePath", normalPath);
	obj.AddString("ShaderDataPath", shaderDataPath);
	obj.AddInt("WrapMode", textureConfiguration.wrapMode);
	obj.AddBool("MipMaps", textureConfiguration.mipMaps);
	obj.AddInt("MinificationMode", textureConfiguration.minificationMode);
	obj.AddInt("MagnificationMode", textureConfiguration.magnificationMode);
	obj.AddInt("ShaderOptions", shaderProgram ? static_cast<int>(shaderProgram->GetShaderOptions()) : 0);
}

void ComponentMaterial::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	modelInfoVaoIndex = obj.GetInt("ModelInfoVaoIndex");
	
	std::string objVertexShaderPath = obj.GetString("VertexShaderPath");
	memcpy_s(vertexShaderPath, 256, objVertexShaderPath.c_str(), objVertexShaderPath.length());
	vertexShaderPath[objVertexShaderPath.length()] = '\0';
	
	std::string objFragmentShaderPath = obj.GetString("FragmentShaderPath");
	memcpy_s(fragmentShaderPath, 256, objFragmentShaderPath.c_str(), objFragmentShaderPath.length());
	fragmentShaderPath[objFragmentShaderPath.length()] = '\0';

	std::string objTexturePath = obj.GetString("DiffuseTexturePath");
	memcpy_s(diffusePath, 256, objTexturePath.c_str(), objTexturePath.length());
	diffusePath[objTexturePath.length()] = '\0';

	objTexturePath = obj.GetString("NormalTexturePath");
	memcpy_s(normalPath, 256, objTexturePath.c_str(), objTexturePath.length());
	normalPath[objTexturePath.length()] = '\0';

	std::string objShaderDataPath = obj.GetString("ShaderDataPath");
	memcpy_s(shaderDataPath, 256, objShaderDataPath.c_str(), objShaderDataPath.length());
	shaderDataPath[objShaderDataPath.length()] = '\0';

	textureConfiguration.wrapMode = obj.GetInt("WrapMode");
	textureConfiguration.mipMaps = obj.GetBool("MipMaps");
	textureConfiguration.minificationMode = obj.GetInt("MinificationMode");
	textureConfiguration.magnificationMode = obj.GetInt("MagnificationMode");
	
	int shaderOptionsCode = obj.GetInt("ShaderOptions");
	
	Apply(static_cast<ShaderOptions>(shaderOptionsCode));
}

void ComponentMaterial::OnEditorMaterialConfiguration()
{
	static const std::string baseLabel = std::string("Material setup##MatSetup");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
		if (ImGui::Button("Use defaults"))
			SetDefaultMaterialConfiguration();

		ImGui::InputText("Vertex shader", vertexShaderPath, 256);
		ImGui::InputText("Fragment shader", fragmentShaderPath, 256);
		ImGui::InputText("Shader data", shaderDataPath, 256);
		ImGui::InputText("Texture path", diffusePath, 256);
		ImGui::InputText("Normal Texture path", normalPath, 256);
		ImGui::InputText("Specular Texture path", specularPath, 256);

		if (ImGui::Button("Apply"))
			Apply();

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentMaterial::OnEditorTextureInformation()
{
	static const std::string baseLabel = std::string("Textures information##TextureInfo");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
		uint w = textureInfo.width;
		uint h = textureInfo.height;
		uint bytesPerPixel = textureInfo.bytesPerPixel;
		uint wInBytes = w * bytesPerPixel;
		uint hInBytes = h * bytesPerPixel;

		ImGui::Text("Image width (pixels) = "); ImGui::SameLine();
		if (w != 0)
			ImGui::Text(std::to_string(w).c_str());
		else
			ImGui::Text("NA");

		ImGui::Text("Image height (pixels) = "); ImGui::SameLine();
		if (h != 0)
			ImGui::Text(std::to_string(h).c_str());
		else
			ImGui::Text("NA");


		ImGui::Text("Bytes per pixel = "); ImGui::SameLine();
		if (bytesPerPixel != 0)
			ImGui::Text(std::to_string(bytesPerPixel).c_str());
		else
			ImGui::Text("NA");


		ImGui::Text("Image width (bytes) = "); ImGui::SameLine();
		if (wInBytes != 0)
			ImGui::Text(std::to_string(wInBytes).c_str());
		else
			ImGui::Text("NA");


		ImGui::Text("Image height (bytes) = "); ImGui::SameLine();
		if (hInBytes != 0)
			ImGui::Text(std::to_string(hInBytes).c_str());
		else
			ImGui::Text("NA");

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentMaterial::OnEditorTextureConfiguration()
{
	static bool changed = false;
	static char* minificationOptions;

	static const std::string baseLabel = std::string("Textures configuration##TextureConfig");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
		if (ImGui::Combo("Wrap mode", &textureConfiguration.wrapMode, "GL_REPEAT\0GL_MIRRORED_REPEAT\0GL_CLAMP_TO_EDGE\0GL_CLAMP\0\0"))
			changed = true;

		if (ImGui::Checkbox("Mipmaps", &textureConfiguration.mipMaps))
			changed = true;

		if (textureConfiguration.mipMaps)
			minificationOptions = "GL_NEAREST\0GL_LINEAR\0GL_NEAREST_MIPMAP_NEAREST\0GL_LINEAR_MIPMAP_NEAREST\0GL_NEAREST_MIPMAP_LINEAR\0GL_LINEAR_MIPMAP_LINEAR\0\0";
		else
			minificationOptions = "GL_NEAREST\0GL_LINEAR\0\0";

		if (ImGui::Combo("Minification", &textureConfiguration.minificationMode, minificationOptions)) {
			changed = true;
		}

		if (ImGui::Combo("Magnification", &textureConfiguration.magnificationMode, "GL_NEAREST\0GL_LINEAR\0\0")) {
			changed = true;
		}

		if (changed)
		{
			changed = false;
			ConfigureTexture(diffuseBufferId);
		}

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentMaterial::OnEditorShaderOptions()
{
	static const std::string baseLabel = std::string("Shader options##ShaderOptions");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
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

		ImGui::NewLine();
		ImGui::TreePop();
	}
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

uint ComponentMaterial::CreateDefaultNormalMap()
{
	static const int normalMapSize = 8;
	GLubyte normalMap[normalMapSize][normalMapSize][4];
	for (int i = 0; i < normalMapSize; i++) {
		for (int j = 0; j < normalMapSize; j++) {
			normalMap[i][j][0] = (GLubyte)127;
			normalMap[i][j][1] = (GLubyte)127;
			normalMap[i][j][2] = (GLubyte)255;
			normalMap[i][j][3] = (GLubyte)127;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normalMapSize, normalMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalMap);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return textureId;
}

uint ComponentMaterial::CreateDefaultSpecularMap()
{
	static const int normalMapSize = 8;
	GLubyte normalMap[normalMapSize][normalMapSize][4];
	for (int i = 0; i < normalMapSize; i++) {
		for (int j = 0; j < normalMapSize; j++) {
			normalMap[i][j][0] = (GLubyte)255;
			normalMap[i][j][1] = (GLubyte)255;
			normalMap[i][j][2] = (GLubyte)255;
			normalMap[i][j][3] = (GLubyte)255;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normalMapSize, normalMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalMap);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return textureId;
}

bool ComponentMaterial::GenerateUniforms()
{
	/* Gets called AFTER successfully linking the shader program */
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

		uniform.location = glGetUniformLocation(shaderProgram->GetProgramId(), uniform.name.c_str());
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

bool ComponentMaterial::LoadTexture(uint& bufferId, const char* path, uint fallbackDefaultId)
{
	if (bufferId != fallbackDefaultId)
		App->textureManager->ReleaseTexture(bufferId);

	if (IsEmptyString(path))
	{
		bufferId = fallbackDefaultId;
		textureInfo.Zero();
	}
	else
		bufferId = App->textureManager->GetTexture(path);

	if (bufferId != 0)
		ConfigureTexture(bufferId);

	return bufferId != 0;
}

void ComponentMaterial::ConfigureTexture(uint bufferId)
{
	glBindTexture(GL_TEXTURE_2D, bufferId);

	/* Set texture clamping method */
	switch (textureConfiguration.wrapMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	}

	/*Generate Mipmap from the current texture evaluated*/
	if (textureConfiguration.mipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
		//glGenerateTextureMipmap(diffuseBufferId);	
	}

	/*Apply magnification filters if requested*/
	switch (textureConfiguration.magnificationMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}
	/*Apply minification filters if requested*/
	switch (textureConfiguration.minificationMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 5:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
