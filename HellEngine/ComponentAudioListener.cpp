#include "ComponentAudioListener.h"
#include "ComponentType.h"

ComponentAudioListener::ComponentAudioListener(GameObject * owner) : Component(owner)
{
	type = ComponentType::AUDIOLISTENER;
}

ComponentAudioListener::~ComponentAudioListener()
{
}
