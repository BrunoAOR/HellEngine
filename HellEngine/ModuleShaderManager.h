#ifndef __H_MODULE_SHADER_MANAGER
#define __H_MODULE_SHADER_MANAGER
#include <map>
#include "Module.h"
#include "ShaderOptions.h"
class ShaderProgram;

class ModuleShaderManager :
	public Module
{
public:
	ModuleShaderManager();
	~ModuleShaderManager();

	virtual bool CleanUp() override;

	const ShaderProgram* GetShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options = ShaderOptions::NONE);
	void ReleaseShaderProgram(const ShaderProgram* shaderProgram);

private:
	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};

	struct ShaderProgramData
	{
		ShaderProgram* shaderProgram = nullptr;
		unsigned int numRefs = 0;
	};

private:
	const ShaderProgram* GenerateNewShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options = ShaderOptions::NONE);
	unsigned int CompileShader(const char* sourceString, ShaderType shaderType);
	unsigned int LinkShaderProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId);
	void ModuleShaderManager::AddShaderPrefix(std::string& sourceString, ShaderOptions options);
	std::string GetJointPath(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderOptions options);

private:
	std::map<std::string, ShaderProgramData> shaderProgramUsage;
};

#endif // !__H_MODULE_SHADER_MANAGER
