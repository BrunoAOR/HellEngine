#include <stack>
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleInput.h"

ComponentAnimation::ComponentAnimation(GameObject * owner) : Component(owner)
{
	type = ComponentType::ANIMATION;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Update()
{
	if (instanceID != -1)
	{
		std::stack<GameObject*> stack;

		GameObject* go = gameObject;

		stack.push(go);

		while (!stack.empty())
		{
			go = stack.top();
			stack.pop();

			ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
			assert(transform);
			float3 position = transform->GetPosition();
			Quat rotation = transform->GetRotationQuat();

			if (App->animation->GetTransform(instanceID, go->name.c_str(), position, rotation))
			{
				transform->SetPosition(position.x, position.y, position.z);
				transform->SetRotation(rotation);
			}

			for (GameObject* child : go->GetChildren())
			{
				stack.push(child);
			}
		}
	}
}

void ComponentAnimation::SetAnimation()
{
	instanceID = App->animation->Play(animationName, loop);
	if (instanceID != -1)
		App->animation->ModifyAnimationActive(instanceID, isActive);
}

void ComponentAnimation::UnsetAnimation()
{
	App->animation->Stop(instanceID);
	instanceID = -1;
}

void ComponentAnimation::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		if (ImGui::Checkbox("Active", &isActive) && instanceID != -1)
			App->animation->ModifyAnimationActive(instanceID, isActive);


		ImGui::InputText("Animation name", animationName, 256);

		if (ImGui::Button("Apply"))
		{
			if (instanceID != -1)
				App->animation->BlendTo(instanceID, animationName, blendTime);
			else
				SetAnimation();
		}

		if (ImGui::Checkbox("Loop", &loop))
		{
			App->animation->ModifyAnimationLoop(instanceID, loop);
		}

		ImGui::InputInt("Blend time (ms)", &blendTime);
	}
}

int ComponentAnimation::MaxCountInGameObject()
{
	return 1;
}
