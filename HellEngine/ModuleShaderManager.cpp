#include "ImGui/imgui.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleShaderManager.h"
#include "ShaderProgram.h"
#include "globals.h"
#include "openGL.h"


ModuleShaderManager::ModuleShaderManager()
{
}

ModuleShaderManager::~ModuleShaderManager()
{
}

bool ModuleShaderManager::CleanUp()
{
	for (std::map<std::string, ShaderProgramData>::iterator it = shaderProgramUsage.begin(); it != shaderProgramUsage.end(); ++it)
	{
		glDeleteProgram(it->second.shaderProgram->GetProgramId());
		if (it->second.numRefs != 0)
		{
			LOGGER("WARNING: ModuleShaderManager - ShaderProgram with id %u was still being held by %u users upon manager clean up", it->second.shaderProgram->GetProgramId(), it->second.numRefs);
		}
		delete(it->second.shaderProgram);
	}
	shaderProgramUsage.clear();
	return true;
}

const ShaderProgram* ModuleShaderManager::GetShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options)
{
	if (options == ShaderOptions::DEFAULT)
		options = defaultShaderOptions;

	if (IsEmptyString(vertexShaderPath))
	{
		LOGGER("ERROR: ModuleShaderManager - vertexShaderPath is empty");
		return nullptr;
	}
	if (IsEmptyString(fragmentShaderPath))
	{
		LOGGER("ERROR: ModuleShaderManager - fragmentShaderPath is empty");
		return nullptr;
	}

	/* The joint paths as used as key */
	std::string jointPath = GetJointPath(vertexShaderPath, fragmentShaderPath, options);

	for (std::map<std::string, ShaderProgramData>::iterator it = shaderProgramUsage.begin(); it != shaderProgramUsage.end(); ++it)
	{
		if (jointPath.size() == it->first.size())
		{
			if (jointPath.compare(it->first) == 0)
			{
				it->second.numRefs++;
				return it->second.shaderProgram;
			}
		}
	}

	return GenerateNewShaderProgram(vertexShaderPath, fragmentShaderPath, options);
}

void ModuleShaderManager::ReleaseShaderProgram(const ShaderProgram* shaderProgram)
{
	if (shaderProgram == nullptr || shaderProgram->GetProgramId() == 0)
		return;
	
	for (std::map<std::string, ShaderProgramData>::iterator it = shaderProgramUsage.begin(); it != shaderProgramUsage.end(); ++it)
	{
		if (it->second.shaderProgram == shaderProgram)
		{
			it->second.numRefs--;
			if (it->second.numRefs == 0)
			{
				glDeleteProgram(shaderProgram->GetProgramId());
				delete(shaderProgram);
				shaderProgramUsage.erase(it);
			}
			return;
		}
	}
}

bool ModuleShaderManager::GetDefaultOptionState(ShaderOptions option)
{
	return (defaultShaderOptions & option) == option;
}

void ModuleShaderManager::OnEditorShaderOptionsWindow(float mainMenuBarHeight, bool* pOpen)
{
	static bool useVertexLighting = false;
	static bool usePixelLighting = false;
	static bool useSpecularLighting = false;
	static bool useNormalMapping = false;
	static bool useGPUSkinning = false;

	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(450, 600));
	ImGui::Begin("Shader options", pOpen, ImGuiWindowFlags_NoCollapse);

	if (ImGui::Checkbox("Vertex Lighting", &useVertexLighting))
	{
		defaultShaderOptions ^= ShaderOptions::VERTEX_LIGHTING;
		if (usePixelLighting && useVertexLighting)
		{
			usePixelLighting = false;
			defaultShaderOptions &= ~(ShaderOptions::PIXEL_LIGHTING);
			useNormalMapping = false;
			defaultShaderOptions &= ~(ShaderOptions::NORMAL_MAPPING);
		}
		if (!usePixelLighting && !useVertexLighting)
		{
			useSpecularLighting = false;
			defaultShaderOptions &= ~(ShaderOptions::SPECULAR);
		}
	}

	if (ImGui::Checkbox("Pixel Lighting", &usePixelLighting))
	{
		defaultShaderOptions ^= ShaderOptions::PIXEL_LIGHTING;
		if (useVertexLighting && usePixelLighting)
		{
			useVertexLighting = false;
			defaultShaderOptions &= ~(ShaderOptions::VERTEX_LIGHTING);
		}
		if (!useVertexLighting && !usePixelLighting)
		{
			useSpecularLighting = false;
			defaultShaderOptions &= ~(ShaderOptions::SPECULAR);
		}
		if (!usePixelLighting)
		{
			useNormalMapping = false;
			defaultShaderOptions &= ~(ShaderOptions::NORMAL_MAPPING);
		}
	}

	if (usePixelLighting || useVertexLighting)
	{
		ImGui::Text("Lighting options:");
		ImGui::Indent();

		if (ImGui::Checkbox("Specular", &useSpecularLighting))
			defaultShaderOptions ^= ShaderOptions::SPECULAR;


		if (usePixelLighting)
		{
			if (ImGui::Checkbox("Normal Mapping", &useNormalMapping))
				defaultShaderOptions ^= ShaderOptions::NORMAL_MAPPING;
		}

		ImGui::Unindent();
	}

	if (ImGui::Checkbox("GPU Skinning", &useGPUSkinning))
		defaultShaderOptions ^= ShaderOptions::GPU_SKINNING;

	ImGui::End();
}

