#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Application.h"
#include "Billboard.h"
#include "ComponentGrass.h"
#include "ComponentType.h"
#include "ModuleEditorCamera.h"
#include "ModuleTextureManager.h"
#include "Shader.h"
#include "openGL.h"

std::vector<Shader*> ComponentGrass::loadedShaders;
std::map<Shader*, uint> ComponentGrass::loadedShaderCount;

ComponentGrass::ComponentGrass(GameObject* owner) : Component(owner)
{
	type = ComponentType::GRASS;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	CreateQuadVAO();
	previousCameraPos.x = FLT_MAX;
	previousCameraPos.y = FLT_MAX;
	previousCameraPos.z = FLT_MAX;
}

ComponentGrass::~ComponentGrass()
{
	if (!loadedShaderCount.empty())
	{
		if (loadedShaderCount.at(shader) == 1) {
			delete shader;
			loadedShaderCount.erase(shader);
			shader = nullptr;
		}
		else {
			loadedShaderCount.at(shader)--;
		}
	}

	if (textureBufferId != 0)
		App->textureManager->ReleaseTexture(textureBufferId);

	textureBufferId = 0;

	for (Billboard* billboard : billboards)
		delete billboard;

	billboards.clear();
}

void ComponentGrass::Update()
{
	if (isValid)
	{
		const float3& currentCameraPos = App->editorCamera->camera->GetPosition3();
		if (billboardsChanged || previousCameraPos.x != currentCameraPos.x || previousCameraPos.z != currentCameraPos.z)
		{
			previousCameraPos = currentCameraPos;

			const uint allVertCount = 6;
			const uint uniqueVertCount = 4;
			const uint billboardsSize = billboards.size();

			GLfloat* newVertQuadData = new GLfloat[uniqueVertCount * 3 * billboardsSize];

			for (uint i = 0; i < billboardsSize; ++i)
			{
				Billboard* billboard = billboards[i];

				const Quad& quad = billboard->ComputeQuad(App->editorCamera->camera);

				/* Quad vertices, color, and UV */
				for (uint j = 0; j < uniqueVertCount; ++j)
				{
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 0] = quad.vertices[j].x;
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 1] = quad.vertices[j].y;
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 2] = quad.vertices[j].z;
				}
			}

			glBindVertexArray(quadMeshInfo.vao);
			glBindBuffer(GL_ARRAY_BUFFER, quadMeshInfo.vbo);
			float* oldQuadData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			int j = 0;
			for (uint i = 0; i < uniqueVertCount * 8 * billboardsSize; ++i)
			{
				if (i % 8 == 0 || i % 8 == 1 || i % 8 == 2)
					oldQuadData[i] = newVertQuadData[j++];

			}

			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindVertexArray(GL_NONE);
			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

			delete[] newVertQuadData;
		}
		DrawElements();
	}
}

bool ComponentGrass::IsValid()
{
	return isValid;
}

bool ComponentGrass::Apply()
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

void ComponentGrass::SetDefaultMaterialConfiguration()
{
	memcpy_s(vertexShaderPath, 256, "assets/shaders/defaultShader.vert", 256);
	memcpy_s(fragmentShaderPath, 256, "assets/shaders/defaultShader.frag", 256);
	shaderDataPath[0] = '\0';
	shaderData = "";
	memcpy_s(texturePath, 256, "assets/images/grass.png", 256);
}

void ComponentGrass::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);

		OnEditorBillboardConfiguration();
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

int ComponentGrass::MaxCountInGameObject()
{
	return 1;
}

void ComponentGrass::CreateQuadVAO()
{
	billboardsChanged = true;
	for (Billboard* billboard : billboards)
		delete billboard;

	billboards.clear();

	billboards.reserve(billboardInstancesX * billboardInstancesZ);

	float mainXOffset = -(offsetX * ((billboardInstancesX - 1) / 2.0f));
	float mainZOffset = -(offsetZ * ((billboardInstancesZ - 1) / 2.0f));

	for (int i = 0; i < billboardInstancesX; ++i)
	{
		for (int j = 0; j < billboardInstancesZ; ++j)
		{
			Billboard* billboard = new Billboard();
			billboard->SetPosition(float3(position.x + offsetX * i + mainXOffset, position.y, position.z + offsetZ * j + mainZOffset));
			billboard->SetSize(width, height);
			billboards.push_back(billboard);
		}
	}

	GLfloat cWhite[3];

	GLfloat corners[8];

	{
		cWhite[0] = 1.0f;
		cWhite[1] = 1.0f;
		cWhite[2] = 1.0f;
	}

	/* UV coords */
	{
		corners[0] = 0.0f;
		corners[1] = 0.0f;

		corners[2] = 1.0f;
		corners[3] = 0.0f;

		corners[4] = 1.0f;
		corners[5] = 1.0f;

		corners[6] = 0.0f;
		corners[7] = 1.0f;
	}

	const uint allVertCount = 6;
	const uint uniqueVertCount = 4;
	const uint billboardsSize = billboards.size();

	GLfloat* uniqueQuadData = new GLfloat[uniqueVertCount * 8 * billboardsSize];
	GLuint* verticesOrder = new GLuint[allVertCount * billboardsSize];

	for (uint i = 0; i < billboardsSize; ++i)
	{
		Billboard* billboard = billboards[i];

		const Quad& quad = billboard->ComputeQuad(App->editorCamera->camera);

		/* Quad vertices, color, and UV */
		for (int j = 0; j < uniqueVertCount; ++j)
		{
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 0] = quad.vertices[j].x;
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 1] = quad.vertices[j].y;
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 2] = quad.vertices[j].z;

			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 3] = cWhite[0];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 4] = cWhite[1];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 5] = cWhite[2];

			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 6] = corners[2 * j];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 7] = corners[2 * j + 1];
		}

		verticesOrder[i * allVertCount + 0] = (i * uniqueVertCount) + 0;
		verticesOrder[i * allVertCount + 1] = (i * uniqueVertCount) + 1;
		verticesOrder[i * allVertCount + 2] = (i * uniqueVertCount) + 2;
		verticesOrder[i * allVertCount + 3] = (i * uniqueVertCount) + 0;
		verticesOrder[i * allVertCount + 4] = (i * uniqueVertCount) + 2;
		verticesOrder[i * allVertCount + 5] = (i * uniqueVertCount) + 3;
	}

	quadMeshInfo.name = "Quad";
	quadMeshInfo.elementsCount = allVertCount * billboardsSize;

	glGenVertexArrays(1, &quadMeshInfo.vao);
	glGenBuffers(1, &quadMeshInfo.vbo);
	glGenBuffers(1, &quadMeshInfo.ebo);

	glBindVertexArray(quadMeshInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, quadMeshInfo.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 8 * billboardsSize, uniqueQuadData, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMeshInfo.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * quadMeshInfo.elementsCount, verticesOrder, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	delete[] uniqueQuadData;
	delete[] verticesOrder;
}

