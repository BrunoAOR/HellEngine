#ifndef __H_SHADER_PROGRAM__
#define __H_SHADER_PROGRAM__

class ShaderProgram
{
public:
	ShaderProgram(unsigned int programId, unsigned int modelMatrixLocation, unsigned int viewMatrixLocation, unsigned int projectionMatrixLocation);
	~ShaderProgram();

	ShaderProgram(const ShaderProgram& shaderProgram) = delete;
	ShaderProgram& operator= (const ShaderProgram& shaderProgram) = delete;
	ShaderProgram(const ShaderProgram&& a) = delete;
	ShaderProgram& operator= (const ShaderProgram&& shaderProgram) = delete;

	unsigned int GetProgramId() const;
	void Activate() const;
	void Deactivate() const;
	void UpdateMatrixUniforms(const float* model, const float* view, const float* projection) const;

private:
	unsigned int programId;
	unsigned int modelMatrixLocation;
	unsigned int viewMatrixLocation;
	unsigned int projectionMatrixLocation;
};

#endif // !__H_SHADER_PROGRAM__
