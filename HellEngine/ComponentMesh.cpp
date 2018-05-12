#define _USE_MATH_DEFINES
#include <math.h>
#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModelInfo.h"
#include "ModuleScene.h"
#include "ModuleShaderManager.h"
#include "MeshInfo.h"
#include "SerializableObject.h"
#include "globals.h"
#include "openGL.h"

uint ComponentMesh::meshesCount = 0;
std::vector<ModelInfo> ComponentMesh::defaultModelInfos;

ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	if (meshesCount == 0)
	{
		App->scene->meshes.reserve(App->scene->meshes.size() + 2);
		PreparePlaneMeshInfo();
		PrepareCubeMeshInfo();
		PrepareSphereMeshInfo();
	}
	++meshesCount;
	UpdateBoundingBox();
	//LOGGER("Component of type '%s'", GetString(type));
}

ComponentMesh::~ComponentMesh()
{
	//LOGGER("Deleting Component of type '%s'", GetString(type));
	--meshesCount;
	if (meshesCount == 0)
	{
		defaultModelInfos.clear();
	}
}

void ComponentMesh::Update()
{
	const ModelInfo* modelInfo = GetActiveModelInfo();
	if (modelInfo)
	{
		bool useGPUSkinning = App->shaderManager->GetDefaultOptionState(ShaderOptions::GPU_SKINNING);
		for (uint meshInfosIndex : modelInfo->meshInfosIndexes)
		{
			MeshInfo* meshInfo = App->scene->meshes[meshInfosIndex];
			if (!meshInfo->bones.empty())
			{
				if (useGPUSkinning && !wasUsingGPUSkinning)
				{
					ResetVerticesPositions(meshInfo);
				}
				else if (!useGPUSkinning && wasUsingGPUSkinning)
				{
					InitializeBonesPalettes();
				}
				ApplyVertexSkinning(meshInfo, useGPUSkinning);
			}
		}
		if (useGPUSkinning && !wasUsingGPUSkinning)
		{
			wasUsingGPUSkinning = true;
		}
		else if (!useGPUSkinning && wasUsingGPUSkinning)
		{
			wasUsingGPUSkinning = false;
		}
	}
}

const ModelInfo* ComponentMesh::GetActiveModelInfo() const
{
	if (activeModelInfo >= 0 && activeModelInfo < (int)defaultModelInfos.size())
		return &defaultModelInfos[activeModelInfo];

	if (activeModelInfo == 3 && customModelInfo.meshInfosIndexes.size() > 0)
		return &customModelInfo;

	return nullptr;
}

bool ComponentMesh::SetActiveModelInfo(int index)
{
	/* Both -1 and the defaultModelInfos size are allowed to account for the "None" and the "Custom Model" option */
	if (index < -1 || index > (int)defaultModelInfos.size())
		return false;

	activeModelInfo = index;
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	UpdateBoundingBox();
	return true;
}

void ComponentMesh::SetCustomModel(const ModelInfo& modelInfo)
{
	customModelInfo = modelInfo;
	InitializeBonesPalettes();
	activeModelInfo = 3;
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	UpdateBoundingBox();
}

void ComponentMesh::OnEditor()
{
	static int selectedMeshOption = 0;
	static const char* options = "None\0Plane\0Cube\0Sphere\0Custom Model\0\0";

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		selectedMeshOption = activeModelInfo + 1;
		if (ImGui::Combo("Selected Mesh", &selectedMeshOption, options))
			SetActiveModelInfo(selectedMeshOption - 1);

		if (activeModelInfo == 3 && customModelInfo.meshInfosIndexes.size() == 0)
		{
			ImGui::Text("No custom model found in this mesh!");
		}
	}
}

int ComponentMesh::MaxCountInGameObject()
{
	return 1;
}

void ComponentMesh::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	obj.AddInt("ActiveModelInfo", activeModelInfo);
	std::vector<int> modelInfos;
	for (unsigned int i = 0; i < customModelInfo.meshInfosIndexes.size(); i++)
		modelInfos.push_back(customModelInfo.meshInfosIndexes.at(i));

	obj.AddVectorInt("ModelInfos", modelInfos);
}

void ComponentMesh::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	activeModelInfo = obj.GetInt("ActiveModelInfo");

	std::vector<int> modelInfos = obj.GetVectorInt("ModelInfos");
	if (!modelInfos.empty())
	{
		customModelInfo.meshInfosIndexes.reserve(modelInfos.size());
		customModelInfo.meshInfosIndexes.insert(customModelInfo.meshInfosIndexes.begin(), modelInfos.begin(), modelInfos.end());
	}
	InitializeBonesPalettes();
	UpdateBoundingBox();
}

