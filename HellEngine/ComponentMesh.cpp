#include "ImGui/imgui.h"
#include "ComponentMesh.h"
#include "ComponentType.h"


ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::OnEditor()
{
	ImGui::Text("Mesh GUI goes here");
}
