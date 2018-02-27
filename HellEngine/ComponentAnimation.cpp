#include <stack>
#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"

ComponentAnimation::ComponentAnimation(GameObject * owner) : Component(owner)
{
	type = ComponentType::ANIMATION;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	SetAnimation("Idle");
}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Update()
{
	if (instanceID >= 0) {

		std::stack<GameObject*> stack;

		GameObject* go = gameObject;
		std::vector<GameObject*> children;

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

			children = go->GetChildren();

			for (int i = children.size(); i > 0; i--)
				stack.push(children.at(i - 1));
		}
	}
}

void ComponentAnimation::SetAnimation(const char * name, bool loop)
{
	animationName = name;
	instanceID = App->animation->Play(name, loop);
}

void ComponentAnimation::OnEditor()
{
}

int ComponentAnimation::MaxCountInGameObject()
{
	return 1;
}
