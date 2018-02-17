#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Shader.h"
#include "globals.h"
#include "openGL.h"

uint ComponentMaterial::checkeredPatternBufferId = 0;
uint ComponentMaterial::checkeredTextureCount = 0;
std::vector<Shader*> ComponentMaterial::loadedShaders;
std::map<Shader*, uint> ComponentMaterial::loadedShaderCount;

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIAL;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	if (checkeredPatternBufferId == 0) {
		checkeredPatternBufferId = CreateCheckeredTexture();
		checkeredTextureCount++;
	}

	//LOGGER("Component of type '%s'", GetString(type));
}

ComponentMaterial::~ComponentMaterial()
{
	if (loadedShaderCount.at(shader) == 1) {
		delete shader;
		loadedShaderCount.erase(shader);
		shader = nullptr;
	}
	else {
		loadedShaderCount.at(shader)--;
	}

	if (textureBufferId == checkeredPatternBufferId)
		checkeredTextureCount--;
	else
		glDeleteTextures(1, &textureBufferId);

	textureBufferId = 0;

	if (checkeredTextureCount == 0) {
		glDeleteTextures(1, &checkeredPatternBufferId);
		checkeredPatternBufferId = 0;
	}

	//LOGGER("Deleting Component of type '%s'", GetString(type));
}

void ComponentMaterial::Update()
{
	BROFILER_CATEGORY("ComponentMaterial::UpdateStart", Profiler::Color::Gold);
	if (!isActive)
		return;

	ComponentMesh* mesh = (ComponentMesh*)gameObject->GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);;

	if (!isValid || !mesh || !transform)
	{
		return;
	}

	bool insideFrustum = true;

	ComponentCamera* editorCamera = App->editorCamera->camera;
	if (editorCamera != nullptr)
	{
		if (App->scene->UsingQuadTree() && transform->GetIsStatic()) {
			BROFILER_CATEGORY("Frustum culling using QuadTree", Profiler::Color::GreenYellow);
			insideFrustum = editorCamera->IsInsideFrustum(gameObject);
		}
		else {
			BROFILER_CATEGORY("Frustum culling without QuadTree", Profiler::Color::GreenYellow);
			insideFrustum = transform->GetBoundingBox().Contains(editorCamera->GetFrustum());
		}
	}
	if (insideFrustum) {

		ComponentCamera* activeGameCamera = App->scene->GetActiveGameCamera();

		if (activeGameCamera != nullptr && activeGameCamera->FrustumCulling())
		{
			if (App->scene->UsingQuadTree() && transform->GetIsStatic()) {
				insideFrustum = activeGameCamera->IsInsideFrustum(gameObject);
			}
			else {
				insideFrustum = transform->GetBoundingBox().Contains(activeGameCamera->GetFrustum());
			}
		}

		if (insideFrustum) {
			float* modelMatrix = transform->GetModelMatrix();

			BROFILER_CATEGORY("ComponentMaterial::GetVao", Profiler::Color::Gold);
			if (mesh->activeVaoChanged) {
				vaoInfo = mesh->GetActiveVao();
				mesh->activeVaoChanged = false;
			}
			BROFILER_CATEGORY("ComponentMaterial::ValidVao", Profiler::Color::Gold);
			if (vaoInfo.vao == 0)
			{
				return;
			}

			BROFILER_CATEGORY("ComponentMaterial::DrawingCall", Profiler::Color::Gold);
			DrawElements(modelMatrix, vaoInfo.vao, vaoInfo.elementsCount);
		}
	}
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
	if (shader != nullptr)
		loadedShaderCount.at(shader)--;

	Shader* s = ShaderAlreadyLinked();

	if (s) {
		shader = s;
		loadedShaderCount.at(shader)++;
		isValid = true;
	}
	else {
		shader = new Shader();

		isValid = (LoadVertexShader()
			&& LoadFragmentShader()
			&& shader->LinkShaderProgram()
			&& GenerateUniforms());

		if (isValid) {
			loadedShaders.push_back(shader);
			loadedShaderCount.insert(std::pair<Shader*, int>(shader, 1));
		}
		else {
			delete shader;
			shader = nullptr;
		}
	}

	isValid &= LoadTexture()
		&& LoadShaderData()
		&& GenerateUniforms();

	return isValid;
}

