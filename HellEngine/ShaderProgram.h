#ifndef __H_SHADER_PROGRAM__
#define __H_SHADER_PROGRAM__

class ShaderProgram
{
public:
	ShaderProgram(unsigned int programId);
	~ShaderProgram();

	ShaderProgram(const ShaderProgram& shaderProgram) = delete;
	ShaderProgram& operator= (const ShaderProgram& shaderProgram) = delete;
	ShaderProgram(const ShaderProgram&& a) = delete;
	ShaderProgram& operator= (const ShaderProgram&& shaderProgram) = delete;

	unsigned int GetProgramId() const;
	void Activate() const;
	void Deactivate() const;
	void UpdateMatrixUniforms(const float* modelMatrix, const float* viewMatrix, const float* projectionMatrix) const;
	void UpdateLightingUniforms(const float* normalMatrix, const float* lightPosition, const float* cameraPosition) const;
	void UpdateBonesUniform(const float* bonesPalette) const;

private:
	unsigned int programId = 0;

	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	bool hasLightingUniforms = false;
	int normalMatrixLocation = -1;
	int lightPositionLocation = -1;
	int cameraPositionLocation = -1;

	bool hasBonesUniform = false;
	int bonesPaletteLocation = -1;
};

#endif // !__H_SHADER_PROGRAM__
