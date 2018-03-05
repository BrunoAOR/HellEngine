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
		ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%1.0f");
		ImGui::SliderFloat("Stereo/Mono", &stereoMono, 0.0f, 1.0f, "%1.0f");
		ImGui::SliderFloat("Pitch", &pitch, -3.0f, 3.0f, "%1.0f");
		ImGui::SliderFloat("Stereo Pan", &stereoPan, -1.0f, 1.0f, "%1.0f");
		ImGui::SliderFloat("Reverb Zone", &reverbZone, 0.0f, 1.0f, "%1.0f");
		ImGui::SliderFloat("Doppler", &dopplerLevel, 0.0f, 5.0f, "%1.0f");
		ImGui::Combo("Rolloff", &rollOff, "Logarithmic\0Linear\0");
		ImGui::InputInt("Max Distance", &maxDistance);

	}
}

bool ComponentAudioSource::Play()
{
	bool willBePlayed = false;

	if (currentAudioSourceState == AudioState::CURRENTLY_STOPPED) 
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

AudioState ComponentAudioSource::GetCurrentState() const
{
	return currentAudioSourceState;
}

int ComponentAudioSource::MaxCountInGameObject()
{
	return -1;
}
