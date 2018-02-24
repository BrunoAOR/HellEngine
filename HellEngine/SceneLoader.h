#ifndef __H_SCENE_LOADER__
#define __H_SCENE_LOADER__

#include "VaoInfo.h"
class GameObject;
struct aiMesh;
struct aiNode;
struct aiScene;

class SceneLoader
{
public:
	SceneLoader();

	bool Load(const char* modelPath, GameObject* parent);

	~SceneLoader();

private:

	void LoadNode(const aiNode* node, GameObject* parent);
	VaoInfo* CreateVao(const aiMesh* assimpMesh);
	void GetTextureFullPath(unsigned int materialIndex, char* outputFullPath);

	void LoadMeshes();

private:

	const aiScene* assimpScene = nullptr;
	const char* currentModelPath = nullptr;
	unsigned int moduleSceneMeshesOffset = 0;

};

#endif // !__H_SCENE_LOADER__
