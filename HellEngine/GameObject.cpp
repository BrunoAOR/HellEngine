#include <stack>
#include <assert.h>
#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "MathGeoLib/src/Algorithm/Random/LCG.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentAnimation.h"
#include "ComponentCamera.h"
#include "ComponentGrass.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentParticleSystem.h"
#include "ComponentTransform.h"
#include "ComponentTransform2D.h"
#include "ComponentType.h"
#include "ComponentUiButton.h"
#include "ComponentUiImage.h"
#include "ComponentUiInputText.h"
#include "ComponentUiLabel.h"
#include "ModuleUI.h"
#include "ModuleScene.h"
#include "SerializableArray.h"
#include "SerializableObject.h"
#include "globals.h"

GameObject* GameObject::hierarchyActiveGameObject = nullptr;

GameObject::GameObject(const char* name, GameObject* parentGameObject) : name(name)
{
	//LOGGER("Constructing GameObject '%s'", name);
	/* If the root is nullptr, then that means that we are currently creating the root and no parent should be set. */
	if (App->scene->root != nullptr)
	{
		parent = App->scene->root;
		parentUuid = App->scene->root->uuid;
		App->scene->root->children.push_back(this);

		if (parentGameObject != nullptr && parentGameObject != App->scene->root)
			SetParent(parentGameObject);
	}

	LCG lcg;
	uuid = lcg.IntFast();
	App->scene->gameObjectsCount++;
}

GameObject::~GameObject()
{
	//LOGGER("Began deletion of GameObject '%s'", name.c_str());
	if (hierarchyActiveGameObject == this)
		hierarchyActiveGameObject = nullptr;

	for (Component* component : components)
	{
		delete component;
	}
	components.clear();

	for (GameObject* child : children)
	{
		delete child;
	}
	children.clear();

	App->scene->gameObjectsCount--;
	//LOGGER("Deleted GameObject '%s'", name.c_str());
}

void GameObject::Update()
{
	if (!isActive)
		return;

	BROFILER_CATEGORY("ModuleScene::IterativeUpdate", Profiler::Color::PapayaWhip);
	std::stack<GameObject*> stack;

	GameObject* go = this;
	std::vector<GameObject*> children;

	stack.push(go);

	while (!stack.empty()) {
		go = stack.top();
		stack.pop();

		BROFILER_CATEGORY("ModuleScene::ComponentsUpdate", Profiler::Color::PapayaWhip);
		for (Component* component : go->components)
		{
			component->Update();
		}

		BROFILER_CATEGORY("ModuleScene::PostComponentUpdate", Profiler::Color::PapayaWhip);
		children = go->GetChildren();

		for (int i = children.size(); i > 0; i--)
			stack.push(children.at(i - 1));
	}
}

