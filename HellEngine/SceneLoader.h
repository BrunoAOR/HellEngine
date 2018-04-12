#ifndef __H_SCENE_LOADER__
#define __H_SCENE_LOADER__

class GameObject;
struct aiMesh;
struct aiNode;
struct aiScene;
struct MeshInfo;

class SceneLoader
{
public:
	SceneLoader();

	void LoadCubeMesh();
	void LoadSphereMesh(unsigned int rings = 32u, unsigned int sections = 32u);
	bool Load(const char* modelPath, GameObject* parent, bool meshesOnly = false);

	~SceneLoader();

private:

	void LoadNode(const aiNode* node, GameObject* parent);
	void LoadMeshes();
	void StoreBoneToTransformLinks();

	MeshInfo* CreateMeshInfo(const aiMesh* assimpMesh);
	void GatherVerticesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo, char* data, unsigned int vertexDataOffset, int* indexes);
	void GatherBonesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo, char* data, unsigned int vertexDataOffset);
	void SendDataToVRAM(MeshInfo* meshInfo, char* data, unsigned int vertexDataOffset, unsigned int dataSize, int* indexes);
	void GetTextureFullPath(unsigned int materialIndex, char* outputFullPath);

	

private:

	const aiScene* assimpScene = nullptr;
	const char* currentModelPath = nullptr;
	unsigned int moduleSceneMeshesOffset = 0;

	std::vector<ComponentMesh*> currentComponentMeshes;
};

#endif // !__H_SCENE_LOADER__
