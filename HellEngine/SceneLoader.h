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

	bool Load(const char* modelPath, GameObject* parent, bool meshesOnly = false);

	~SceneLoader();

private:

	void LoadNode(const aiNode* node, GameObject* parent);
	void LoadMeshes();
	void StoreBoneToTransformLinks();

	MeshInfo* CreateMeshInfo(const aiMesh* assimpMesh);
	void GatherVerticesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo);
	void GatherBonesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo);
	void GetTextureFullPath(unsigned int materialIndex, char* outputFullPath);

	

private:

	const aiScene* assimpScene = nullptr;
	const char* currentModelPath = nullptr;
	unsigned int moduleSceneMeshesOffset = 0;

	std::vector<MeshInfo*> currentMeshInfos;
	GameObject* rootNode = nullptr;

};

#endif // !__H_SCENE_LOADER__