void GameObject::OnEditorInspector()
{
	ImGui::Checkbox("", &isActive);
	ImGui::SameLine();
	static char nameBuf[32] = "";
	memcpy_s(nameBuf, 32, name.c_str(), name.length());
	nameBuf[name.length()] = '\0';
	ImGui::InputText(" ", nameBuf, 32);
	name = nameBuf;
	if (ImGui::BeginMenu("Add new Component"))
	{
		for (ComponentType componentType : COMPONENT_TYPES_3D)
		{
			if (ImGui::MenuItem(GetString(componentType), ""))
			{
				AddComponent(componentType);
			}
		}

		if (ImGui::BeginMenu("UI"))
		{
			for (ComponentType componentType : COMPONENT_TYPES_2D)
			{
				if (ImGui::MenuItem(GetString(componentType), ""))
				{
					AddComponent(componentType);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
	for (Component* component : components)
	{
		component->OnEditor();
	}
	if (componentPendingToRemove)
	{
		for (Component* component : components)
		{
			if (component->toRemove)
			{
				component->gameObject->RemoveComponent(component);
			}
		}
		componentPendingToRemove = false;
	}
}

void GameObject::OnEditorRootHierarchy()
{
	bool open = false;
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

	if (children.size() > 0)
		open = ImGui::TreeNodeEx(this, 0, "Scene");
	else
		ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Scene");

	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseReleased(0) && hierarchyActiveGameObject)
	{
		hierarchyActiveGameObject->SetParent(this);
		hierarchyActiveGameObject = nullptr;
	}
	if (ImGui::BeginPopupContextItem())
	{
		OnEditorHierarchyCreateMenu();
		ImGui::Separator();
		OnEditorHierarchyLoadModelMenu();
		ImGui::EndPopup();
	}
	if (open)
	{
		for (GameObject* child : children)
		{
			if (child->OnEditorHierarchy())
				break;
		}
		ImGui::TreePop();
	}
}

bool GameObject::OnEditorHierarchy()
{
	bool earlyReturn = false;
	ImGuiTreeNodeFlags selectedFlag = App->scene->editorInfo.selectedGameObject == this ? ImGuiTreeNodeFlags_Selected : 0;
	if (children.size() == 0)
	{
		ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | selectedFlag, name.c_str());
		earlyReturn = OnEditorHierarchyRightClick();
		if (earlyReturn)
			return true;

		if (ImGui::IsItemClicked())
			App->scene->editorInfo.selectedGameObject = this;
		else
			OnEditorHierarchyDragAndDrop();

	}
	else
	{
		bool open = ImGui::TreeNodeEx(this, selectedFlag, name.c_str());
		earlyReturn = OnEditorHierarchyRightClick();
		if (earlyReturn)
		{
			if (open) ImGui::TreePop();
			return true;
		}

		if (ImGui::IsItemClicked())
			App->scene->editorInfo.selectedGameObject = this;
		else
			OnEditorHierarchyDragAndDrop();

		if (open)
		{
			for (GameObject* child : children)
			{
				earlyReturn = child->OnEditorHierarchy();
				if (earlyReturn)
					break;
			}
			ImGui::TreePop();
		}
	}
	return earlyReturn;
}

void GameObject::OnEditorHierarchyDragAndDrop()
{
	static ImGuiIO& io = ImGui::GetIO();

	if (ImGui::IsItemActive())
	{
		hierarchyActiveGameObject = this;
		// Draw a line between the button and the mouse cursor
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRectFullScreen();
		draw_list->AddLine(ImGui::CalcItemRectClosestPoint(io.MousePos, true, -2.0f), io.MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_PlotLines]), 2.0f);
		draw_list->PopClipRect();
	}
	else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseReleased(0) && hierarchyActiveGameObject)
	{
		hierarchyActiveGameObject->SetParent(this);
		hierarchyActiveGameObject = nullptr;
	}
}

bool GameObject::OnEditorHierarchyRightClick()
{
	bool earlyReturn = false;
	if (ImGui::BeginPopupContextItem())
	{
		OnEditorHierarchyCreateMenu();

		if (ImGui::Selectable("Destroy"))
		{
			App->scene->Destroy(this);
			App->scene->editorInfo.selectedGameObject = nullptr;
			ImGui::EndPopup();
			return true;
		}

		ImGui::Separator();

		if (IsFirstChild())
			ImGui::TextDisabled("Move up");
		else
			if (ImGui::Selectable("Move up"))
				SwapWithPreviousSibling();

		if (IsLastChild())
			ImGui::TextDisabled("Move down");
		else
			if (ImGui::Selectable("Move down"))
				SwapWithNextSibling();

		ImGui::Separator();

		earlyReturn = OnEditorHierarchyLoadModelMenu();

		ImGui::EndPopup();
	}
	return earlyReturn;

}