void ComponentMesh::PreparePlaneMeshInfo()
{
	ModelInfo modelInfo;
	modelInfo.meshInfosIndexes.push_back(0);
	defaultModelInfos.push_back(modelInfo);
}

void ComponentMesh::PrepareCubeMeshInfo()
{
	ModelInfo modelInfo;
	modelInfo.meshInfosIndexes.push_back(1);
	defaultModelInfos.push_back(modelInfo);
}

void ComponentMesh::PrepareSphereMeshInfo()
{
	ModelInfo modelInfo;
	modelInfo.meshInfosIndexes.push_back(2);
	defaultModelInfos.push_back(modelInfo);
}

void ComponentMesh::UpdateBoundingBox()
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform)
		transform->UpdateBoundingBox(this);
}

void ComponentMesh::StoreBoneToTransformLinks()
{
	const ModelInfo* activeModelInfo = GetActiveModelInfo();
	GameObject* parent = gameObject->GetParent();

	if (parent)
	{
		for (uint meshInfoIndex : activeModelInfo->meshInfosIndexes)
		{
			MeshInfo* meshInfo = App->scene->meshes.at(meshInfoIndex);
			for (Bone* bone : meshInfo->bones)
			{
				GameObject* go = parent->FindByName(bone->name);
				assert(go);
				ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
				assert(transform);

				boneToTransformLinks[bone] = transform;
			}
		}
	}
}

const std::map<const MeshInfo*, float4x4[MAX_BONES]> ComponentMesh::GetBonesPalletes() const
{
	return bonesPalettes;
}

