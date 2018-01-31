#include "Shader.h"
#include "globals.h"
#include "openGL.h"

Shader::Shader()
{
}


Shader::~Shader()
{
	glDeleteShader(vertexShaderId);
	vertexShaderId = 0;
	glDeleteShader(fragmentShaderId);
	fragmentShaderId = 0;
	glDeleteProgram(shaderProgramId);
	shaderProgramId = 0;
}

bool Shader::CompileVertexShader(const char * sourceString)
{
	return compileShader(sourceString, ShaderType::VERTEX);
}

bool Shader::CompileFragmentShader(const char * sourceString)
{
	return compileShader(sourceString, ShaderType::FRAGMENT);
}

bool Shader::LinkShaderProgram()
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
		LOGGER("Shader link error: %s", infoLog);
		return false;
	}
	else
	{
		glDeleteProgram(shaderProgramId);
		shaderProgramId = programId;
		glDeleteShader(vertexShaderId);
		vertexShaderId = 0;
		glDeleteShader(fragmentShaderId);
		fragmentShaderId = 0;
		return true;
	}
}

bool Shader::Activate()
{
	if (shaderProgramId)
	{
		glUseProgram(shaderProgramId);
		return true;
	}
	return false;
}

void Shader::Deactivate()
{
	glUseProgram(GL_NONE);
}

bool Shader::IsValid()
{
	return shaderProgramId != 0;
}

GLuint Shader::getProgramId()
{
	return shaderProgramId;
}

bool Shader::compileShader(const char* sourceString, ShaderType shaderType)
{
	GLenum glShaderType;
	switch (shaderType)
	{
	case Shader::ShaderType::VERTEX:
		glShaderType = GL_VERTEX_SHADER;
		glDeleteShader(vertexShaderId);
		break;
	case Shader::ShaderType::FRAGMENT:
		glShaderType = GL_FRAGMENT_SHADER;
		glDeleteShader(fragmentShaderId);
		break;
	default:
		break;
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
		LOGGER("Shader compilation error: %s", infoLog);
		glDeleteShader(shaderId);
		return false;
	}
	else
	{
		switch (shaderType)
		{
		case Shader::ShaderType::VERTEX:
			vertexShaderId = shaderId;
			break;
		case Shader::ShaderType::FRAGMENT:
			fragmentShaderId = shaderId;
			break;
		default:
			break;
		}
		return true;
	}
}