void GameObject::OnEditorHierarchyCreateMenu()
{
	if (ImGui::BeginMenu("Create..."))
	{
		if (ImGui::Selectable("Create Empty"))
			AddEmptyChild();

		if (ImGui::Selectable("Create Camera"))
			AddCameraChild();

		ImGui::Separator();

		if (ImGui::Selectable("Create Cube"))
			AddCubeChild();

		if (ImGui::Selectable("Create Sphere"))
			AddSphereChild();

		if (ImGui::BeginMenu("UI"))
		{
			for (UIElementType uiElementType : UI_ELEMENT_TYPES)
			{
				if (ImGui::Selectable(GetUITypeString(uiElementType)))
					AddUiElement(uiElementType);

			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

bool GameObject::OnEditorHierarchyLoadModelMenu()
{
	static char modelPath[256]{ '\0' };
	static bool error = false;

	if (ImGui::BeginMenu("Load Model"))
	{
		ImGui::InputText("Model path", modelPath, 256);
		ImGui::Separator();
		if (ImGui::Selectable("Load"))
			error = !App->scene->LoadModel(modelPath, this);
		
		if (error)
			ImGui::Text("Could not load model from provided path!");

		ImGui::EndMenu();
	}
	return !error;
}

GameObject* GameObject::GetParent()
{
	/* The scene root remains protected from use */
	if (parent == App->scene->root)
		return nullptr;
	else
		return parent;
}

bool GameObject::SetParent(GameObject* newParent)
{
	/* Protect against manipulation of the root */
	if (this == App->scene->root)
		return false;

	/* All non-root gameObjects are guaranteed to have a parent and the root should never be directly used. But always check. */
	assert(parent);

	/* Abort if assigning to self or the current parent */
	if (newParent == this || newParent == parent)
		return true;

	/* Verify for nullptr. Parent to Scene's root if so */
	if (newParent == nullptr)
	{
		/* Inform the transform or transform2D (if it exists) */
		ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);
		if (transform)
			transform->RecalculateLocalMatrix(nullptr);
		else
		{
			ComponentTransform2D* transform2D = (ComponentTransform2D*)GetComponent(ComponentType::TRANSFORM_2D);
			if (transform2D)
				transform2D->RecalculateLocalPos(nullptr);
		}

		/* Remove from current parent. */
		parent->RemoveChild(this);

		LOGGER("Changed parent of '%s' from '%s' to the root GameObject", name.c_str(), parent->name.c_str());

		/* Assign new parent */
		parent = App->scene->root;
		App->scene->root->children.push_back(this);
		parentUuid = parent->uuid;
		return true;
	}

	/* Ensure the newParent is NOT a child of this GameObject or this GameObject itself */
	if (HasGameObjectInChildrenHierarchy(newParent))
	{
		LOGGER("Warning: Attempted to change the parent of GameObject '%s' to '%s'. Operation was aborted because '%s' is a child of '%s'", name.c_str(), newParent->name.c_str(), newParent->name.c_str(), name.c_str());
		return false;
	}
	else
	{
		/* Inform the transform or transform2D (if it exists) about the newParent's transform (if it exists, else nullptr) */
		ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);

		if (transform)
		{
			ComponentTransform* parentTransform = (ComponentTransform*)newParent->GetComponent(ComponentType::TRANSFORM);

			/* Note that parentTransform might be nullptr, but that is a case handled by the Transform::RecalculateLocalMatrix method */
			transform->RecalculateLocalMatrix(parentTransform);
		}
		else
		{
			ComponentTransform2D* transform2D = (ComponentTransform2D*)GetComponent(ComponentType::TRANSFORM_2D);
			if (transform2D)
			{
				ComponentTransform2D* parentTransform2D = (ComponentTransform2D*)newParent->GetComponent(ComponentType::TRANSFORM_2D);
				transform2D->RecalculateLocalPos(parentTransform2D);
			}
		}

		/* Remove from current parent. */
		parent->RemoveChild(this);

		//LOGGER("Changed parent of '%s' from '%s' to '%s'", name.c_str(), parent->name.c_str(), newParent->name.c_str());

		/* Assign new parent */
		parent = newParent;
		parentUuid = parent->uuid;
		parent->children.push_back(this);
		return true;
	}
}

std::vector<Component*> GameObject::GetComponents(ComponentType type)
{
	std::vector<Component*> foundComponents;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			foundComponents.push_back(*it);
		}
	}

	return foundComponents;
}

