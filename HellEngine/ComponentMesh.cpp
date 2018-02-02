#include "ImGui/imgui.h"
#include "ComponentMesh.h"
#include "ComponentType.h"
#include "globals.h"


ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	LOGGER("Component of type '%s'", GetEditorTitle(type));
}

ComponentMesh::~ComponentMesh()
{
	LOGGER("Deleting Component of type '%s'", GetEditorTitle(type));
}

void ComponentMesh::OnEditor()
{
	ImGui::Text("Mesh GUI goes here");
}
