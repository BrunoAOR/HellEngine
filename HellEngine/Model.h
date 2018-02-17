#ifndef __H_MODEL__
#define __H_MODEL__

class Shader;
struct aiScene;
struct VaoInfo;

class Model
{
public:

	Model();
	~Model();

	void Load(const char* modelPath);
	void Clear();
	void Draw() const;

private:

	void DrawMesh(unsigned int meshIndex) const;
	void CreateTextureBuffers(const char* modelPath);
	void CreateVaoInfo();

private:

	VaoInfo* vaoInfos;
	unsigned int numVaoInfos;
	const aiScene* assimpScene = nullptr;
	unsigned int* textureBufferIds = nullptr;
	unsigned int numTextureBufferIds = 0;
	
	static unsigned int modelsCount;
	static Shader* shader;
	static int modelMatrixLoc;
	static int viewLoc;
	static int projectionLoc;
};

#endif // !__H_MODEL__