Component * GameObject::GetComponent(ComponentType type)
{
	BROFILER_CATEGORY("ModuleScene::GetComponent", Profiler::Color::PapayaWhip);
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			return(*it);
		}
	}
	return nullptr;
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;

	int count = 0;
	for (Component* component : components)
	{
		if (component->GetType() == type)
		{
			/* If the maxCount is -1, that means there's no limit */
			if (component->MaxCountInGameObject() == -1)
				break;

			++count;
			if (count >= component->MaxCountInGameObject())
			{
				LOGGER("Warning: Can't add a component of type %s because the limit in a single GameObject is %d", GetString(type), component->MaxCountInGameObject());
				return nullptr;
			}
		}
	}

	//Certain components should be created along with the correspondent transform component
	switch (type)
	{
	case ComponentType::ANIMATION:
		component = new ComponentAnimation(this);
		break;
	case ComponentType::CAMERA:
		component = new ComponentCamera(this);
		break;
	case ComponentType::GRASS:
		component = new ComponentGrass(this);
		break;
	case ComponentType::MATERIAL:
		component = new ComponentMaterial(this);
		break;
	case ComponentType::MESH:
		AddDependingComponent();
		component = new ComponentMesh(this);
		break;
	case ComponentType::PARTICLE_SYSTEM:
		component = new ComponentParticleSystem(this);
		break;
	case ComponentType::TRANSFORM:
		component = new ComponentTransform(this);
		break;
	case ComponentType::TRANSFORM_2D:
		component = new ComponentTransform2D(this);
		break;
	case ComponentType::UI_BUTTON:
		component = new ComponentUiButton(this);
		break;
	case ComponentType::UI_IMAGE:
		component = new ComponentUiImage(this);
		break;
	case ComponentType::UI_INPUT_TEXT:
		component = new ComponentUiInputText(this);
		break;
	case ComponentType::UI_LABEL:
		component = new ComponentUiLabel(this);
		break;
	}

	if (component != nullptr)
		components.push_back(component);

	return component;
}

void GameObject::AddDependingComponent()
{
	bool transformAlreadyExists = false;
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == ComponentType::TRANSFORM)
		{
			transformAlreadyExists = true;
		}
	}
	if (transformAlreadyExists != true)
	{
		AddComponent(ComponentType::TRANSFORM);
	}
}

bool GameObject::RemoveComponent(Component* component)
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (component == (*it))
		{
			if ((*it)->GetType() == ComponentType::TRANSFORM)
			{
				RemoveDependingComponent();
			}

			delete component;
			components.erase(it);
			return true;
		}
	}

	return false;
}

void GameObject::RemoveDependingComponent()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == ComponentType::MESH)
		{
			(*it)->toRemove = true;
			componentPendingToRemove = true;
		}
	}
}

bool GameObject::GetActive() const
{
	if (isActive)
	{
		GameObject* testParent = parent;
		while (testParent)
		{
			if (!testParent->isActive)
				return false;

			testParent = testParent->parent;
		}
		return true;
	}
	else
		return false;
}

void GameObject::SetActive(bool activeState)
{
	isActive = activeState;
}

void GameObject::Save(SerializableObject& obj)
{
	obj.Addu32("UUID", uuid);
	obj.AddString("Name", name);
	obj.Addu32("ParentUUID", parentUuid);
	obj.AddString("Name", name);
	obj.AddBool("Active", isActive);
	SerializableArray sArray = obj.BuildSerializableArray("Components");
	for (Component* c : components)
	{
		SerializableObject sObject = sArray.BuildSerializableObject();
		c->Save(sObject);
	}
}

