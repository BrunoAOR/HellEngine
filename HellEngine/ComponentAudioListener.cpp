#include "ComponentAudioListener.h"
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
}

int ComponentAudioListener::MaxCountInGameObject()
{
	return 1;
}
