#include <math.h>
#include <stdlib.h>
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "Jump.h"
#include "ModuleTime.h"

Jump::Jump(GameObject * owner) : ComponentScript(owner)
{
	scriptType = ScriptType::JUMP;
	editorInfo.idLabel = std::string(GetScriptString(scriptType)) + "##" + std::to_string(editorInfo.id);
}

Jump::~Jump()
{
}

void Jump::UpdateScript()
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	elapsedTime += App->time->DeltaTimeMS();
	float jumpHeight = sin(elapsedTime / 1000.f) / 100;
	float3 position = transform->GetPosition();
	transform->SetPosition(position.x, position.y + jumpHeight, position.z);
}