void ComponentMesh::ApplyVertexSkinning(const MeshInfo* meshInfo, bool useGPUSkinning)
{
	BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning", Profiler::Color::Crimson);
	/* Find the ComponentAnimation in the siblings to identify the Root Node of the animation */
	std::vector<GameObject*> siblings = gameObject->GetParent()->GetChildren();
	ComponentAnimation* animationComponent = nullptr;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		animationComponent = (ComponentAnimation*)parent->GetComponent(ComponentType::ANIMATION);
	}
	if (!animationComponent)
	{
		for (GameObject* sibling : siblings)
		{
			animationComponent = (ComponentAnimation*)sibling->GetComponent(ComponentType::ANIMATION);
			if (animationComponent)
				break;
		}
	}

	if (animationComponent)
	{
		GameObject* root = animationComponent->gameObject;
		assert(root);

		/* Get the rootToWorldInverse matrix because all bone inverse matrices are in model (Root) coordinate space */
		ComponentTransform* rootTransform = (ComponentTransform*)root->GetComponent(ComponentType::TRANSFORM);
		assert(rootTransform);
		float4x4 rootToWorldInverse = rootTransform->GetModelMatrix4x4().Inverted();
		//float4x4 rootToWorldInverse = rootToWorld.Inverted().Transposed();
		
		if (useGPUSkinning)
		{
			uint bonesCount = meshInfo->bones.size();
			for (unsigned int i = 0; i < bonesCount; ++i)
			{
				Bone* bone = meshInfo->bones[i];
				float4x4 matrixProduct = boneToTransformLinks[bone]->GetModelMatrix4x4();
				matrixProduct.LeftMultiply(rootToWorldInverse);
				matrixProduct.RightMultiply(bone->inverseBindMatrix);
				bonesPalettes[meshInfo][i] = matrixProduct;
			}
		}
		else
		{
			uint verticesCount = meshInfo->vertices.size();

			/* Reset all vertices in the VRAM to ZERO */
			char* vramData = new char[verticesCount * meshInfo->vertexDataOffset];
			glBindVertexArray(meshInfo->vao);
			glBindBuffer(GL_ARRAY_BUFFER, meshInfo->vbo);
			glGetBufferSubData(GL_ARRAY_BUFFER, 0, verticesCount * meshInfo->vertexDataOffset, vramData);

			for (uint v = 0; v < verticesCount; ++v)
			{
				memset(vramData + (v * meshInfo->vertexDataOffset), 0, sizeof(float) * 3);
			}

			/* Iterate through bones and add their effect to the adecuate vertices */
			bool useVerticesGroupOptimization = true;
			if (useVerticesGroupOptimization)
			{
				for (std::map<std::vector<uint>, VerticesGroup>::const_iterator it = meshInfo->verticesGroups.begin(); it != meshInfo->verticesGroups.cend(); ++it)
				{
					const VerticesGroup& verticesGroup = it->second;
					uint vectorSize = verticesGroup.bones.size();
					assert(vectorSize == verticesGroup.weights.size());

					float4x4 matrixSum = float4x4::zero;

					for (uint i = 0; i < vectorSize; ++i)
					{
						Bone* bone = verticesGroup.bones[i];

						float4x4 matrixProduct = boneToTransformLinks[bone]->GetModelMatrix4x4();
						matrixProduct.LeftMultiply(rootToWorldInverse);
						matrixProduct.RightMultiply(bone->inverseBindMatrix);
						matrixProduct *= verticesGroup.weights[i];

						matrixSum += matrixProduct;
					}

					for (uint vertexIndex : it->first)
					{
						const float3& originalVertex = meshInfo->vertices[vertexIndex];
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 0 * sizeof(float)] += matrixSum.v[0][0] * originalVertex.x + matrixSum.v[1][0] * originalVertex.y + matrixSum.v[2][0] * originalVertex.z + matrixSum.v[3][0];
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 1 * sizeof(float)] += matrixSum.v[0][1] * originalVertex.x + matrixSum.v[1][1] * originalVertex.y + matrixSum.v[2][1] * originalVertex.z + matrixSum.v[3][1];
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 2 * sizeof(float)] += matrixSum.v[0][2] * originalVertex.x + matrixSum.v[1][2] * originalVertex.y + matrixSum.v[2][2] * originalVertex.z + matrixSum.v[3][2];
					}
				}
			}
			else
			{
				for (Bone* bone : meshInfo->bones)
				{
					float4x4 matrixProduct = boneToTransformLinks[bone]->GetModelMatrix4x4();
					matrixProduct.LeftMultiply(rootToWorldInverse);
					matrixProduct.RightMultiply(bone->inverseBindMatrix);

					/* Iterate through all Bone weights and apply their effects */
					uint weightsCount = bone->numWeights;
					uint vertexIndex = 0;

					for (uint w = 0; w < weightsCount; ++w)
					{
						/* Calculate effect */
						const BoneWeight& boneWeight = bone->weights[w];
						vertexIndex = boneWeight.vertexIndex;
						const float3& originalVertex = meshInfo->vertices[vertexIndex];

						/* Apply effect */
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 0 * sizeof(float)] += boneWeight.weight * (matrixProduct.v[0][0] * originalVertex.x + matrixProduct.v[1][0] * originalVertex.y + matrixProduct.v[2][0] * originalVertex.z + matrixProduct.v[3][0]);
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 1 * sizeof(float)] += boneWeight.weight * (matrixProduct.v[0][1] * originalVertex.x + matrixProduct.v[1][1] * originalVertex.y + matrixProduct.v[2][1] * originalVertex.z + matrixProduct.v[3][1]);
						(float&)vramData[vertexIndex * meshInfo->vertexDataOffset + 2 * sizeof(float)] += boneWeight.weight * (matrixProduct.v[0][2] * originalVertex.x + matrixProduct.v[1][2] * originalVertex.y + matrixProduct.v[2][2] * originalVertex.z + matrixProduct.v[3][2]);
					}
				}
			}

			/* Unmap buffer */
			glBufferSubData(GL_ARRAY_BUFFER, 0, verticesCount * meshInfo->vertexDataOffset, vramData);

			delete vramData;
			vramData = nullptr;
		}
	}
}

void ComponentMesh::ResetVerticesPositions(const MeshInfo* meshInfo)
{
	uint verticesCount = meshInfo->vertices.size();
	char* vramData = new char[verticesCount * meshInfo->vertexDataOffset];
	glBindVertexArray(meshInfo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshInfo->vbo);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, verticesCount * meshInfo->vertexDataOffset, vramData);

	for (uint v = 0; v < verticesCount; ++v)
	{
		(float3&)vramData[v * meshInfo->vertexDataOffset] = meshInfo->vertices[v];
	}

	/* Unmap buffer */
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesCount * meshInfo->vertexDataOffset, vramData);

	delete vramData;
	vramData = nullptr;
}

void ComponentMesh::InitializeBonesPalettes()
{
	bonesPalettes.clear();
	for (unsigned int i = 0; i < customModelInfo.meshInfosIndexes.size(); ++i)
	{
		const MeshInfo* meshInfo = App->scene->meshes.at(customModelInfo.meshInfosIndexes[i]);
		for (unsigned int i = 0; i < MAX_BONES; ++i)
		{
			bonesPalettes[meshInfo][i] = float4x4::identity;
		}
	}
}