/* Applies the default material configuration */
void ComponentMaterial::SetDefaultMaterialConfiguration()
{
	memcpy_s(vertexShaderPath, 256, "assets/shaders/defaultShader.vert", 256);
	memcpy_s(fragmentShaderPath, 256, "assets/shaders/defaultShader.frag", 256);
	shaderDataPath[0] = '\0';
	shaderData = "";
	texturePath[0] = '\0';
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
	return 1;
}

void ComponentMaterial::OnEditorMaterialConfiguration()
{
	static const std::string label = std::string("Material setup##MatSetup") + std::to_string(editorInfo.id);
	if (ImGui::TreeNode(label.c_str()))
	{
		if (ImGui::Button("Use defaults"))
			SetDefaultMaterialConfiguration();

		ImGui::InputText("Vertex shader", vertexShaderPath, 256);
		ImGui::InputText("Fragment shader", fragmentShaderPath, 256);
		ImGui::InputText("Shader data", shaderDataPath, 256);
		ImGui::InputText("Texture path", texturePath, 256);

		if (ImGui::Button("Apply"))
			Apply();

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentMaterial::OnEditorTextureInformation()
{
	static const std::string label = std::string("Textures information##TextureInfo") + std::to_string(editorInfo.id);
	if (ImGui::TreeNode(label.c_str()))
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

	static const std::string label = std::string("Textures configuration##TextureConfig") + std::to_string(editorInfo.id);
	if (ImGui::TreeNode(label.c_str()))
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
			ConfigureTexture();
		}

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentMaterial::OnEditorShaderOptions()
{
	static const std::string label = std::string("Shader options##ShaderOptions") + std::to_string(editorInfo.id);
	if (ImGui::TreeNode(label.c_str()))
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

Shader * ComponentMaterial::ShaderAlreadyLinked()
{
	Shader* s = nullptr;
	for (std::vector<Shader*>::iterator it = loadedShaders.begin(); it != loadedShaders.end() && s == nullptr; ++it) {
		if ((strcmp(vertexShaderPath, (*it)->vertexPath) == 0) && (strcmp(fragmentShaderPath, (*it)->fragmentPath) == 0)) {
			s = *it;
			break;
		}
	}

	return s;
}

bool ComponentMaterial::DrawElements(float * modelMatrix, uint vao, uint vertexCount)
{
	if (!IsValid())
		return false;

	shader->Activate();

	glUniformMatrix4fv(privateUniforms["model_matrix"], 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(privateUniforms["view"], 1, GL_FALSE, App->editorCamera->camera->GetViewMatrix());
	glUniformMatrix4fv(privateUniforms["projection"], 1, GL_FALSE, App->editorCamera->camera->GetProjectionMatrix());
	UpdatePublicUniforms();

	glBindTexture(GL_TEXTURE_2D, textureBufferId);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, nullptr);
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

	strncpy_s(shader->vertexPath, sizeof(shader->vertexPath), vertexShaderPath, sizeof(vertexShaderPath));

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;

	return true;
}

bool ComponentMaterial::LoadFragmentShader()
{
	std::string fragmentString;
	if (!LoadTextFile(fragmentShaderPath, fragmentString))
		return false;

	strncpy_s(shader->fragmentPath, sizeof(shader->fragmentPath), fragmentShaderPath, sizeof(fragmentShaderPath));

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
	{
		textureBufferId = checkeredPatternBufferId;
		textureInfo.Zero();
		checkeredTextureCount++;
	}
	else
		textureBufferId = App->renderer->LoadImageWithDevIL(texturePath, &textureInfo);

	if (textureBufferId != 0)
		ConfigureTexture();

	return textureBufferId != 0;
}

void ComponentMaterial::ConfigureTexture()
{
	glBindTexture(GL_TEXTURE_2D, textureBufferId);

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
		//glGenerateTextureMipmap(textureBufferId);	
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
