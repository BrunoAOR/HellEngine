#ifndef __H_MODULEAUDIO__
#define __H_MODULEAUDIO__

#include <vector>
#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;
typedef struct _Mix_Music Mix_Music;

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool startEnabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();

	/* Play a music file */
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	/* Load a WAV in memory */
	unsigned int LoadFx(const char* path);

	/* Play a previously loaded WAV */
	bool PlayFx(unsigned int fx, int repeat = 0);

private:

	Mix_Music*	music = nullptr;
	std::vector<Mix_Chunk*>	fx;
};

#endif /* __H_MODULEAUDIO__ */
