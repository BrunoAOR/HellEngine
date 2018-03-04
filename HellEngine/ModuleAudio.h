#ifndef __H_MODULEAUDIO__
#define __H_MODULEAUDIO__

#include <vector>
#include "Module.h"

enum class AudioFormat { OGG, WAV };
enum class AudioType { EFFECT, MUSIC };

struct AudioInfo
{
	unsigned int audioID  = 0;
	AudioFormat audioFormat;
	AudioType audioType;
};


class ModuleAudio : public Module
{
public:

	ModuleAudio(bool startEnabled = true);
	~ModuleAudio();

	bool Load(const char *audioPath);
	bool Init();
	bool CleanUp();
	UpdateStatus Update();

private:
	const char * ParseBassErrorCode(const int& bassErrorCode);
	std::string ObtainAudioExtension(const char *audioPath);
	AudioInfo *audioInfo = nullptr;

};

#endif /* __H_MODULEAUDIO__ */
