#include <queue>
#include "ImGui/imgui.h"
#include "ComponentTransform2D.h"
#include "ComponentType.h"
#include "GameObject.h"

ComponentTransform2D::ComponentTransform2D(GameObject* owner) : Component(owner)
{
	type = ComponentType::TRANSFORM_2D;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);

	if (GameObject* parent = gameObject->GetParent())
	{
		ComponentTransform2D* parentTransform2D = (ComponentTransform2D*)parent->GetComponent(ComponentType::TRANSFORM_2D);
		/* Passing nullptr to parentTransform2D is valid */
		UpdateWorldPos(parentTransform2D);
	}
}


ComponentTransform2D::~ComponentTransform2D()
{
}

void ComponentTransform2D::RecalculateLocalPos(const ComponentTransform2D* parentTransform2D)
{
	/*
	When changing parent, the worldPos should be kept, but the localPos changes
	All children remain the same, both in localPos and worldPos
	*/
	
	if (!parentTransform2D)
	{
		localPos = worldPos;
	}
	else
	{
		localPos.x = worldPos.x - parentTransform2D->GetWorldPos().x;
		localPos.y = worldPos.y - parentTransform2D->GetWorldPos().y;
	}
}

const fPoint & ComponentTransform2D::GetWorldPos() const
{
	return worldPos;
}

const fPoint & ComponentTransform2D::GetLocalPos() const
{
	return localPos;
}

void ComponentTransform2D::SetLocalPos(const fPoint& newPos)
{
	SetLocalPos(newPos.x, newPos.y);
}

void ComponentTransform2D::SetLocalPos(float x, float y)
{
	/* Adjust worldPos first */
	worldPos.x += x - localPos.x;
	worldPos.y += y - localPos.y;
	/* Adjust localPos second */
	localPos.x = x;
	localPos.y = y;

	UpdateChildrenWorldPos();
}

const fPoint & ComponentTransform2D::GetSize() const
{
	return size;
}

void ComponentTransform2D::SetSize(const fPoint & newSize)
{
	size.x = newSize.x;
	size.y = newSize.y;
}

void ComponentTransform2D::SetSize(float x, float y)
{
	size.x = x;
	size.y = y;
}

void ComponentTransform2D::UpdateWorldPos(const ComponentTransform2D* parentTransform2D)
{
	if (parentTransform2D)
	{
		const fPoint& parentWorldPos = parentTransform2D->GetWorldPos();
		worldPos.x = localPos.x + parentWorldPos.x;
		worldPos.y = localPos.y + parentWorldPos.y;
	}
	else
	{
		worldPos = localPos;
	}
}

void ComponentTransform2D::OnEditor()
{
	static fPoint guiPos;
	static fPoint guiSize;
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		guiPos.x = localPos.x;
		guiPos.y = localPos.y;
		if (ImGui::DragFloat2("Position", guiPos.ptr(), 0.1f))
		{
			SetLocalPos(guiPos);
		}
		guiSize.x = size.x;
		guiSize.y = size.y;
		if (ImGui::DragFloat2("Size", guiSize.ptr(), 0.1f))
		{
			SetSize(guiSize);
		}
	}
}

int ComponentTransform2D::MaxCountInGameObject()
{
	return 1;
}

void ComponentTransform2D::UpdateChildrenWorldPos()
{
	/* A queue is used to ensure every parent's transform is updated before its children's transform */
	std::queue<ComponentTransform2D*> goQueue;
	ComponentTransform2D* currentTransform2D = nullptr;

	goQueue.push(this);

	while (!goQueue.empty())
	{
		currentTransform2D = goQueue.front();
		goQueue.pop();

		std::vector<GameObject*> children = currentTransform2D->gameObject->GetChildren();
		for (GameObject* child : children)
		{
			ComponentTransform2D* childTransform2D = (ComponentTransform2D*)child->GetComponent(ComponentType::TRANSFORM_2D);
			assert(childTransform2D);
			childTransform2D->UpdateWorldPos(currentTransform2D);
			goQueue.push(childTransform2D);
		}
	}
}
