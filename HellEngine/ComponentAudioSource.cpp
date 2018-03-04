#include "ComponentAudioSource.h"
#include "ComponentType.h"

ComponentAudioSource::ComponentAudioSource(GameObject * owner) : Component(owner)
{
	type = ComponentType::AUDIOSOURCE;
	audioInfo = new AudioInfo();
}

ComponentAudioSource::~ComponentAudioSource()
{
}

void ComponentAudioSource::Update()
{
	 
}

void ComponentAudioSource::OnEditor()
{
}
