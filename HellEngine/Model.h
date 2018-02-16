#ifndef __H_MODEL__
#define __H_MODEL__

struct aiScene;

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
	
private:

	const aiScene* assimpScene = nullptr;
	unsigned int* textureBufferIds = nullptr;
	unsigned int numTextureBufferIds = 0;
};

#endif // !__H_MODEL__
