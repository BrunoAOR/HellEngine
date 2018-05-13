#include <stack>
#include "ImGui/imgui.h"
#include "ComponentMaterial.h"
#include "ComponentMaterialsGroup.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ShaderOptions.h"


ComponentMaterialsGroup::ComponentMaterialsGroup(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIALS_GROUP;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentMaterialsGroup::~ComponentMaterialsGroup()
{
}

void ComponentMaterialsGroup::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);

		static const std::string baseLabel = std::string("Material setup##MatSetup");
		if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
		{
			if (ImGui::Button("Use defaults"))
				SetDefaultMaterialConfiguration();

			ImGui::InputText("Vertex shader", vertexShaderPath, 256);
			ImGui::InputText("Fragment shader", fragmentShaderPath, 256);
			ImGui::InputText("Shader data", shaderDataPath, 256);
			ImGui::InputText("Texture path", diffusePath, 256);
			ImGui::InputText("Normal Texture path", normalPath, 256);
			ImGui::InputText("Specular Texture path", specularPath, 256);

			if (ImGui::Button("Apply to children"))
				Apply();

			ImGui::NewLine();
			ImGui::TreePop();
		}

		if (!isValid)
		{
			ImGui::Text("Invalid Material setup detected!");
		}
	}
}

int ComponentMaterialsGroup::MaxCountInGameObject()
{
	return 1;
}

bool ComponentMaterialsGroup::Apply()
{
	std::stack<GameObject*> stack;
	stack.push(gameObject);
	GameObject* go = nullptr;
	isValid = true;

	while (!stack.empty())
	{
		go = stack.top();
		stack.pop();

		std::vector<GameObject*> children = go->GetChildren();
		for (GameObject* childGo : children)
		{
			stack.push(childGo);
			std::vector<Component*> componentMaterials = childGo->GetComponents(ComponentType::MATERIAL);
			for (Component* component : componentMaterials)
			{
				ComponentMaterial* material = (ComponentMaterial*)component;
				isValid &= ApplyConfig(material);
			}
		}
	}
	return isValid;
}

void ComponentMaterialsGroup::SetDefaultMaterialConfiguration()
{
	memcpy_s(vertexShaderPath, 256, "assets/shaders/uberShader.vert", 256);
	memcpy_s(fragmentShaderPath, 256, "assets/shaders/uberShader.frag", 256);
	shaderDataPath[0] = '\0';
	diffusePath[0] = '\0';
	normalPath[0] = '\0';
	specularPath[0] = '\0';
}

bool ComponentMaterialsGroup::ApplyConfig(ComponentMaterial* material)
{
	material->SetVertexShaderPath(vertexShaderPath);
	material->SetFragmentShaderPath(fragmentShaderPath);
	material->SetShaderDataPath(shaderDataPath);
	material->SetTexturePath(diffusePath);
	material->SetNormalPath(normalPath);
	material->SetSpecularPath(specularPath);
	return material->Apply();
}
