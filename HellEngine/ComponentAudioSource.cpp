#include "ComponentAudioSource.h"
#include "ComponentType.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ImGui/imgui.h"

ComponentAudioSource::ComponentAudioSource(GameObject * owner) : Component(owner)
{
	type = ComponentType::AUDIOSOURCE;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	audioInfo = new AudioInfo();
}

ComponentAudioSource::~ComponentAudioSource()
{
	App->audio->UnloadAudioSource(*this);
}

void ComponentAudioSource::Update()
{
	 
}

void ComponentAudioSource::OnEditor()
{
	static bool loadMessage = false;
	static bool loadedCorrectly = false;
	static char audioPath[256] = "";
	static char audioName[256] = "";

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::InputText("Audio Clip path", audioPath, 256);
		if (audioName != "") {
			ImGui::Text(audioPath, 256);
		}

		if (ImGui::Button("Load")) 
		{

			loadedCorrectly = App->audio->Load(audioPath,this);
			loadMessage = true;
		}

		if (loadMessage) 
		{
			if (loadedCorrectly)
			{
				ImGui::Text("Audio loaded correctly.");
				*audioName = *audioPath;
			}
			else
				ImGui::Text("Audio not found.");
		}
		if(ImGui::Checkbox("Loop", &isMusicLoop))
		{
			//enable Loop with ModuleMusic
		}
		ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Stereo/Mono", &stereoMono, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Pitch", &pitch, -3.0f, 3.0f, "%.2f");
		ImGui::SliderFloat("Stereo Pan", &stereoPan, -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Reverb Zone", &reverbZone, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Doppler", &dopplerLevel, 0.0f, 5.0f, "%.2f");
		ImGui::Combo("Rolloff", &rollOff, "Logarithmic\0Linear\0");
		ImGui::InputFloat("Max Distance", &minDistance);
		ImGui::InputFloat("Max Distance", &maxDistance);

		//Test
		if (ImGui::Checkbox("Play", &playTest))
		{
			Play();
		}

	}
}

bool ComponentAudioSource::Play()
{
	bool willBePlayed = false;

	if (currentAudioSourceState == AudioState::CURRENTLY_STOPPED || currentAudioSourceState == AudioState::LOADED) 
	{
		currentAudioSourceState = AudioState::WAITING_TO_PLAY;
		willBePlayed = true;
	}

	return willBePlayed;
}

bool ComponentAudioSource::Stop()
{
	bool willBeStopped = false;
	if (currentAudioSourceState == AudioState::CURRENTLY_PLAYED)
	{
		currentAudioSourceState = AudioState::WAITING_TO_STOP;
		willBeStopped =  true;
	}
	return willBeStopped;
}

float ComponentAudioSource::GetVolume() const
{
	return volume;
}

float ComponentAudioSource::GetPitch() const
{
	return pitch;
}

float ComponentAudioSource::GetStereoPan() const
{
	return stereoPan;
}

float ComponentAudioSource::GetStereoMono() const
{
	return stereoMono;
}

float ComponentAudioSource::GetDopplerLevel() const
{
	return dopplerLevel;
}

float ComponentAudioSource::GetMinDistance() const
{
	return minDistance;
}

float ComponentAudioSource::GetMaxDistance() const
{
	return 0;
}

void ComponentAudioSource::SetVolume(float volumeValue)
{
	volume = volumeValue;
}

void ComponentAudioSource::SetPitch(float pitchValue)
{
	pitch = pitchValue;
}

void ComponentAudioSource::SetStereoMono(float stereoOrMonoValue)
{
	stereoMono = stereoOrMonoValue;
}

void ComponentAudioSource::SetStereoPan(float panValue)
{
	stereoPan = panValue;
}

void ComponentAudioSource::SetDopplerLevel(float dopplerLevelValue)
{
	dopplerLevel = dopplerLevelValue;
}

void ComponentAudioSource::SetMinDistance(float minDistanceValue)
{
	minDistance = minDistanceValue;
}

void ComponentAudioSource::SetMaxDistance(float maxDistanceValue)
{
	maxDistance = maxDistanceValue;
}

AudioState ComponentAudioSource::GetCurrentState() const
{
	return currentAudioSourceState;
}

void ComponentAudioSource::SetCurrentState(AudioState audioStateValue)
{
	currentAudioSourceState = audioStateValue;
}

int ComponentAudioSource::MaxCountInGameObject()
{
	return -1;
}