const ShaderProgram* ModuleShaderManager::GenerateNewShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options)
{
	std::string sourceString;
	
	/* Compile Vertex Shader */
	if (!LoadTextFile(vertexShaderPath, sourceString))
	{
		LOGGER("ERROR: ModuleShaderManager - Could not load vertex shader file from path %s", vertexShaderPath);;
	}
	AddShaderPrefix(sourceString, options);
	uint vertexShaderId = CompileShader(sourceString.c_str(), ShaderType::VERTEX);
	if (vertexShaderId == 0)
	{
		return nullptr;
	}

	/* Compile Fragment Shader */
	if (!LoadTextFile(fragmentShaderPath, sourceString))
	{
		LOGGER("ERROR: ModuleShaderManager - Could not load fragment shader file from path %s", fragmentShaderPath);;
	}
	AddShaderPrefix(sourceString, options);
	uint fragmentShaderId = CompileShader(sourceString.c_str(), ShaderType::FRAGMENT);
	if (fragmentShaderId == 0)
	{
		glDeleteShader(vertexShaderId);
		return nullptr;
	}

	/* Link Shader Program */
	uint programId = LinkShaderProgram(vertexShaderId, fragmentShaderId);

	/* Delete individual shaders regardless of linking success */
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	if (programId == 0)
	{
		return nullptr;
	}

	/* Link Matrices UBO */
	uint matricesUniformBlockIndex = glGetUniformBlockIndex(programId, "Matrices");
	if (matricesUniformBlockIndex != GL_INVALID_INDEX)
		glUniformBlockBinding(programId, matricesUniformBlockIndex, App->renderer->matricesUBOBindingPoint);

	/* Prepare ShaderProgram */
	ShaderProgram* shaderProgram = new ShaderProgram(programId, options);

	/* Save ShaderProgram for future reuse */
	ShaderProgramData programData;
	programData.shaderProgram = shaderProgram;
	programData.numRefs = 1;

	/* Use joint paths as key */
	std::string jointPath = GetJointPath(vertexShaderPath, fragmentShaderPath, options);
	shaderProgramUsage[jointPath] = programData;

	return shaderProgram;
}

unsigned int ModuleShaderManager::CompileShader(const char* sourceString, ShaderType shaderType)
{
	GLenum glShaderType;
	switch (shaderType)
	{
	case ShaderType::VERTEX:
		glShaderType = GL_VERTEX_SHADER;
		break;
	case ShaderType::FRAGMENT:
		glShaderType = GL_FRAGMENT_SHADER;
		break;
	default:
		return 0;
	}

	GLuint shaderId;
	shaderId = glCreateShader(glShaderType);
	glShaderSource(shaderId, 1, &sourceString, NULL);
	glCompileShader(shaderId);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		LOGGER("ERROR: ModuleShaderManager - Shader compilation error: %s", infoLog);
		glDeleteShader(shaderId);
		return 0;
	}
	else
	{
		return shaderId;
	}
}

unsigned int ModuleShaderManager::LinkShaderProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId)
{
	GLuint programId;

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (success == 0) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		glDeleteProgram(programId);
		LOGGER("ERROR: ModuleShaderManager - Shader link error: %s", infoLog);
		programId = 0;
	}

	return programId;
}

void ModuleShaderManager::AddShaderPrefix(std::string& sourceString, ShaderOptions options)
{
	uint insertionPoint = 0;
	if (sourceString.compare(0, 8, "#version") == 0)
	{
		insertionPoint = sourceString.find('\n') + 1;
	}
	assert(insertionPoint > 0);

	if ((options & ShaderOptions::GPU_SKINNING) == ShaderOptions::GPU_SKINNING)
		sourceString.insert(insertionPoint, "#define GPU_SKINNING\n");
	
	if ((options & ShaderOptions::NORMAL_MAPPING) == ShaderOptions::NORMAL_MAPPING)
		sourceString.insert(insertionPoint, "#define NORMAL_MAPPING\n");

	if ((options & ShaderOptions::SPECULAR) == ShaderOptions::SPECULAR)
		sourceString.insert(insertionPoint, "#define SPECULAR\n");

	if ((options & ShaderOptions::PIXEL_LIGHTING) == ShaderOptions::PIXEL_LIGHTING)
		sourceString.insert(insertionPoint, "#define PIXEL_LIGHTING\n");
	else if ((options & ShaderOptions::VERTEX_LIGHTING) == ShaderOptions::VERTEX_LIGHTING)
		sourceString.insert(insertionPoint, "#define VERTEX_LIGHTING\n");
}

std::string ModuleShaderManager::GetJointPath(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options)
{
	std::string jointPath(vertexShaderPath);
	jointPath += fragmentShaderPath;
	jointPath += std::to_string(static_cast<unsigned int>(options));
	return jointPath;
}
