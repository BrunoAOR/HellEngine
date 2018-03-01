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
	if (App->input->GetKey((SDL_SCANCODE_P)) == KeyState::KEY_DOWN) {
		//App->animation->BlendTo(instanceID, "runforwards", 500);
	}


	if (instanceID >= 0) {

		std::stack<GameObject*> stack;

		GameObject* go = gameObject;

		stack.push(go);

		while (!stack.empty()) {
			go = stack.top();
			stack.pop();

			ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
			float3 position = transform->GetPosition();
			Quat rotation = transform->GetRotationQuat();

			App->animation->GetTransform(instanceID, go->name.c_str(), position, rotation);

			transform->SetPosition(position.x, position.y, position.z);
			transform->SetRotation(rotation);

			for (GameObject* child : go->GetChildren()) {
				stack.push(child);
			}
		}
	}
}

void ComponentAnimation::SetAnimation()
{
	instanceID = App->animation->Play(animationName, loop);
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

		if (ImGui::Checkbox("Active", &isActive)) {
			if (!isActive)
				UnsetAnimation();
			else
				SetAnimation();
		}

		ImGui::InputText("Animation name", animationName, 256);

		if (ImGui::Button("Apply"))
		{
			if (isActive)
				SetAnimation();
		}

		ImGui::Checkbox("Loop", &loop);

	}
}

int ComponentAnimation::MaxCountInGameObject()
{
	return 1;
}
