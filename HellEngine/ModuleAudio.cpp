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

	if (BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL) != TRUE) //default initialization to allow 3D sound
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
	bool loadIsCorrect = false;

	if (audioPath != nullptr) {

		std::string extension = ObtainAudioExtension(audioPath);

		if (extension != " ") {

			if (extension == "wav") 
			{
				unsigned long stream_bassID = 0;
				stream_bassID = BASS_StreamCreateFile(FALSE, audioPath, 0, 0, BASS_SAMPLE_3D | BASS_STREAM_AUTOFREE);

				if (stream_bassID != 0)
				{
					loadIsCorrect = true;

					source->audioInfo->audioID = stream_bassID;
					source->audioInfo->audioType = AudioType::EFFECT;
					source->SetCurrentState(AudioState::LOADED);
					source->SetPreviousState(AudioState::LOADED); //first value

					StoreAudioSource(source);
					StoreAudioFileWithBassID(audioPath, stream_bassID);

				}
				else
				{
					LOGGER("BASS_StreamCreateFile() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
				}
			}
			else if (extension == "ogg")
			{
				unsigned long sample_bassID = 0;
				sample_bassID = BASS_SampleLoad(FALSE, audioPath, 0, 0, 7, BASS_SAMPLE_3D | BASS_SAMPLE_OVER_VOL);

				if (sample_bassID != 0)
				{
					HCHANNEL bassChannelID = BASS_SampleGetChannel(sample_bassID, false);

					loadIsCorrect = true;

					source->audioInfo->audioID = bassChannelID;
					source->audioInfo->audioType = AudioType::MUSIC;
					source->SetCurrentState(AudioState::LOADED);
					source->SetPreviousState(AudioState::LOADED); //first value

					StoreAudioSource(source);
					StoreAudioFileWithBassID(audioPath, bassChannelID);
				}
			}
		}
		else
		{
			LOGGER("Audio file has no valid extension");
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
			break;
		case 1:
			charErrorCode = "memory error";
			break;
		case 2:
			charErrorCode = "can't open the file";
			break;
		case 3:
			charErrorCode = "can't find a free / valid driver";
			break;
		case 4:
			charErrorCode = "the sample buffer was lost";
			break;
		case 5:
			charErrorCode = "invalid handle";
			break;
		case 6:
			charErrorCode = "unsupported sample format";
			break;
		case 7:
			charErrorCode = "invalid position";
			break;
		case 8:
			charErrorCode = "BASS_Init has not been successfully called";
			break;
		case 9:
			charErrorCode = "BASS_Start has not been successfully called";
			break;
		case 10:
			charErrorCode = "SSL/HTTPS support isn't available";
			break;
		case 14:
			charErrorCode = "already initialized/paused/whatever";
			break;
		case 18:
			charErrorCode = "can't get a free channel";
			break;
		case 19:
			charErrorCode = "an illegal type was specified";
			break;
		case 20:
			charErrorCode = "an illegal parameter was specified";
			break;
		case 21:
			charErrorCode = "no 3D support";
			break;
		case 22:
			charErrorCode = "no EAX support";
			break;
		case 23:
			charErrorCode = "illegal device number";
			break;
		case 24:
			charErrorCode = "not playing";
			break;
		case 25:
			charErrorCode = "illegal sample rate";
			break;
		case 27:
			charErrorCode = "the stream is not a file stream";
			break;
		case 29:
			charErrorCode = "no hardware voices available";
			break;
		case 31:
			charErrorCode = "the MOD music has no sequence data";
			break;
		case 32:
			charErrorCode = "no internet connection could be opened";
			break;
		case 33:
			charErrorCode = "couldn't create the file";
			break;
		case 34:
			charErrorCode = "effects are not available";
			break;
		case 37:
			charErrorCode = "requested data/action is not available";
			break;
		case 38:
			charErrorCode = "the channel is/isn't a charErrorCode = decoding channel";
			break;
		case 39:
			charErrorCode = "a sufficient DirectX version is not installed";
			break;
		case 40:
			charErrorCode = "connection timeout";
			break;
		case 41:
			charErrorCode = "unsupported file format";
			break;
		case 42:
			charErrorCode = "unavailable speaker";
			break;
		case 43:
			charErrorCode = "invalid BASS version (used by add-ons)";
			break;
		case 44:
			charErrorCode = "codec is not available/supported";
			break;
		case 45:
			charErrorCode = "the channel/file has ended";
			break;
		case 46:
			charErrorCode = "the device is busy";
			break;
		case -1:
			charErrorCode = "some other mystery problem";
			break;
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
	BASS_Apply3D();
	
	GameObject *goRoot = App->scene->root;
	UpdateAudio(goRoot);
	
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

void ModuleAudio::StoreAudioFileWithBassID(const char *audioPath, unsigned long bassID)
{
	std::string path(audioPath);

	if (!path.empty())
	{
		std::map<std::string, unsigned long>::iterator it = audioFilesByBassID.begin();
		audioFilesByBassID.insert(it, std::pair<std::string, unsigned long>(path, bassID));
	}
}

const int ModuleAudio::CheckLoadingAudioAlreadyUpload(const char * audioPath)
{
	unsigned long loadingAudioBassID = 0;

	std::string path(audioPath);
	
	if (!path.empty())
	{
		std::map<std::string, unsigned long>::iterator it;
		if (audioFilesByBassID.size() != 0)
		{
			it = audioFilesByBassID.find(path);
			if (it != audioFilesByBassID.end()) 
			{
				loadingAudioBassID = it->second;
			}
		}
	}
	return loadingAudioBassID;
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
				float3 audioSourcePos;
				BASS_3DVECTOR audioSourcePosBASS(0,0,0);

				ComponentTransform *transformAudioSource = (ComponentTransform*)audioSource->gameObject->GetComponent(ComponentType::TRANSFORM);

				DecomposeMatrixPosition(transformAudioSource->GetModelMatrix4x4(), audioSourcePos);
				audioSourcePosBASS.x = audioSourcePos.x;
				audioSourcePosBASS.y = audioSourcePos.y;
				audioSourcePosBASS.z = audioSourcePos.z;

				BASS_ChannelSetAttribute(audioID, BASS_ATTRIB_VOL, audioSource->GetVolume());
				BASS_ChannelSetAttribute(audioID, BASS_ATTRIB_PAN, audioSource->GetStereoPan());
				if (audioSource->GetStereoMono() == 0) //Means Stereo is Selected
				{
					BASS_ChannelSet3DAttributes(audioID, BASS_3DMODE_NORMAL,
						audioSource->GetMinDistance(), audioSource->GetMaxDistance(),
						-1, -1, -1);
				}
				else
				{
					BASS_ChannelSet3DAttributes(audioID, BASS_3DMODE_OFF,
						audioSource->GetMinDistance(), audioSource->GetMaxDistance(),
						-1, -1, -1);
				}

				bool bassPlayResponse = BASS_ChannelSet3DPosition(audioID, &audioSourcePosBASS, NULL, NULL);
				if (bassPlayResponse != TRUE)
				{
					LOGGER("BASS_ChannelSet3DPosition() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
					break;
				}

				double trackCurrentTime = BASS_ChannelBytes2Seconds(audioID,(BASS_ChannelGetPosition(audioID, BASS_POS_BYTE)));
				double trackDuration = BASS_ChannelBytes2Seconds(audioID, (BASS_ChannelGetLength(audioID, BASS_POS_BYTE)));
				if (trackCurrentTime >= trackDuration - 1)  // if the audio finishes, it cannot be played again without a new load, so stop will be pausing it just before reaching the end
				{
					if (!audioSource->GetIsLopping())
					{
						audioSource->SetPlayBackState(2); //state stop on audioSource
					}
					else
					{
						audioSource->SetCurrentState(AudioState::WAITING_TO_STOP);
					}
				}
			}
			case AudioState::WAITING_TO_PLAY:
			{
				if (audioSource->GetPreviousState() == AudioState::WAITING_TO_PAUSE)
				{
					if (BASS_ChannelPlay(audioID, false)) {
						audioSource->SetCurrentState(AudioState::CURRENTLY_PLAYED);
						audioSource->SetPreviousState(AudioState::WAITING_TO_PLAY);
						break;
					}
					else
					{
						LOGGER("BASS_ChannelPlay() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
						break;
					}
				}
				else if (audioSource->GetPreviousState() == AudioState::WAITING_TO_STOP || audioSource->GetPreviousState() == AudioState::LOADED)
				{
					if (BASS_ChannelPlay(audioID, true)) {
						audioSource->SetCurrentState(AudioState::CURRENTLY_PLAYED);
						audioSource->SetPreviousState(AudioState::WAITING_TO_PLAY);
						break;
					}
					else
					{
						LOGGER("BASS_ChannelPlay() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
						break;
					}
				}
				break;
			}
			case AudioState::WAITING_TO_PAUSE:
			{
				if (BASS_ChannelPause(audioID))
				{
					LOGGER("BASS_ChannelPause() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
					break;
				}
				else {
					audioSource->SetCurrentState(AudioState::CURRENTLY_PAUSED);
					audioSource->SetPreviousState(AudioState::WAITING_TO_PAUSE);
				}
				break;
			}
			case AudioState::WAITING_TO_STOP:
			{
				if (BASS_ChannelPause(audioID)) //Pause is use instead of Stop because if the channel is stopped, the audio has to be loaded again before playing it
				{
					LOGGER("BASS_ChannelPause() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
					break;
				}
				else {
					audioSource->SetCurrentState(AudioState::CURRENTLY_STOPPED);
					audioSource->SetPreviousState(AudioState::WAITING_TO_STOP);
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
		LOGGER("BASS_Set3DPosition() error: %s", ParseBassErrorCode(BASS_ErrorGetCode()));
	}
}




