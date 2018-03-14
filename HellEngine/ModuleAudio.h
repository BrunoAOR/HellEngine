#ifndef __H_MODULEAUDIO__
#define __H_MODULEAUDIO__

#include <vector>
#include <map>
#include "Module.h"
#include "ComponentAudioSource.h"
#include "ComponentAudioListener.h"

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool startEnabled = true);
	~ModuleAudio();

	bool Load(const char *audioPath, ComponentAudioSource* source);
	bool Init();
	bool CleanUp();
	UpdateStatus PostUpdate();

	void StoreAudioSource(ComponentAudioSource *source );
	void UnloadAudioSource(const ComponentAudioSource &source);
	void StoreAudioFileWithBassID(const char * audioPath, unsigned long bassID);
	const int CheckLoadingAudioAlreadyUpload(const char *audioPath);

	void UpdateAudio(GameObject *go);
	void UpdateAudioSource(ComponentAudioSource *audioSource);
	void UpdateAudioListener(ComponentAudioListener *audioListener);

private:
	const char * ParseBassErrorCode(const int& bassErrorCode);
	std::string ObtainAudioExtension(const char *audioPath);

	std::vector<ComponentAudioSource*> storedAudioSources;
	std::map<std::string, unsigned long> audioFilesByBassID;
	ComponentAudioListener* activeAudioListener = nullptr;

};

#endif /* __H_MODULEAUDIO__ */
