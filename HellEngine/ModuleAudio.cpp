#include "Application.h"
#include "ModuleAudio.h"
#include "globals.h"
#include <string>
#include "Bass/include/bass.h"

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
			charErrorCode = "connection timedout";
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

/* Called before quitting */
bool ModuleAudio::CleanUp()
{
	LOGGER("Freeing Bass system");

	BASS_Free();

	return true;
}

UpdateStatus ModuleAudio::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}