void ComponentGrass::UpdateBillboards()
{
	billboardsChanged = true;
	assert(billboardInstancesX * billboardInstancesZ == billboards.size());

	float mainXOffset = -(offsetX * ((billboardInstancesX - 1) / 2.0f));
	float mainZOffset = -(offsetZ * ((billboardInstancesZ - 1) / 2.0f));

	for (int i = 0; i < billboardInstancesX; ++i)
	{
		for (int j = 0; j < billboardInstancesZ; ++j)
		{
			Billboard* billboard = billboards[i * billboardInstancesZ + j];
			billboard->SetPosition(float3(position.x + offsetX * i + mainXOffset, position.y, position.z + offsetZ * j + mainZOffset));
			billboard->SetSize(width, height);
		}
	}
}

bool ComponentGrass::DrawElements()
{
	if (IsValid())
	{
		shader->Activate();

		glBindTexture(GL_TEXTURE_2D, textureBufferId);
		glBindVertexArray(quadMeshInfo.vao);

		float4x4 modelMatrix(1, 0, 0, position.x, 0, 1, 0, position.y, 0, 0, 1, position.z, 0, 0, 0, 1);
		glUniformMatrix4fv(privateUniforms["model_matrix"], 1, GL_FALSE, modelMatrix.Transposed().ptr());
		glUniformMatrix4fv(privateUniforms["view"], 1, GL_FALSE, App->editorCamera->camera->GetViewMatrix());
		glUniformMatrix4fv(privateUniforms["projection"], 1, GL_FALSE, App->editorCamera->camera->GetProjectionMatrix());
		UpdatePublicUniforms();

		glDrawElements(GL_TRIANGLES, quadMeshInfo.elementsCount, GL_UNSIGNED_INT, nullptr);


		glBindVertexArray(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, 0);

		shader->Deactivate();
		return true;
	}

	return false;
}

bool ComponentGrass::LoadVertexShader()
{
	std::string vertexString;
	if (!LoadTextFile(vertexShaderPath, vertexString))
		return false;

	strncpy_s(shader->vertexPath, sizeof(shader->vertexPath), vertexShaderPath, sizeof(vertexShaderPath));

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;

	return true;
}

bool ComponentGrass::LoadFragmentShader()
{
	std::string fragmentString;
	if (!LoadTextFile(fragmentShaderPath, fragmentString))
		return false;

	strncpy_s(shader->fragmentPath, sizeof(shader->fragmentPath), fragmentShaderPath, sizeof(fragmentShaderPath));

	if (!shader->CompileFragmentShader(fragmentString.c_str()))
		return false;

	return true;
}

bool ComponentGrass::LoadShaderData()
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

bool ComponentGrass::LoadTexture()
{
	if (textureBufferId != 0) {
		App->textureManager->ReleaseTexture(textureBufferId);
		textureBufferId = 0;
		textureInfo.Zero();
	}

	if (!IsEmptyString(texturePath))
		textureBufferId = App->textureManager->GetTexture(texturePath);

	if (textureBufferId != 0)
		ConfigureTexture();

	return textureBufferId != 0;
}

void ComponentGrass::ConfigureTexture()
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

bool ComponentGrass::GenerateUniforms()
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

void ComponentGrass::UpdatePublicUniforms()
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

void ComponentGrass::OnEditorBillboardConfiguration()
{
	static const std::string baseLabel = std::string("Billboard configuration##BillboardConfig");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
		if (ImGui::DragFloat3("Position", position.ptr(), 0.03f))
			UpdateBillboards();


		if (ImGui::InputInt("Width", &width))
			UpdateBillboards();


		if (ImGui::InputInt("Height", &height))
			UpdateBillboards();


		if (ImGui::InputInt("InstancesX", &billboardInstancesX))
			CreateQuadVAO();

		if (ImGui::InputInt("InstancesZ", &billboardInstancesZ))
			CreateQuadVAO();

		if (ImGui::InputFloat("OffsetX", &offsetX))
			UpdateBillboards();

		if (ImGui::InputFloat("OffsetZ", &offsetZ))
			UpdateBillboards();

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentGrass::OnEditorMaterialConfiguration()
{
	static const std::string baseLabel = std::string("Material setup##MatSetup");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
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

void ComponentGrass::OnEditorTextureInformation()
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

void ComponentGrass::OnEditorTextureConfiguration()
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
			ConfigureTexture();
		}

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

void ComponentGrass::OnEditorShaderOptions()
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

Shader* ComponentGrass::ShaderAlreadyLinked()
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
