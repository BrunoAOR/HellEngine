#include "ComponentAudioListener.h"
#include "ImGui/imgui.h"
#include "ComponentType.h"

ComponentAudioListener::ComponentAudioListener(GameObject * owner) : Component(owner)
{
	type = ComponentType::AUDIOLISTENER;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
}

ComponentAudioListener::~ComponentAudioListener()
{
}


void ComponentAudioListener::OnEditor()
{

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		
	}
}

int ComponentAudioListener::MaxCountInGameObject()
{
	return 1;
}
