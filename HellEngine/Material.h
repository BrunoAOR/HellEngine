#ifndef __H_MATERIAL__
#define __H_MATERIAL__

#include <string>
#include "globals.h"
class Shader;

class Material
{
public:
	Material();
	~Material();

	/* Recieves the vertex shader file path and tries to compile it */
	bool SetVertexShaderPath(const std::string& sourcePath);
	
	/* Recieves the fragment shader file path and tries to compile it */
	bool SetFragmentShaderPath(const std::string& sourcePath);

	/* Recieves the texture file path and tries to load it using Devil */
	bool SetTexturePath(const std::string& sourcePath);
	
	/* Returns whether the Material has a loaded texture and a linked shader */
	bool IsValid();

	/* Attemps to link the shader, if a valid texture has been set */
	bool Apply();

	/* Reloads the vertex shader, fragment shader and texture, recompiles the shaders and relinks the shader program */
	bool Reapply();

	/* Draws a certain model using the Material's shader and texture */
	bool DrawArray(float* modelMatrix, uint drawDataBufferId, uint vertexCount);

private:
	Shader* shader = nullptr;
	uint textureBufferId = 0;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string texturePath;
};

#endif // !__H_MATERIAL__