void GameObject::Load(const SerializableObject& obj)
{
	uuid = obj.Getu32("UUID");
	name = obj.GetString("Name");
	parentUuid = obj.Getu32("ParentUUID");
	name = obj.GetString("Name");
	isActive = obj.GetBool("Active");

	SerializableArray componentsArray = obj.GetSerializableArray("Components");
	uint componentsCount = componentsArray.Size();

	for (uint i = 0; i < componentsCount; ++i)
	{
		SerializableObject componentObject = componentsArray.GetSerializableObject(i);
		std::string cTypeString = componentObject.GetString("Type");
		ComponentType cType = GetComponentType(cTypeString.c_str());
		
		Component* component = AddComponent(cType);
		component->Load(componentObject);
	}

}

u32 GameObject::GetUUID() const
{
	return uuid;
}

u32 GameObject::GetParentUUID() const
{
	return parentUuid;
}

bool GameObject::HasGameObjectInChildrenHierarchy(GameObject * testGameObject)
{
	for (GameObject* child : children)
	{
		if (child == testGameObject || child->HasGameObjectInChildrenHierarchy(testGameObject))
			return true;
	}
	return false;
}

bool GameObject::IsFirstChild()
{
	assert(parent);
	return parent->children[0] == this;
}

bool GameObject::IsLastChild()
{
	assert(parent);
	return parent->children[parent->children.size() - 1] == this;
}

int GameObject::GetIndexInSiblings()
{
	for (uint i = 0; i < parent->children.size(); ++i)
	{
		if (parent->children[i] == this)
			return i;
	}
	return -1;
}

void GameObject::SwapWithPreviousSibling()
{
	assert(!IsFirstChild());
	int idx = GetIndexInSiblings();
	std::swap(parent->children[idx], parent->children[idx - 1]);
}

void GameObject::SwapWithNextSibling()
{
	assert(!IsLastChild());
	int idx = GetIndexInSiblings();
	std::swap(parent->children[idx], parent->children[idx + 1]);
}

GameObject* GameObject::AddEmptyChild()
{
	return (new GameObject("Empty", this));
}

GameObject * GameObject::AddCameraChild()
{
	GameObject* go = new GameObject("Camera", this);
	go->AddComponent(ComponentType::TRANSFORM);
	ComponentCamera* camera = (ComponentCamera*)go->AddComponent(ComponentType::CAMERA);
	camera->SetAsActiveCamera();
	return go;
}

GameObject* GameObject::AddCubeChild()
{
	GameObject* go = new GameObject("Cube", this);
	go->AddComponent(ComponentType::TRANSFORM);
	ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
	mesh->SetActiveModelInfo(0);
	ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
	mat->SetDefaultMaterialConfiguration();
	mat->Apply();
	return go;
}

GameObject* GameObject::AddSphereChild()
{
	GameObject* go = new GameObject("Sphere", this);
	go->AddComponent(ComponentType::TRANSFORM);
	ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
	mesh->SetActiveModelInfo(1);
	ComponentMaterial* mat = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
	mat->SetDefaultMaterialConfiguration();
	mat->Apply();
	return go;
}

GameObject* GameObject::AddUiElement(UIElementType uiElementType)
{
	GameObject* go = App->ui->NewUIElement(uiElementType);
	go->SetParent(this);
	return go;
}

bool GameObject::RemoveChild(GameObject * childToRemove)
{
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (*it == childToRemove)
		{
			children.erase(it);
			return true;
		}
	}
	return false;
}

std::vector<GameObject*> GameObject::GetChildren()
{
	return children;
}

GameObject* GameObject::FindByName(const char * lookUpName)
{
	GameObject* go = this;
	std::vector<GameObject*> children;

	std::stack<GameObject*> stack;
	stack.push(go);

	while (!stack.empty())
	{
		go = stack.top();
		stack.pop();

		if (strcmp(lookUpName, go->name.c_str()) == 0)
		{
			return go;
		}

		children = go->GetChildren();
		for (int i = children.size(); i > 0; i--)
			stack.push(children.at(i - 1));
	}

	return nullptr;
}
