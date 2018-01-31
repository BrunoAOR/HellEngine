#ifndef __H_SHADER__
#define __H_SHADER__

typedef unsigned int GLuint;

class Shader
{
public:
	Shader();
	~Shader();

	bool CompileVertexShader(const char* sourceString);
	bool CompileFragmentShader(const char* sourceString);
	bool LinkShaderProgram();
	bool Activate();
	void Deactivate();
	bool IsValid();

	GLuint getProgramId();

private:
	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};

	bool compileShader(const char* sourceString, ShaderType shaderType);

private:
	GLuint vertexShaderId = 0;
	GLuint fragmentShaderId = 0;
	GLuint shaderProgramId = 0;
};

#endif // !__H_SHADER__
