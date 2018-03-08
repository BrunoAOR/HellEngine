#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "globals.h"
#include <string>
#include "Bass/include/bass.h"
#include "SDL_mixer/include/SDL_mixer.h"

#pragma comment (lib,"Bass/libx86/bass.lib")

ModuleAudio::ModuleAudio( bool startEnabled) : Module( startEnabled)
{}

/* Destructor */
ModuleAudio::~ModuleAudio()
{}

/* Called before render is available */
bool ModuleAudio::Init()
{
	bool ret = true;
	LOGGER("Loading Audio Mixer");

	if (BASS_Init(-1, 44000, BASS_DEVICE_3D, 0, NULL) != TRUE)
	{
		LOGGER("BASS_Init() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
		ret = false;
	}
	else
	{
		LOGGER("Using Bass %s", BASSVERSIONTEXT);

		int a, count = 0;
		BASS_DEVICEINFO info;
		if (BASS_GetDevice()!=-1) {
			for (a = 0; BASS_GetDeviceInfo(a, &info); a++)
			{
				if (info.flags & BASS_DEVICE_ENABLED)
				{
					LOGGER("Audio device detected: %s", info.name);
				}
			}
		}
		else
		{
			LOGGER("No audio device detected");
		}
		
	}
	return ret;
}

bool ModuleAudio::Load(const char *audioPath, ComponentAudioSource* source)
{
	bool loadIsCorrect;

	if (audioPath != nullptr) {

		unsigned long bassID = 0;
		std::string extension = ObtainAudioExtension(audioPath);

		if (extension != " ") {

			if (extension == "ogg") //effect
			{
				bassID = BASS_StreamCreateFile(FALSE, audioPath, 0, 0, BASS_STREAM_AUTOFREE);
			}
			else if (extension == "wav") //music
			{
				bassID = BASS_SampleLoad(FALSE, audioPath, 0, 0, 7, BASS_SAMPLE_3D || BASS_SAMPLE_OVER_VOL);	
			}

			if (bassID != 0)
			{
				HCHANNEL bassChannelID = BASS_SampleGetChannel(bassID, false);
				loadIsCorrect = true;

				source->audioInfo->audioID = bassChannelID;
				source->audioInfo->audioType = extension == "wav" ? AudioType::MUSIC : AudioType::EFFECT;

				source->SetCurrentState(AudioState::LOADED);
				StoreAudioSource(source);
			}
			else
			{
				LOGGER("BASS_StreamCreateFile() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
				loadIsCorrect = false;
			}
		}
		else
		{
			LOGGER("Audio file has no valid extension");
			loadIsCorrect = false;
		}
		
	}
	return loadIsCorrect;
}

const char* ModuleAudio::ParseBassErrorCode(const int& bassErrorCode) 
{
	char const *charErrorCode = nullptr;
	switch (bassErrorCode) 
	{

		case 0:
			charErrorCode = "all is OK";
		case 1:
			charErrorCode = "memory error";
		case 2:
			charErrorCode = "can't open the file";
		case 3:
			charErrorCode = "can't find a free / valid driver";
		case 4:
			charErrorCode = "the sample buffer was lost";
		case 5:
			charErrorCode = "invalid handle";
		case 6:
			charErrorCode = "unsupported sample format";
		case 7:
			charErrorCode = "invalid position";
		case 8:
			charErrorCode = "BASS_Init has not been successfully called";
		case 9:
			charErrorCode = "BASS_Start has not been successfully called";
		case 10:
			charErrorCode = "SSL/HTTPS support isn't available";
		case 14:
			charErrorCode = "already initialized/paused/whatever";
		case 18:
			charErrorCode = "can't get a free channel";
		case 19:
			charErrorCode = "an illegal type was specified";
		case 20:
			charErrorCode = "an illegal parameter was specified";
		case 21:
			charErrorCode = "no 3D support";
		case 22:
			charErrorCode = "no EAX support";
		case 23:
			charErrorCode = "illegal device number";
		case 24:
			charErrorCode = "not playing";
		case 25:
			charErrorCode = "illegal sample rate";
		case 27:
			charErrorCode = "the stream is not a file stream";
		case 29:
			charErrorCode = "no hardware voices available";
		case 31:
			charErrorCode = "the MOD music has no sequence data";
		case 32:
			charErrorCode = "no internet connection could be opened";
		case 33:
			charErrorCode = "couldn't create the file";
		case 34:
			charErrorCode = "effects are not available";
		case 37:
			charErrorCode = "requested data/action is not available";
		case 38:
			charErrorCode = "the channel is/isn't a charErrorCode = decoding channel";
		case 39:
			charErrorCode = "a sufficient DirectX version is not installed";
		case 40:
			charErrorCode = "connection timeout";
		case 41:
			charErrorCode = "unsupported file format";
		case 42:
			charErrorCode = "unavailable speaker";
		case 43:
			charErrorCode = "invalid BASS version (used by add-ons)";
		case 44:
			charErrorCode = "codec is not available/supported";
		case 45:
			charErrorCode = "the channel/file has ended";
		case 46:
			charErrorCode = "the device is busy";
		case -1:
			charErrorCode = "some other mystery problem";
	}
	return charErrorCode;
}

std::string ModuleAudio::ObtainAudioExtension(const char * audioPath)
{
	std::string audioFilePath(audioPath);
	int lastPeriod = 0;
	lastPeriod = audioFilePath.find_last_of(".");

	std::string audioExtension = " ";
	if (lastPeriod != 0) {
		audioExtension = audioFilePath.substr(lastPeriod + 1);
	}

	return audioExtension;

}

/* Called before quitting */
bool ModuleAudio::CleanUp()
{
	LOGGER("Freeing Bass system");

	BASS_Free();

	return true;
}

UpdateStatus ModuleAudio::PostUpdate()
{
	GameObject *goRoot = App->scene->root;
	UpdateAudio(goRoot);
	
	BASS_Apply3D();
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleAudio::UpdateAudio(GameObject *gameObject)
{
	std::stack<GameObject*> stack;

	GameObject* go = gameObject;
	std::vector<GameObject*> children;

	stack.push(go);

	while (!stack.empty()) {
		go = stack.top();
		stack.pop();

		ComponentAudioSource *audioSource = (ComponentAudioSource*)go->GetComponent(ComponentType::AUDIOSOURCE);
		if (audioSource != nullptr)
		{
			UpdateAudioSource(audioSource);
		}

		ComponentAudioListener *audioListener = (ComponentAudioListener*)go->GetComponent(ComponentType::AUDIOLISTENER);
		if (audioListener != nullptr)
		{
			UpdateAudioListener(audioListener);
		}

		for (GameObject* child : go->GetChildren())
		{
			stack.push(child);
		}
	}	
}

void ModuleAudio::StoreAudioSource(ComponentAudioSource *source)
{
	storedAudioSources.push_back(source);
}

void ModuleAudio::UnloadAudioSource(const ComponentAudioSource &source)
{
	
	for (std::vector<ComponentAudioSource*>::iterator it = storedAudioSources.begin(); it != storedAudioSources.end(); ) {
		if (*it == &source) {
			//TODO check if call BASS_ChannelStop is needed
			it = storedAudioSources.erase(it);
			break;
		}
		else {
			++it;
		}
	}
}


void ModuleAudio::UpdateAudioSource(ComponentAudioSource * audioSource)
{
	unsigned long audioID = audioSource->audioInfo->audioID;

	if (audioID != 0)
	{
		switch (audioSource->GetCurrentState())
		{
			case AudioState::CURRENTLY_PLAYED:
			{
				BASS_ChannelSetAttribute(audioID, BASS_ATTRIB_VOL, audioSource->GetVolume());
				BASS_ChannelSetAttribute(audioID, BASS_SAMPLE_LOOP, audioSource->GetIsLopping());

				float3 audioSourcePos;
				/*BASS_3DVECTOR audioSourcePosBASS(0,0,0);
				BASS_3DVECTOR audioSourceOrBASS(0, 0, 0);
				BASS_3DVECTOR audioSourceVelBASS(0, 0, 0);*/

				ComponentTransform *transformAudioSource = (ComponentTransform*)audioSource->gameObject->GetComponent(ComponentType::TRANSFORM);

				DecomposeMatrixPosition(transformAudioSource->GetModelMatrix4x4(), audioSourcePos);
			/*	audioSourcePosBASS.x = audioSourcePos.x;
				audioSourcePosBASS.y = audioSourcePos.y;
				audioSourcePosBASS.z = audioSourcePos.z;*/

				if (audioSource->GetStereoMono() == 0) //Means Stereo is Selected
				{
					BASS_ChannelSet3DAttributes(audioID, BASS_3DMODE_NORMAL,
						audioSource->GetMinDistance(), audioSource->GetMaxDistance(),
						360, 360, 1);
				}
				else
				{
					BASS_ChannelSet3DAttributes(audioID, BASS_3DMODE_OFF,
						audioSource->GetMinDistance(), audioSource->GetMaxDistance(),
						-1, -1, -1);
				}

				bool bassPlayResponse = BASS_ChannelSet3DPosition(audioID, NULL, NULL, NULL);
				if (bassPlayResponse != TRUE)
				{
					if(BASS_ErrorGetCode() == 45) {
						audioSource->SetCurrentState(AudioState::WAITING_TO_STOP);
					}
					else
					{
						LOGGER("BASS_Init() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
						break;
					}
				}
				break;
			}
			case AudioState::WAITING_TO_PLAY:
			{
				if (BASS_ChannelPlay(audioID, false) != TRUE)
				{
					LOGGER("BASS_Init() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
					break;
				}
				else
				{
					BASS_ChannelSetAttribute(audioID, BASS_ATTRIB_VOL, audioSource->GetVolume());
					BASS_ChannelSetAttribute(audioID, BASS_ATTRIB_PAN, audioSource->GetStereoPan());
					//BASS_ATTRIB_MUSIC_SPEED()
				}
				audioSource->SetCurrentState(AudioState::CURRENTLY_PLAYED);
				break;
			}
			case AudioState::WAITING_TO_STOP:
			{
				if (BASS_ChannelStop(audioID) != TRUE)
				{
					LOGGER("BASS_Init() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
					break;
				}
				else {
					audioSource->SetCurrentState(AudioState::CURRENTLY_STOPPED);
				}
				break;
			}
		}
	}
	
}

void ModuleAudio::UpdateAudioListener(ComponentAudioListener * audioListener)
{
	float3 audioListenerPos;
	BASS_3DVECTOR audioListenerPosBASS;

	ComponentTransform *transformAudioSource = (ComponentTransform*)audioListener->gameObject->GetComponent(ComponentType::TRANSFORM);
	float4x4 listenerWorldMatrix = transformAudioSource->GetModelMatrix4x4();

	float4 forwardFloat4;
	float4 upFloat4;
	float3 forward;
	float3 up;

	DecomposeMatrixPosition(listenerWorldMatrix, audioListenerPos);
	audioListenerPosBASS.x = audioListenerPos.x;
	audioListenerPosBASS.y = audioListenerPos.y;
	audioListenerPosBASS.z = audioListenerPos.z;

	upFloat4 = listenerWorldMatrix.Mul(float4::unitY).Normalized();
	forwardFloat4 = listenerWorldMatrix.Mul(float4::unitZ).Normalized();
	forward = FLOAT4_TO_POINT(forwardFloat4);
	up = FLOAT4_TO_POINT(upFloat4);

	if (BASS_Set3DPosition(&audioListenerPosBASS, NULL, (BASS_3DVECTOR*)&forward, 
		(BASS_3DVECTOR*)&up) != TRUE) //still lacks go velocity
	{
		LOGGER("BASS_Init() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
	}
}

void ModuleAudio::UpdateActiveAudioListener(ComponentAudioListener & listener)
{


}




